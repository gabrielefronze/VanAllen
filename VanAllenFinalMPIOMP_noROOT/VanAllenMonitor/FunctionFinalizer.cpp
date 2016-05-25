//
//  FunctionFinalizer.cpp
//  VanAllen
//
//  Created by Gabriele Gaetano Fronzé on 05/03/15.
//  Copyright (c) 2015 Gabriele Gaetano Fronzé. All rights reserved.
//

#ifndef define
#include "Functions.h"
#include "Tags.h"
#include "Params.h"
#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include "mpi.h"
#endif

using namespace std;

void FunctionFinalizer(int size,int NelMin,int NelMax,int TaskMultiplier,int Master,int Finalizer,int WorkerNumber,int wFinalizer,MPI_Comm gathercomm){
    
    /*Variabili per l'handling delle comunicazioni*/
    MPI_Status status;
    MPI_Request req;
    int dummy=0;
    int totalel=0;
    
    MPI_Bcast(&wFinalizer,1,MPI_INT,Finalizer,MPI_COMM_WORLD);
    MPI_Bcast(&dummy,1,MPI_INT,Master,MPI_COMM_WORLD);
    
    /*Variabili di buffer e di conteggio*/
    int threadnumber=omp_get_num_procs();
    
    vector<double> alldevst(steps);
    vector<double> devst(steps);
    fill(devst.begin(),devst.end(),0.0);
    fill(alldevst.begin(),alldevst.end(),0.0);
    
    MPI_Reduce(&devst[0],&alldevst[0],steps,MPI_DOUBLE,MPI_SUM,wFinalizer,gathercomm);
    
    /*Deallocazione*/
    devst.clear();
    devst.shrink_to_fit();
    alldevst.clear();
    alldevst.shrink_to_fit();
    
    //stoppy.Stop();

    return;
}
