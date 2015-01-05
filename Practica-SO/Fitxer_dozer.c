//
//  Fitxer.c
//  Fase 1
//
//  Created by Manel Roca & Guillermo Martínez-Ubierna on 2/11/14.
//  Copyright (c) 2014 Guille Martínez-Ubierna i Manel Roca. All rights reserved.
//

#include "Fitxer_dozer.h"

/*********************************************************************************************************
 *
 *   @Nombre: carregaFitxerStock
 *   @Def: Función que sirve para cargar la información del fichero stock.dat
 *   @Arg:   In: file_stock -> File descriptor del fichero stock.dat
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void Fitxer_carregaFitxerStock(int file_stock, Operador* stOperador){
    char cAux;
    int nContador = 0;
    char aux_text[100];
    Accio a;
    
    read(file_stock, &cAux, 1);
    while(cAux != '\n'){
        stOperador->cNom[nContador] = cAux;
        read(file_stock, &cAux, 1);
        nContador++;
    }
    stOperador->cNom[nContador] = '\0';
    //Llegim el següent digit
    nContador = 0;
    read(file_stock, &aux_text[0], 1);
    while (aux_text[nContador] != '\n') {
        nContador++;
        read(file_stock, &aux_text[nContador], 1);
    }
    aux_text[nContador] = '\0';
    stOperador->fDinersTotals = atof(aux_text);
    //El \n després dels diners ja està llegit, el proxim és el primer caràcter del ticker
    
    nContador = 0;
    while(read(file_stock, &aux_text[nContador], 1) != 0) {
        while (aux_text[nContador] != '-') {
            nContador++;
            read(file_stock, &aux_text[nContador], 1);
        }
        aux_text[nContador] = '\0';
        strcpy(a.cTicker, aux_text);
        nContador = 0;
        read(file_stock, &aux_text[nContador], 1);
        while (aux_text[nContador] != '\n') {
            nContador++;
            read(file_stock, &aux_text[nContador], 1);
        }
        aux_text[nContador] = '\0';
        a.nAccions = atoi(aux_text);
        LlistaPDI_insereix(&stOperador->llistaAccions, a);
        nContador = 0;
    }
    close(file_stock);
}

/*********************************************************************************************************
 *
 *   @Nombre: carregaFitxerConfig
 *   @Def: Función que sirve para cargar la información del fichero config.dat
 *   @Arg:   In:    file_config -> File descriptor del fichero config.dat
 *                  stIP -> struct que contiene la información de conexión
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void Fitxer_carregaFitxerConfig(int file_config, IpInfo* stIP){
    char cAux;
    int nContador = 0;
    char aux_num[50];
    
    read(file_config, &cAux, 1);
    while (cAux != '\n') {
        stIP->sIP[nContador] = cAux;
        nContador++;
        read(file_config, &cAux, 1);
    }
    stIP->sIP[nContador] = '\0';
    nContador = 0;
    while (read(file_config, &aux_num[nContador], 1) != 0) {
        nContador++;
    }
    aux_num[nContador] = '\0';
    stIP->nPort = atoi(aux_num);
    close(file_config);
}
