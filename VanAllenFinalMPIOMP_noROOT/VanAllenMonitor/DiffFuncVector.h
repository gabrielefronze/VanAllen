//
//  DiffFuncVector.h
//  VanAllen
//
//  Created by Gabriele Gaetano Fronzé on 05/03/15.
//  Copyright (c) 2015 Gabriele Gaetano Fronzé. All rights reserved.
//

#ifndef VanAllen_DiffFuncVector_h
#define VanAllen_DiffFuncVector_h

#ifndef define
#include "Params.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <iostream>
#include "pcg_random.hpp"
#include <random>
#endif

const int RNGCallsPerExecution=2+N;

double norm(const double vett[3]){
    return(sqrt(vett[0]*vett[0]+vett[1]*vett[1]+vett[2]*vett[2]));
}

void DiffusionFunctionVector(std::vector<double> &resarr,int elindex,int seed){
    
    pcg64 RNG(seed,elindex);
    //RNG.discard(elindex*RNGCallsPerExecution);
    //pcg64 RNG(seed,elindex);
    std::uniform_real_distribution<double> distribution(0., 1.);
    
    double x0[3] = {0., 0., 0.};	/*Posizione iniziale degli elettroni test*/
    double v0[3] = {0., 0., 0.};	/*Velocit‡ iniziale degli elettroni test*/
    
    ///*Vettori d'onda (Ipotesi: le onde whistler si propagano lungo la direzione z, cioË quella del campo magnetico terrestre)*/
    double kz[N];
    /*Campi elettrici e magnetici delle onde whistler*/
    double dBx[N];
    double dBy[N];
    double dEx[N];
    double dEy[N];
    /*Fase iniziale delle onde whistler*/
    double phi0[N];
    /*Fase delle onde whistler*/
    double phi[N];
    /*Frequenza angolare delle onde whistler*/
    double w[N];
    w[0] = wmin;
    int j;
    for(j = 1; j < N; j++) {
        w[j] = w[j-1]+((wmax-wmin)/N);
    }
    
    double S, D, alpha, beta, rand_angle, f;
    double Bwx, Bwy, Bwz, Ewx, Ewy, Ewz;
    /*Campi elettrico e magnetico totali: campi terrestri + campi delle onde*/
    double Btot[3], Etot[3];
    /*Vettori utilizzati nel Boris mover*/
    double v_minus[3], v_plus[3], v_p[3], v_bef[3], v_aft[3], x_bef[3], x_aft[3];
    
    /*Campo d'onde*/
    for(int i = 0; i < N; i++) {
        
        /*Relazione di dispersione delle onde whistler*/
        //S = 1+(pow(wpi,2)/(pow(wci,2)-pow(w[ii],2)))+(pow(wpe,2)/(pow(wce,2)-pow(w[ii],2)));
        S = 1+(wpi*wpi/(wci*wci-w[i]*w[i]))+(wpe*wpe/(wce*wce-w[i]*w[i]));
        //D = ((wci*pow(wpi,2))/(pow(wci,2)-pow(w[i],2))) - ((wce*pow(wpe,2))/(pow(wce,2)-pow(w[i],2)));
        D = ((wci*wpi*wpi)/(wci*wci-w[i]*w[i])) - ((wce*wpe*wpe)/(wce*wce-w[i]*w[i]));
        kz[i] = -sqrt((w[i]/(c*c))*(w[i]*(S)-(D)));
        
        /*Componenti dei campi: soluzioni delle equazioni di Maxwell*/
        alpha = pow((w[i]/c),2)*S;
        beta = (w[i]/(c*c))*D;
        
        dBy[i] = 1e-12;
        dEx[i] = (w[i]/kz[i])*dBy[i];
        //dEy[i] = (((alpha-pow(kz[i],2))/beta))*dEx[i];
        dEy[i] = (((alpha-kz[i]*kz[i])/beta))*dEx[i];
        dBx[i] = (kz[i]/w[i])*dEy[i];
    }
    
    /*Calcolo della velocit‡ inziale dell'elettrone partendo da un angolo casuale*/
    rand_angle = 2*pi*distribution(RNG);
    
    x0[2] = (2*pi*distribution(RNG))/fabs(kz[(int)(N/2)]);
    v0[0] = mod_v0*sin(pitch_angle0)*cos(rand_angle);
    v0[1] = mod_v0*sin(pitch_angle0)*sin(rand_angle);
    v0[2] = mod_v0*cos(pitch_angle0);
    
    /*Fase iniziale: casuale*/
    int iv;
    for(iv = 0; iv < N; iv++) {
        phi0[iv] = 2*pi*distribution(RNG);
    }
    
    /*Calcolo dei campi delle onde: somma di tutti i contributi delle onde piane*/
    Bwx = 0.;
    Bwy = 0.;
    Ewx = 0.;
    Ewy = 0.;
    Bwz = 0.;
    Ewz = 0.;
    
    int vi;
    for(vi = 0; vi < N; vi++) {
        phi[vi] = phi0[vi];
        Bwx = Bwx + dBx[vi]*cos(phi[vi]);
        Bwy = Bwy + (-1)*dBy[vi]*sin(phi[vi]);
        Ewx = Ewx + (-1)*dEx[vi]*sin(phi[vi]);
        Ewy = Ewy + (-1)*dEy[vi]*cos(phi[vi]);
    }
    
    /*Calcolo dei campi totali*/
    Btot[0] = B[0]+Bwx;
    Btot[1] = B[1]+Bwy;
    Btot[2] = B[2]+Bwz;
    Etot[0] = E[0]+Ewx;
    Etot[1] = E[1]+Ewy;
    Etot[2] = E[2]+Ewz;
    
    /*Boris mover: mezzo ste indietro*/
    
    /*Step 1*/
    v_minus[0] = v0[0] + (Qe/m)*(-dt/4)*Etot[0];
    v_minus[1] = v0[1] + (Qe/m)*(-dt/4)*Etot[1];
    v_minus[2] = v0[2] + (Qe/m)*(-dt/4)*Etot[2];
    /*Step 2*/
    if(norm(Btot) == 0) f = 0;
    else f = tan((Qe/m)*(-dt/4)*norm(Btot))/norm(Btot);
    v_p[0] = v_minus[0] + f*(v_minus[1]*Btot[2]-v_minus[2]*Btot[1]);
    v_p[1] = v_minus[1] + f*(v_minus[2]*Btot[0]-v_minus[0]*Btot[2]);
    v_p[2] = v_minus[2] + f*(v_minus[0]*Btot[1]-v_minus[1]*Btot[0]);
    v_plus[0] = v_minus[0] + ((2*f)/(1+f*f*pow(norm(Btot),2)))*(v_p[1]*Btot[2]-v_p[2]*Btot[1]);
    v_plus[1] = v_minus[1] + ((2*f)/(1+f*f*pow(norm(Btot),2)))*(v_p[2]*Btot[0]-v_p[0]*Btot[2]);
    v_plus[2] = v_minus[2] + ((2*f)/(1+f*f*pow(norm(Btot),2)))*(v_p[0]*Btot[1]-v_p[1]*Btot[0]);
    /*Step 3*/
    v_aft[0] = v_plus[0] + ((Qe/m)*(-dt/4))*Etot[0];
    v_aft[1] = v_plus[1] + ((Qe/m)*(-dt/4))*Etot[1];
    v_aft[2] = v_plus[2] + ((Qe/m)*(-dt/4))*Etot[2];
    
    /*Setup per il prossimo step*/
    v_bef[0] = v_aft[0];
    v_bef[1] = v_aft[1];
    v_bef[2] = v_aft[2];
    x_bef[0] = x0[0];
    x_bef[1] = x0[1];
    x_bef[2] = x0[2];
    
    /*Ciclo sugli step temporali*/
    int st;
    for(st = 0; st < steps; st++) {
        
        /*Aggiornamenti dei campi*/
        
        /*Calcolo dei campi delle onde: somma di tutti i contributi delle onde piane*/
        Bwx = 0.;
        Bwy = 0.;
        Ewx = 0.;
        Ewy = 0.;
        Bwz = 0.;
        Ewz = 0.;
        int vii;
        for(vii = 0; vii < N; vii++){
            phi[vii] = kz[vii]*x_bef[2]-w[vii]*(st*dt)+phi0[vii];
            Bwx = Bwx + dBx[vii]*cos(phi[vii]);
            Bwy = Bwy + (-1)*dBy[vii]*sin(phi[vii]);
            Ewx = Ewx + (-1)*dEx[vii]*sin(phi[vii]);
            Ewy = Ewy + (-1)*dEy[vii]*cos(phi[vii]);
        }
        
        /*Calcolo dei campi totali*/
        Btot[0] = B[0]+Bwx;
        Btot[1] = B[1]+Bwy;
        Btot[2] = B[2]+Bwz;
        Etot[0] = E[0]+Ewx;
        Etot[1] = E[1]+Ewy;
        Etot[2] = E[2]+Ewz;
        
        /*Boris mover: step*/
        
        /*Step 1*/
        v_minus[0] = v_bef[0] + (Qe/m)*(dt/2)*Etot[0];
        v_minus[1] = v_bef[1] + (Qe/m)*(dt/2)*Etot[1];
        v_minus[2] = v_bef[2] + (Qe/m)*(dt/2)*Etot[2];
        
        /*Step 2*/
        if(norm(Btot) == 0) f = 0;
        else f = tan((Qe/m)*(dt/2)*norm(Btot))/norm(Btot);
        
        v_p[0] = v_minus[0] + f*(v_minus[1]*Btot[2]-v_minus[2]*Btot[1]);
        v_p[1] = v_minus[1] + f*(v_minus[2]*Btot[0]-v_minus[0]*Btot[2]);
        v_p[2] = v_minus[2] + f*(v_minus[0]*Btot[1]-v_minus[1]*Btot[0]);
        
        v_plus[0] = v_minus[0] + ((2*f)/(1+f*f*pow(norm(Btot),2)))*(v_p[1]*Btot[2]-v_p[2]*Btot[1]);
        v_plus[1] = v_minus[1] + ((2*f)/(1+f*f*pow(norm(Btot),2)))*(v_p[2]*Btot[0]-v_p[0]*Btot[2]);
        v_plus[2] = v_minus[2] + ((2*f)/(1+f*f*pow(norm(Btot),2)))*(v_p[0]*Btot[1]-v_p[1]*Btot[0]);
        
        /*Step 3*/
        v_aft[0] = v_plus[0] + ((Qe/m)*(dt/2))*Etot[0];
        v_aft[1] = v_plus[1] + ((Qe/m)*(dt/2))*Etot[1];
        v_aft[2] = v_plus[2] + ((Qe/m)*(dt/2))*Etot[2];
        
        /*Nuova posizione della particella test*/
        x_aft[0] = x_bef[0] + dt*v_aft[0];
        x_aft[1] = x_bef[1] + dt*v_aft[1];
        x_aft[2] = x_bef[2] + dt*v_aft[2];
        
        /*Setup per il prossimo step*/
        v_bef[0] = v_aft[0];
        v_bef[1] = v_aft[1];
        v_bef[2] = v_aft[2];
        x_bef[0] = x_aft[0];
        x_bef[1] = x_aft[1];
        x_bef[2] = x_aft[2];    
        
        /*Calcolo del pitch angle allo step corrente*/
        resarr.push_back(acos(v_aft[2]/norm(v_aft))*(180/pi));
        //resarr.push_back(10);
    }
}

#endif
