//
//  Tipus.h
//  LlistaPDI
//
//  Created by Manel Roca & Guillermo Martínez-Ubierna on 2/11/14.
//  Copyright (c) 2014 Guille Martínez-Ubierna i Manel Roca. All rights reserved.
//

#ifndef Dozer_Tipus_h
#define Dozer_Tipus_h

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include "LlistaPDIAccio.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

typedef struct{
    char Origen[14];
    char Tipus;
    char Data[100];
}Trama;

typedef struct{
    LlistaPDIAccio llistaAccions;
    float fDinersTotals;
    char cNom[100];
}Operador;

typedef struct{
    char sIP[30];
    int nPort;
}IpInfo;

#endif
