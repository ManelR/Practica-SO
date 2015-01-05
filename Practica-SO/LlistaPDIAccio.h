//
//  LlistaPDIAccio.h
//  GeneraLots
//
//  Created by Manel Roca & Guillermo Martínez-Ubierna on 2/11/14.
//  Copyright (c) 2014 Guille Martínez-Ubierna i Manel Roca. All rights reserved.
//

#ifndef LlistaPDIAccio_h
#define LlistaPDIAccio_h

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
}LlistaPDIAccio;

LlistaPDIAccio LlistaPDIAccio_crea();
void LlistaPDIAccio_insereix(LlistaPDIAccio * l, Accio a);
Accio LlistaPDIAccio_consulta(LlistaPDIAccio l);
void LlistaPDIAccio_esborra(LlistaPDIAccio * l);
void LlistaPDIAccio_avanca(LlistaPDIAccio * l);
void LlistaPDIAccio_vesInici(LlistaPDIAccio * l);
int LlistaPDIAccio_fi(LlistaPDIAccio l);
int LlistaPDIAccio_buida(LlistaPDIAccio l);
void LlistaPDIAccio_destrueix(LlistaPDIAccio * l);


#endif
