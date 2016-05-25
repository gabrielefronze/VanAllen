//
//  socketcomm.h
//  VanAllenMonitor
//
//  Created by Gabriele Gaetano Fronzé on 01/04/15.
//  Copyright (c) 2015 Gabriele Gaetano Fronzé. All rights reserved.
//

#ifndef VanAllenMonitor_socketcomm_h
#define VanAllenMonitor_socketcomm_h

int initClient();

int initServer();

int closesocket(int &socketID);

int socketwrite(char buf,int socketID);

int socketsend(int num, int fd);

int socketsend(int num[], int dim, int fd);

int socketsend(double num, int fd);

int socketsend(double num[], int dim, int fd);

int socketreceive(int &num, int fd);

int socketreceive(int num[], int dim, int fd);

int socketreceive(double &num, int fd);

int socketreceive(double num[], int dim, int fd);

#endif
