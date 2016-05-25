#compiler definition
CC=clang++
MPICC=~/clangomp/build/Release+Asserts/bin/clang++

#flags declaration
CFLAGS=-lncurses -O3
CFLAGSSEQ=-std=c++11  -O3
MPICFLAGS=-I/usr/local/Cellar/open-mpi/1.8.4/include -L/usr/local/opt/libevent/lib -L/usr/local/Cellar/open-mpi/1.8.4/lib -lmpi_cxx -lmpi -fopenmp -std=c++11 -O3

all: VanAllen Monitor VanAllenSeq

VanAllen: MainWithFunc.cpp FunctionMaster.cpp FunctionWorker.cpp FunctionFinalizer.cpp FunctionMasterFinalizer.cpp socketcomm.cpp
	$(MPICC) $(MPICFLAGS) MainWithFunc.cpp FunctionMaster.cpp FunctionWorker.cpp FunctionFinalizer.cpp FunctionMasterFinalizer.cpp socketcomm.cpp -o VanAllen

Monitor: VanAllenMonitor.cpp socketcomm.cpp
	$(CC) $(CFLAGS) VanAllenMonitor.cpp socketcomm.cpp -o Monitor

VanAllenSeq: Sequential.cpp
	$(CC) $(CFLAGSSEQ) Sequential.cpp -o VanAllenSeq

clean:
	rm VanAllen Monitor VanAllenSeq