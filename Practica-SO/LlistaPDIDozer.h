//
//  LlistaPDIDozer.h
//  GeneraLots
//
//  Created by Manel Roca & Guillermo Martínez-Ubierna on 14/01/15.
//  Copyright (c) 2014 Guille Martínez-Ubierna i Manel Roca. All rights reserved.
//

#ifndef LlistaPDIDozer_h
#define LlistaPDIDozer_h

#include "Tipus_gekko.h"

typedef struct{
    char sOperador[14];
    int nSocket;
    Trama trama;
}Dozer;

typedef struct n4{
    Dozer d;
    struct n4 *pstSeg;
}Noded;

typedef struct{
    Noded * pstCap;
    Noded * pstAnt;
}LlistaPDIDozer;

LlistaPDIDozer LlistaPDIDozer_crea();
void LlistaPDIDozer_insereix(LlistaPDIDozer * l, Dozer a);
Dozer LlistaPDIDozer_consulta(LlistaPDIDozer l);
void LlistaPDIDozer_esborra(LlistaPDIDozer * l);
void LlistaPDIDozer_avanca(LlistaPDIDozer * l);
void LlistaPDIDozer_vesInici(LlistaPDIDozer * l);
int LlistaPDIDozer_fi(LlistaPDIDozer l);
int LlistaPDIDozer_buida(LlistaPDIDozer l);
void LlistaPDIDozer_destrueix(LlistaPDIDozer * l);


#endif
