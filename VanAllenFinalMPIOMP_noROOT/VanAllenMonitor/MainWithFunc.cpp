//
//  MainWithFunc.cpp
//  VanAllen
//
//  Created by Gabriele Gaetano Fronzé on 06/03/15.
//  Copyright (c) 2015 Gabriele Gaetano Fronzé. All rights reserved.
//

#ifndef define
#include "Functions.h"
#include "Tags.h"
#include "mpi.h"
#include <iostream>
#endif

using namespace std;

int main(int argc, char* argv[]){
    /*Parametri di funzionamento di MPI*/
    int size;
    int rank;
    
    /*Numero standard di particelle test della simulazione*/
    int NelMin=0;
    int NelMax=400;
    
    /*Dimensione standard del task*/
    int TaskMultiplier=4;
    
    /*Presenza del monitor*/
    int Monitor=1; //1= utilizzo del monitor
    
    /*Seed default*/
    unsigned int Seed=1234;
    
    /*Avvio di MPI*/
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    /*Rank di Master e Finalizer*/
    int mMaster=0;
    int mFinalizer=size-1;
    
    /*Inizializzazione dei dati*/
    if(argc>=2) NelMin = atoi(argv[1]);
    if(argc>=3) NelMax = atoi(argv[2]);
    if(argc>=4) TaskMultiplier = atoi(argv[3]);
    if(argc>=5) Monitor = atoi(argv[4]);
    if(argc>=6) Seed = atoi(argv[5]);
    if(argc>=7) mMaster = atoi(argv[6]);
    if(argc>=8) mFinalizer = atoi(argv[7]);
    
    int WorkerNumber;
    int rankexclusions[2];
    int exclusionscount;
    bool noMaster=(mMaster==mFinalizer);
    
    if(noMaster){
        WorkerNumber=size-1;
        rankexclusions[0]=mFinalizer;
        exclusionscount=1;
    }else{
        WorkerNumber=size-2;
        rankexclusions[0]=mMaster;
        rankexclusions[1]=mFinalizer;
        exclusionscount=2;
    }
    
    if(mMaster>=size || mMaster<0){
        if(rank==0)printf("!!! ERROR !!! Master rank not valid %d\n"
                          "\tUsage: mpiexec [-np number_of_machines][…] VanAllen [first_electron=0] [last_electron=400] [task_multiplier=4] [monitor_onoff=1] [Seed=1234] [master_rank=0] [finalizer_rank=size-1]\n"
                          "\tWhere values after '=' are ment to be default ones, taken if no other value has been inserted.\n",mMaster);
        MPI_Finalize();
        return 0;
    }
    if(mFinalizer>=size || mFinalizer<0){
        if(rank==0)printf("!!! ERROR !!! Finalizer rank not valid %d\n"
                          "\tUsage: mpiexec [-np number_of_machines][…] VanAllen [first_electron=0] [last_electron=400] [task_multiplier=4] [monitor_onoff=1] [Seed=1234] [master_rank=0] [finalizer_rank=size-1]\n"
                          "\tWhere values after '=' are ment to be default ones, taken if no other value has been inserted.\n",mFinalizer);
        MPI_Finalize();
        return 0;
    }
    if(WorkerNumber<=0){
        if(rank==0)printf("!!! ERROR !!! Size not valid %d\n"
                          "\tUsage: mpiexec [-np number_of_machines][…] VanAllen [first_electron=0] [last_electron=400] [task_multiplier=4] [monitor_onoff=1] [Seed=1234] [master_rank=0] [finalizer_rank=size-1]\n"
                          "\tWhere values after '=' are ment to be default ones, taken if no other value has been inserted.\n",size);
        MPI_Finalize();
        return 0;
    }
    
    MPI_Group worldgroup,workergroup,gathergroup;
    MPI_Comm MPI_COMM_WORKERS,MPI_COMM_GATHER;
    MPI_Comm_group(MPI_COMM_WORLD,&worldgroup);
    MPI_Group_excl(worldgroup,exclusionscount,rankexclusions,&workergroup);
    MPI_Comm_create(MPI_COMM_WORLD,workergroup,&MPI_COMM_WORKERS);
    MPI_Comm_group(MPI_COMM_WORLD,&gathergroup);
    if(!noMaster)MPI_Group_excl(gathergroup,1,&mMaster,&gathergroup);
    MPI_Comm_create(MPI_COMM_WORLD,gathergroup,&MPI_COMM_GATHER);
    
    //_____________________________________________________________________________________________________________//
    /*Processo Master																							   */
    //_____________________________________________________________________________________________________________//
    if(rank==mMaster && !noMaster){
        FunctionMaster(size,NelMin,NelMax,TaskMultiplier,mMaster,mFinalizer,WorkerNumber,Monitor,Seed);
    //_____________________________________________________________________________________________________________//
    /*Processo Finalizer																	                       */
    //_____________________________________________________________________________________________________________//
    }else if(rank==mFinalizer){
        int wFinalizer;
        MPI_Comm_rank(MPI_COMM_GATHER,&wFinalizer);
        if(noMaster){
            FunctionMasterFinalizer(size, NelMin, NelMax, TaskMultiplier, mMaster, mFinalizer, WorkerNumber, wFinalizer, MPI_COMM_GATHER,Monitor,Seed);
        }
        else{
            FunctionFinalizer(size,NelMin,NelMax,TaskMultiplier,mMaster,mFinalizer,WorkerNumber,wFinalizer,MPI_COMM_GATHER);
        }
    //_____________________________________________________________________________________________________________//
    /*Processo Worker																						       */
    //_____________________________________________________________________________________________________________//
    }else{
        int wrank;
        MPI_Comm_rank(MPI_COMM_GATHER,&wrank);
        FunctionWorker(size,NelMin,NelMax,TaskMultiplier,mMaster,mFinalizer,WorkerNumber,rank,wrank,MPI_COMM_WORKERS,MPI_COMM_GATHER);
    }
    
    
    if(rank!=mMaster && rank!=mFinalizer){
        MPI_Comm_free(&MPI_COMM_WORKERS);
        MPI_Group_free(&workergroup);
    }
    if(rank!=mMaster){
        MPI_Comm_free(&MPI_COMM_GATHER);
        MPI_Group_free(&gathergroup);
    }
    
    MPI_Finalize();
    return 0;
}

