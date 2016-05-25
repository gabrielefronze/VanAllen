//
//  VanAllenMonitor.cpp
//  VanAllenMonitor
//
//  Created by Gabriele Gaetano Fronzé on 30/03/15.
//  Copyright (c) 2015 Gabriele Gaetano Fronzé. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ncurses.h>
#include "Tags.h"
#include "socketcomm.h"
#include "MonitorPrints.h"

using namespace std;

int main(){
    int newsockfd;
    newsockfd=initServer();

    int Master=0;
    int Finalizer=0;
    int Size=0;
    int Nel=0;
    int TaskMultiplier=0;
    int totalTaskCount=0;
    
    LoadBorders();
    
    initscr();
    WINDOW *welcome=newwin(31,80,2,2);
    WINDOW *message=newwin(10, 80, 31, 2);
    WINDOW *monitor;
    
    for(;;){
        char bufferInstr[1];
        bzero(bufferInstr,1);
        read(newsockfd,bufferInstr,1);
        
        if(bufferInstr[0]==Welcome){
            int bufferInt;
            socketreceive(Master, newsockfd);
            socketreceive(Finalizer, newsockfd);
            socketreceive(Size, newsockfd);
            socketreceive(Nel, newsockfd);
            socketreceive(TaskMultiplier, newsockfd);
            
            wclear(welcome);
            wclear(message);
            wclear(monitor);
            PrintWelcome(welcome,Nel, Size, Master, Finalizer);
            WelcomeBorder(welcome);
            wprintw(message,"\n   RUNTIME MESSAGES:\n");
            MessageBorder(message);
            MonitorBorder(monitor, Size);
            wrefresh(welcome);
            wrefresh(message);
            wrefresh(monitor);
            
            monitor=newwin(Size+8,80,2,81);
        }
        
        if(bufferInstr[0]==Monitor){
            wprintw(message,"\n   Simulation started...");
            MessageBorder(message);
            wrefresh(message);
            int *threadNumber;
            int *taskCount;
            double *realTimes;
            double *CPUTimes;
            threadNumber=new int[Size];
            taskCount=new int[Size];
            realTimes=new double[Size];
            CPUTimes=new double[Size];
            socketreceive(threadNumber, Size, newsockfd);
            int elcount=0;
            
            for(;;){
                elcount=0;
                socketreceive(elcount, newsockfd);
                socketreceive(taskCount, Size, newsockfd);
                socketreceive(realTimes, Size, newsockfd);
                socketreceive(CPUTimes, Size, newsockfd);
                
                totalTaskCount=0;
                for(int i=0;i<Size;i++){
                    totalTaskCount+=taskCount[i];
                }
                double el=(double)(elcount);
                
                wclear(monitor);
                PrintTableHeader(monitor);
                for(int i=0;i<Size;i++){
                    if(i==Master || i==Finalizer)continue;
                    PrintTableLine(monitor,i,threadNumber[i]/TaskMultiplier, taskCount[i], realTimes[i], CPUTimes[i], taskCount[i]*threadNumber[i]);
                }
                PrintTableFooter(monitor);
                if(elcount>=0) PrintProcess(monitor,(double)el,(double)Nel,totalTaskCount);
                else PrintProcess(monitor,(double)Nel,(double)Nel,totalTaskCount);
                MonitorBorder(monitor,Size);
                wrefresh(monitor);
                if(elcount<0)break;
            }
            
            delete[] threadNumber;
            delete[] taskCount;
            delete[] realTimes;
            delete[] CPUTimes;
        }
        
        if(bufferInstr[0]==MeanBeg){
            wprintw(message,"\r   Simulation done.    ");
            wprintw(message,"\n   Starting mean calculation...");
            read(newsockfd,bufferInstr,1);
            wprintw(message,"\r   Mean calculation done.      ");
            MessageBorder(message);
            wrefresh(message);
        }
        
        if(bufferInstr[0]==DevBeg){
            wprintw(message,"\n   Starting standard deviation calculation...");
            read(newsockfd,bufferInstr,1);
            wprintw(message,"\r   Standard deviation calculation done.      ");
            MessageBorder(message);
            wrefresh(message);
            break;
        }
        
        if(bufferInstr[0]==HistBeg){
            wprintw(message,"\n   Starting histogram generation...");
            read(newsockfd,bufferInstr,1);
            wprintw(message,"\r   Histogram generation done.      ");
            MessageBorder(message);
            wrefresh(message);
        }
        
        if(bufferInstr[0]==File){
            wprintw(message,"\n   Saving file...");
            read(newsockfd,bufferInstr,1);
            wprintw(message,"\r   Saving done.  ");
            MessageBorder(message);
            wrefresh(message);
            break;
        }
    }
    wgetch(welcome);
    endwin();
    
    close(newsockfd);
    
    return 0;
}