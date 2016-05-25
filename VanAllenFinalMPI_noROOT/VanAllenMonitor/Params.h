//
//  Params.h
//  VanAllen
//
//  Created by Gabriele Gaetano Fronzé on 05/03/15.
//  Copyright (c) 2015 Gabriele Gaetano Fronzé. All rights reserved.
//

#ifndef VanAllen_Params_h
#define VanAllen_Params_h

#ifndef define
#include "Functions.h"
#include <math.h>
#endif

/*Costanti fisiche*/
const double Qe = -1.60217657e-19;	/*Carica degli elettroni*/
const double Qi = 1.60217657e-19;	/*Carica degli ioni positivi (protoni)*/
const double m = 9.10938291e-31;	/*Massa degli elettroni*/
const double M = 1836*m;	/*Massa degli ioni positivi*/
const double eps0 = 8.854187817e-12;	/*Costante dielettrica del vuoto*/
const double c = 3e8;	/*Velocit‡ della luce*/
const double pi = acos(-1.);	/*Pi greco*/

/*Densit‡ del plasma di background*/
const double ni = 1e7;	/*Densit‡ degli ioni*/
const double ne = 1e7;	/*Densita degli elettroni*/

/*Campi elettrici e magnetici della Terra*/
const double B[3] = {0, 0, 1.4e-7};	/*Campo magnetico terrestre nella magnetosfera*/
const double E[3] = {0, 0, 0};	/*Campo elettrico terrestre*/

/*Parametri delle particelle test*/
const double En = 9.313e3*fabs(Qe);	/*Energia degli elettroni test*/
const double pitch_angle0 = 40*(pi/180);	/*Pitch angle (angolo tra i vettori campo magnetico e velocit‡) iniziale degli elettroni test*/
const double mod_v0 = sqrt(2*En/m);	/*Calcolo della velocit‡ iniziale a partire dall'energia*/

/*Parametri del plasma*/
const double wci = fabs(Qi)*B[2]/M;	/*Frequenza di ciclotrone degli ioni*/
const double wce = fabs(Qe)*B[2]/m;	/*Frequenza di ciclotrone degli elettroni*/
const double wpi = sqrt((ni*pow(Qi,2))/(eps0*M));	/*Frequenza di plasma degli ioni*/
const double wpe = sqrt((ne*pow(Qe,2))/(eps0*m));	/*Frequenza di plasma degli elettroni*/

/*Parametri del Boris mover*/
const double dt = 1e-5;	/*Step temporale della simulazione*/
const double tmax = (200*2*pi)/wce;	/*Tempo massimo della simulazione*/
const int steps = (int)floor(tmax/dt);


/*Whistler: http://www.treccani.it/vocabolario/whistler/*/
const int N = 100;	/*Numero di contributi per lo sviluppo in onde piane*/
const double wmin = 0.2*wce;	/*Frequenza minima delle onde piane*/
const double wmax = 0.4*wce;	/*Frequenza massima delle onde piane*/

#endif
