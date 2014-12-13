//
//  LlistaPDI.h
//  GeneraLots
//
//  Created by Manel Roca & Guillermo Martínez-Ubierna on 2/11/14.
//  Copyright (c) 2014 Guille Martínez-Ubierna i Manel Roca. All rights reserved.
//

#ifndef LlistaPDI_h
#define LlistaPDI_h

typedef struct{
    char cTicker[6];
    int nAccions;
}Accio;

typedef struct n2{
    Accio a;
    struct n2 *pstSeg;
}Nodec;

typedef struct{
    Nodec * pstCap;
    Nodec * pstAnt;
}LlistaPDI;

LlistaPDI LlistaPDI_crea();
void LlistaPDI_insereixOrdenat(LlistaPDI * l, Accio a);
Accio LlistaPDI_consulta(LlistaPDI l);
void LlistaPDI_esborra(LlistaPDI * l);
void LlistaPDI_avanca(LlistaPDI * l);
void LlistaPDI_vesInici(LlistaPDI * l);
int LlistaPDI_fi(LlistaPDI l);
int LlistaPDI_buida(LlistaPDI l);
void LlistaPDI_destrueix(LlistaPDI * l);


#endif
