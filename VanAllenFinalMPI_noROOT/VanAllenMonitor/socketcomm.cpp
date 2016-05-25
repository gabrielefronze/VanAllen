//
//  socketcomm.h
//  VanAllenMonitor
//
//  Created by Gabriele Gaetano Fronzé on 01/04/15.
//  Copyright (c) 2015 Gabriele Gaetano Fronzé. All rights reserved.
//

#include "socketcomm.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int initClient(){
    int portno,socketID;
    struct sockaddr_in serv_addr;
    struct hostent *server=new hostent;
    portno = 51717;
    socketID = socket(AF_INET, SOCK_STREAM, 0);
    char hostname[10]={'l','o','c','a','l','h','o','s','t','\0'};
    server = gethostbyname(hostname);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);
    connect(socketID,(struct sockaddr *) &serv_addr,sizeof(serv_addr));
    delete server;
    return socketID;
}

int initServer(){
    int portno,socketID,newsocketID;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    socketID = socket(AF_INET, SOCK_STREAM, 0);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 51717;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    bind(socketID, (struct sockaddr *) &serv_addr,sizeof(serv_addr));
    listen(socketID,5);
    clilen = sizeof(cli_addr);
    newsocketID = accept(socketID,(struct sockaddr *) &cli_addr,&clilen);
    return newsocketID;
}

int closesocket(int &socketID){
    return close(socketID);
}

int socketwrite(char buf,int socketID){
    return write(socketID,&buf,1);
}

int socketsend(int num, int fd){
    int size=256;
    std::string s = std::to_string(num);
    char const *buffer = s.c_str();
    int returnvalue=write(fd,buffer,size);
    return returnvalue;
}

int socketsend(int num[], int dim, int fd){
    int returnvalue=0;
    for(int i=0;i<dim;i++){
        returnvalue+=socketsend(num[i], fd);
    }
    return returnvalue;
}

int socketsend(double num, int fd){
    int size=sizeof(double);
    std::string s = std::to_string(num);
    char const *buffer = s.c_str();
    int returnvalue=write(fd,buffer,size);
    return returnvalue;
}

int socketsend(double num[], int dim, int fd){
    int returnvalue=0;
    for(int i=0;i<dim;i++){
        returnvalue+=socketsend(num[i], fd);
    }
    return returnvalue;
}

int socketreceive(int &num, int fd){
    int size=256;
    char *buffer=new char[size];
    bzero(buffer,size);
    int returnvalue=read(fd,buffer,size);
    num=atoi(buffer);
    delete[] buffer;
    return returnvalue;
}

int socketreceive(int num[], int dim, int fd){
    int returnvalue=0;
    for(int i=0;i<dim;i++){
        returnvalue+=socketreceive(num[i], fd);
    }
    return returnvalue;
}

int socketreceive(double &num, int fd){
    int size=sizeof(double);
    char *buffer=new char[size];
    bzero(buffer,size);
    int returnvalue=read(fd,buffer,size);
    num=atof(buffer);
    delete[] buffer;
    return returnvalue;
}

int socketreceive(double num[], int dim, int fd){
    int returnvalue=0;
    for(int i=0;i<dim;i++){
        returnvalue+=socketreceive(num[i], fd);
    }
    return returnvalue;
}
