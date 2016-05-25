//
//  Functions.h
//  VanAllen
//
//  Created by Gabriele Gaetano Fronzé on 05/03/15.
//  Copyright (c) 2015 Gabriele Gaetano Fronzé. All rights reserved.
//

#ifndef VanAllen_Functions_h
#define VanAllen_Functions_h

#include "mpi.h"

void FunctionMaster         (int size,int NelMin,int NelMax,int TaskMultiplier,int Master,int Finalizer,int WorkerNumber,int monitor,unsigned int Seed=1234);
void FunctionWorker         (int size,int NelMin,int NelMax,int TaskMultiplier,int Master,int Finalizer,int WorkerNumber,int rank,int workerrank,MPI_Comm workercomm,MPI_Comm gathercomm);
void FunctionFinalizer      (int size,int NelMin,int NelMax,int TaskMultiplier,int Master,int Finalizer,int WorkerNumber,int wFinalizer,MPI_Comm gathercomm);
void FunctionMasterFinalizer(int Size,int NelMin,int NelMax,int TaskMultiplier,int Master,int Finalizer,int WorkerNumber,int wFinalizer,MPI_Comm gathercomm,int monitor=0,unsigned int Seed=1234);

#endif
