//
//  LlistaPDIVenta.h
//  GeneraLots
//
//  Created by Manel Roca & Guillermo Martínez-Ubierna on 2/11/14.
//  Copyright (c) 2014 Guille Martínez-Ubierna i Manel Roca. All rights reserved.
//

#ifndef LlistaPDIVenta_h
#define LlistaPDIVenta_h

#include "Tipus_gekko.h"

typedef struct n2{
    Venta v;
    struct n2 *pstSeg;
}Nodec;

typedef struct{
    Nodec * pstCap;
    Nodec * pstAnt;
}LlistaPDIVenta;

LlistaPDIVenta LlistaPDIVenta_crea();
void LlistaPDIVenta_insereix(LlistaPDIVenta * l, Venta a);
Venta LlistaPDIVenta_consulta(LlistaPDIVenta l);
void LlistaPDIVenta_esborra(LlistaPDIVenta * l);
void LlistaPDIVenta_avanca(LlistaPDIVenta * l);
void LlistaPDIVenta_vesInici(LlistaPDIVenta * l);
int LlistaPDIVenta_fi(LlistaPDIVenta l);
int LlistaPDIVenta_buida(LlistaPDIVenta l);
void LlistaPDIVenta_destrueix(LlistaPDIVenta * l);


#endif
