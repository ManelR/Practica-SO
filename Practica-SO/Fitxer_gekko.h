//
//  Fitxer.h
//  Gekko
//
//  Created by Manel Roca & Guillermo Martínez-Ubierna on 2/11/14.
//  Copyright (c) 2014 Guille Martínez-Ubierna i Manel Roca. All rights reserved.
//

#ifndef __Gekko__Fitxer__
#define __Gekko__Fitxer__

#include <stdio.h>
#include "Tipus_gekko.h"

void Fitxer_carregaFitxerConfig(int file_config, IpInfo* stIP);
void Fitxer_carregaFitxerIbex(int file_ibex, Accio ibex[35], InfoVentes ventes[35]);
void Fitxer_actualitzaXML(AccioXML ibex[35]);
#endif
