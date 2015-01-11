//
//  Shell.h
//  Fase 1
//
//  Created by Manel Roca & Guillermo Martínez-Ubierna on 2/11/14.
//  Copyright (c) 2014 Guille Martínez-Ubierna i Manel Roca. All rights reserved.
//

#ifndef __Fase_1__Shell__
#define __Fase_1__Shell__

#include <stdio.h>
#include "Tipus_dozer.h"
#include <semaphore.h>

void Shell_analitzaComanda(int * sortir, Operador* stOperador, int sockGekko, sem_t * mutex);
void showIbex(Trama trama);
void buy(Trama trama);
void sell(Trama trama);
void esborra(Trama trama);
void vengut(Trama trama);

#endif
