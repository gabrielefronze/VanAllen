//
//  FunctionWorker.cpp
//  VanAllen
//
//  Created by Gabriele Gaetano Fronzé on 05/03/15.
//  Copyright (c) 2015 Gabriele Gaetano Fronzé. All rights reserved.
//

#ifndef define
#include "Functions.h"
#include "Tags.h"
#include "Params.h"
#include "DiffFuncVector.h"
#include <omp.h>
#include <stdio.h>
#include <vector>
#include "mpi.h"
#include <unistd.h>
#include "timer.h"
#endif

using namespace std;

void FunctionWorker(int size,int NelMin,int NelMax,int TaskMultiplier,int Master,int Finalizer,int WorkerNumber,int rank,int workerrank,MPI_Comm workercomm,MPI_Comm gathercomm){
    
    unsigned long long start_time=GetTimeMs64();
    clock_t start_clock=clock();
    
    /*Ricezione del rank del finalizer per indirizzare l'ultima comunicazione collettiva*/
    int wFinalizer=0;
    int seed=0;
    MPI_Bcast(&wFinalizer,1,MPI_INT,Finalizer,MPI_COMM_WORLD);
    MPI_Bcast(&seed, 1, MPI_INT, Master, MPI_COMM_WORLD);
    
    /*Variabili di controllo e conteggio*/
    bool KeepAlive=true;
    int callcount=0;
    int elcount=0;
    int thnum=0;
    int sendcount=0;
    
    /*Dimensione dei pool di thread da avviare*/
    int threadnumber=1;//omp_get_num_procs();
    
    /*Simulazione di diverso hardware*/
    //if(rank==3)threadnumber=1;
    //if(rank==2)threadnumber=6;
    
    MPI_Ssend(&threadnumber, 1, MPI_INT, Master, NProcess, MPI_COMM_WORLD);
    
    /*Variabili di buffering*/
    int masterelcount;
    int dummy;
    
    /*Variabile di storage dei dati*/
    vector<double> resarr;
    
    /*Variabili MPI per l'handling delle comunicazioni*/
    MPI_Status status;
    MPI_Request req;
    
    /*Il Worker continua a lavorare fino a quando il master non gli dice di smettere*/
    while(KeepAlive){
        double Time[2];
        unsigned long long end_time=GetTimeMs64();
        Time[0]=(double)(end_time-start_time)/1000;
        Time[1]=(double)((clock()-start_clock)/CLOCKS_PER_SEC);
        /*Richiesta dei parametri della task da eseguire*/
        MPI_Ssend(Time,2,MPI_DOUBLE,Master,TaskQuery,MPI_COMM_WORLD);
        MPI_Recv(&masterelcount,1,MPI_INT,Master,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        
        /*Se il Master ha dato l'ok viene eseguito un job in parallelo*/
        if(status.MPI_TAG==GoOn){
            /*Contatore per l'attribuzione dell'RNG corretto*/
            int taskelcount=0;
            int thrank;
            
            /*Simulazione di velocità di calcolo diverse*/
            //if(rank==3) usleep(1000);
            //if(rank==2) usleep(1500);
            
            int maxelectron=0;
            if(masterelcount+threadnumber*TaskMultiplier>NelMax)
                maxelectron=NelMax-masterelcount;
            else
                maxelectron=threadnumber*TaskMultiplier;
            
            /*La simulazione viene eseguita come unfor*/
            for(int h=0;h<maxelectron;h++){
                thrank=omp_get_thread_num();
                int localelcount;
                localelcount=masterelcount+h;
                
                vector<double> bufferarr;
                DiffusionFunctionVector(bufferarr, localelcount, seed);
                resarr.insert(resarr.end(),bufferarr.begin(),bufferarr.end());
                elcount++;
                
                bufferarr.clear();
            }
            callcount++;
        }else KeepAlive=false;
    }
    
    int StatusWorker=0;
    if(wFinalizer==StatusWorker) StatusWorker=1;

    if(elcount!=0){
        /*Vectore variabili per le comunicazioni collettive*/
        vector<double> localsum(steps);
        vector<double> globalsum(steps);
        vector<double> devst(steps);
        int totalel=0;
        
        /*L'indipendenza dell'output permette lo svolgimento in parallelo degli accumuli*/
#pragma omp parallel for num_threads(threadnumber) schedule(guided)
        for(int st=0;st<steps;st++){
            for(int el=0;el<elcount;el++){
                localsum[st]+=resarr[el*steps+st];
            }
        }
        
        /*Viene comunicato l'array locale di accumuli e ricevuta la somma complessiva*/
        if(workerrank==StatusWorker){
            MPI_Isend(&dummy, 1, MPI_INT, Master, Mean, MPI_COMM_WORLD, &req);
            MPI_Request_free(&req);
        }
        if(WorkerNumber>1) MPI_Allreduce(&localsum[0],&globalsum[0],steps,MPI_DOUBLE,MPI_SUM,workercomm);
        else globalsum=localsum;
        if(workerrank==StatusWorker){
            MPI_Isend(&dummy, 1, MPI_INT, Master, Mean, MPI_COMM_WORLD, &req);
            MPI_Request_free(&req);
        }
        
        localsum.clear();
        localsum.shrink_to_fit();
        
        /*Come sopra*/
#pragma omp parallel for num_threads(threadnumber) schedule(guided)
        for(int st=0;st<steps;st++){
            double mean=globalsum[st]/(double)(NelMax-NelMin);
            for(int el=0;el<elcount;el++){
                devst[st]+=(resarr[el*steps+st]-mean)*(resarr[el*steps+st]-mean);
            }
        }
        
        /*L'array di deviazioni standard viene accumulato nel finalizer*/
        if(workerrank==StatusWorker && Master!=Finalizer){
            MPI_Isend(&dummy, 1, MPI_INT, Master, DevSt, MPI_COMM_WORLD, &req);
            MPI_Request_free(&req);
        }

        MPI_Reduce(&devst[0],NULL,steps,MPI_DOUBLE,MPI_SUM,wFinalizer,gathercomm);
        
        if(workerrank==StatusWorker && Master!=Finalizer){
            MPI_Isend(&dummy, 1, MPI_INT, Master, DevSt, MPI_COMM_WORLD, &req);
            MPI_Request_free(&req);
        }
        
        /*Deallocazione dei vector*/
        resarr.clear();
        resarr.shrink_to_fit();
        globalsum.clear();
        globalsum.shrink_to_fit();
        devst.clear();
        devst.shrink_to_fit();
    }else{
        /*Il worker deve comunque partecipare alle comunicazioni collettive,senza alterarle*/
        vector<double> localsum(steps);
        fill(localsum.begin(),localsum.end(),0.0);
        vector<double> globalsum(steps);
        fill(globalsum.begin(),globalsum.end(),0.0);
        vector<double> devst(steps);
        fill(devst.begin(),devst.end(),0.0);
        int totalel=0;
        
        /*Comunicazioni collettive tra i worker ed il Finalizer*/
        if(workerrank==StatusWorker){
            MPI_Isend(&dummy, 1, MPI_INT, Master, Mean, MPI_COMM_WORLD, &req);
            MPI_Request_free(&req);
        }
        if(WorkerNumber>1) MPI_Allreduce(&localsum[0],&globalsum[0],steps,MPI_DOUBLE,MPI_SUM,workercomm);
        else globalsum=localsum;
        if(workerrank==StatusWorker){
            MPI_Isend(&dummy, 1, MPI_INT, Master, Mean, MPI_COMM_WORLD, &req);
            MPI_Request_free(&req);
        }

        if(workerrank==StatusWorker && Master!=Finalizer){
            MPI_Isend(&dummy, 1, MPI_INT, Master, DevSt, MPI_COMM_WORLD, &req);
            MPI_Request_free(&req);
        }
        MPI_Reduce(&devst[0],NULL,steps,MPI_DOUBLE,MPI_SUM,wFinalizer,gathercomm);
        if(workerrank==StatusWorker && Master!=Finalizer){
            MPI_Isend(&dummy, 1, MPI_INT, Master, DevSt, MPI_COMM_WORLD, &req);
            MPI_Request_free(&req);
        }
        
        /*Deallocazione dei vector*/
        localsum.clear();
        localsum.shrink_to_fit();
        globalsum.clear();
        globalsum.shrink_to_fit();
        devst.clear();
        devst.shrink_to_fit();
    }
    return;
}
