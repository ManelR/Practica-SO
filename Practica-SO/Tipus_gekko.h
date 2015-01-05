//
//  Tipus.h
//  Gekko
//
//  Created by Manel Roca & Guillermo Martínez-Ubierna on 2/11/14.
//  Copyright (c) 2014 Guille Martínez-Ubierna i Manel Roca. All rights reserved.
//

#ifndef Gekko_Tipus_h
#define Gekko_Tipus_h

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

typedef struct{
    int nSegons;
    char sIP[30];
    int nPort;
}IpInfo;

typedef struct{
    char cTicker[10];
    float fPreu;
    long long llAccions;
}Accio;

typedef struct{
    char sOperador[14];
    int nNumAccions;
    int nSocket;
}Venta;

typedef struct{
    char Origen[14];
    char Tipus;
    char Data[100];
}Trama;

#endif
