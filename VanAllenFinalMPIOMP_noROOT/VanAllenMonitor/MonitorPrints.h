//
//  MonitorPrints.h
//  VanAllenMonitor2
//
//  Created by Gabriele Gaetano Fronzé on 03/04/15.
//  Copyright (c) 2015 Gabriele Gaetano Fronzé. All rights reserved.
//

#ifndef VanAllenMonitor2_MonitorPrints_h
#define VanAllenMonitor2_MonitorPrints_h

#include <ncurses.h>

void LoadBorders(){
    addch(ACS_ULCORNER);
    addch(ACS_LLCORNER);
    addch(ACS_URCORNER);
    addch(ACS_LRCORNER);
    addch(ACS_LTEE);
    addch(ACS_RTEE);
    addch(ACS_BTEE);
    addch(ACS_TTEE);
    addch(ACS_HLINE);
    addch(ACS_VLINE);
    addch(ACS_PLUS);
    addch(ACS_CKBOARD);
}

void WelcomeBorder(WINDOW *win){
    wborder(win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
}

void MessageBorder(WINDOW *win){
    wborder(win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_LTEE, ACS_RTEE, ACS_LLCORNER, ACS_LRCORNER);
}

void MonitorBorder(WINDOW *win,int Size){
    int MonitorSize=Size+8;
    if(MonitorSize<39){
        wborder(win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_TTEE, ACS_URCORNER, ACS_LTEE, ACS_LRCORNER);
        mvwaddch(win,29,0,ACS_RTEE);
    }else if(MonitorSize==39){
        wborder(win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_TTEE, ACS_URCORNER, ACS_BTEE, ACS_LRCORNER);
        mvwaddch(win,29,0,ACS_RTEE);
    }else{
        wborder(win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_TTEE, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
        mvwaddch(win,29,0,ACS_RTEE);
        mvwaddch(win,38,0,ACS_RTEE);
    }
}

int PrintWelcome(WINDOW* win,int Nel,int Size,int Master,int Finalizer){
    if(Master!=Finalizer) return wprintw(win, "   \n"
                                              "                                                                             \n"
                                              "                             WELCOME TO VANALLEN MPI+OMP                     \n"
                                              "                   a software developed by G.G. Fronzé and M. Mina           \n"
                                              "                                                                             \n"
                                              "   \n"
                                              "                                                                             \n"
                                              "    This program will simulate the electron drift mote crossing Earth        \n"
                                              "    magnetic field of %d electrons.                                          \n"
                                              "                                                                             \n"
                                              "   \n"
                                              "    The simulation is running on %3d machines or workers, each with possibly \n"
                                              "    a different kind of CPU. This program tries to use the full power of the \n"
                                              "    machines connected, assigning jobs on demand customized on the local     \n"
                                              "    computational power.                                                     \n"
                                              "   \n"
                                              "                                                                             \n"
                                              "    For this run Master and Finalizer are on ranks %3d and %3d.              \n"
                                              "                                                                             \n"
                                              "    This run can be exactly reproduced giving it the same RNG seed. We use a \n"
                                              "    PCG64 RNG capable of 2^127 different sequences with 2^128 period.        \n"
                                              "                                                                             \n"
                                              "    Output file is produced using the CERN ROOT Framework.                   \n"
                                              "    To open it please use a TBrowser instance.                               \n"
                                              "                                                                             \n"
                                              "       Have sincerely fun                                                    \n"
                                              "                                                                             \n"
                                              "   \n"
                                              "                                                                             \n",Nel,Size,Master,Finalizer);
    else return wprintw(win,"   \n"
                            "                                                                             \n"
                            "                                WELCOME TO VANALLEN                          \n"
                            "                   a software developed by G.G. Fronzé and M. Mina           \n"
                            "                                                                             \n"
                            "   \n"
                            "                                                                             \n"
                            "    This program will simulate the electron drift mote crossing Earth        \n"
                            "    magnetic field of %d electrons.                                          \n"
                            "                                                                             \n"
                            "   \n"
                            "    The simulation is running on %3d machines or workers, each with possibly \n"
                            "    a different kind of CPU. This program tries to use the full power of the \n"
                            "    machines connected, assigning jobs on demand customized on the local     \n"
                            "    computational power.                                                     \n"
                            "   \n"
                            "                                                                             \n"
                            "    For this run Master and Finalizer are on the same rank %3d.              \n"
                            "                                                                             \n"
                            "    This run can be exactly reproduced giving it the same RNG seed. We use a \n"
                            "    PCG64 RNG capable of 2^127 different sequences with 2^128 period.        \n"
                            "                                                                             \n"
                            "    Output file is produced using the CERN ROOT Framework.                   \n"
                            "    To open it please use a TBrowser instance.                               \n"
                            "                                                                             \n"
                            "       Have sincerely fun                                                    \n"
                            "                                                                             \n"
                            "   \n"
                            "                                                                             \n",Nel,Size,Master);
}

int PrintTableHeader(WINDOW* win){
    return wprintw(win,
                   "\n\n"
                   "      Worker  |  Threads  |  Tasks  |   Real time   |  CPU time  |  Electrons  \n");
}

int PrintTableLine(WINDOW* win,int i,int threads,int taskCount,double realTimes,double CPUTimes,int elcount){
    return wprintw(win,
                   "      %6d  |  %7d  |  %5d  |   %9.2f   |  %8.2f  |  %9d  \n",i,threads,taskCount,realTimes,CPUTimes,elcount);
}

int PrintTableFooter(WINDOW* win){
    return wprintw(win,
                   "                                                                             \n"
                   "   \n"
                   "                                                                             \n");
}

int PrintProcess(WINDOW* win,double el,double Nel,int totalTaskCount){
    std::string s("    Progress: [");
    for(int i=0;i<(int)(el/Nel*36.);i++){
        s+='|';
    }
    if(el<Nel-1){
        s+='>';
    }
    for(int i=(int)(el/Nel*36.);i<36;i++){
        s+='-';
    }
    wprintw(win,s.data());
    if(el/Nel*100.<=100.)
        return wprintw(win,"] %4.1f%% with %5d tasks \n\n",el/Nel*100.,totalTaskCount);
    else
        return wprintw(win,"] %4.1f%% with %5d tasks \n\n",100.,totalTaskCount);
}

#endif