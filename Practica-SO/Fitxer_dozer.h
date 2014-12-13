//
//  Fitxer.h
//  Fase 1
//
//  Created by Manel Roca & Guillermo Martínez-Ubierna on 2/11/14.
//  Copyright (c) 2014 Guille Martínez-Ubierna i Manel Roca. All rights reserved.
//

#ifndef __Fase_1__Fitxer__
#define __Fase_1__Fitxer__

#include "Tipus_dozer.h"
#include "LlistaPDI.h"

void Fitxer_carregaFitxerStock(int file_stock, Operador* stOperador);
void Fitxer_carregaFitxerConfig(int file_config, IpInfo* stIP);

#endif 
