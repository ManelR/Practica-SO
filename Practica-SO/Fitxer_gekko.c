//
//  Fitxer.c
//  Gekko
//
//  Created by Manel Roca & Guillermo Martínez-Ubierna on 2/11/14.
//  Copyright (c) 2014 Guille Martínez-Ubierna i Manel Roca. All rights reserved.
//

#include "Fitxer_gekko.h"

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
    char sText[100];
    int nContador = 0;
    
    //Lectura dels segons
    read(file_config, &cAux, 1);
    while (cAux != '\n') {
        sText[nContador] = cAux;
        read(file_config, &cAux, 1);
        nContador++;
    }
    sText[nContador] = '\0';
    stIP->nSegons = atoi(sText);
    nContador = 0;
    //Lectura de la ip
    read(file_config, &cAux, 1);
    while (cAux != '\n') {
        sText[nContador] = cAux;
        read(file_config, &cAux, 1);
        nContador++;
    }
    sText[nContador] = '\0';
    strcpy(stIP->sIP, sText);
    //Lectura del port
    nContador = 0;
    while (read(file_config, &cAux, 1) != 0) {
        sText[nContador] = cAux;
        nContador++;
    }
    sText[nContador] = '\0';
    stIP->nPort = atoi(sText);
    close(file_config);
}

/*********************************************************************************************************
 *
 *   @Nombre: carregaFitxerIbex
 *   @Def: Función que sirve para cargar la información del fichero ibex.dat
 *   @Arg:   In:    file_ibex -> File descriptor del fichero ibex.dat
 *                  ibex -> array struct que contiene la información de las acciones
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void Fitxer_carregaFitxerIbex(int file_ibex, Accio ibex[35], InfoVentes ventes[35]){
    int nContadorIbex = 0, nContador = 0;
    char cAux;
    char sText[100];
    
    while (read(file_ibex, &cAux, 1) != 0) {
        //Lectura del nom
        nContador = 0;
        while (cAux != '\t') {
            sText[nContador] = cAux;
            nContador++;
            read(file_ibex, &cAux, 1);
        }
        sText[nContador] = '\0';
        strcpy(ibex[nContadorIbex].cTicker, sText);
        strcpy(ventes[nContadorIbex].sNom, sText);
        //Lectura del preu
        nContador = 0;
        read(file_ibex, &cAux, 1);
        while (cAux != '\t') {
            sText[nContador] = cAux;
            nContador++;
            read(file_ibex, &cAux, 1);
        }
        sText[nContador] = '\0';
        ibex[nContadorIbex].fPreu = atof(sText);
        //Lectura del numero d'accions
        nContador = 0;
        read(file_ibex, &cAux, 1);
        while (cAux != '\n') {
            sText[nContador] = cAux;
            nContador++;
            read(file_ibex, &cAux, 1);
        }
        sText[nContador] = '\0';
        ibex[nContadorIbex].llAccions = atoll(sText);
        nContadorIbex++;
    }
    close(file_ibex);
}
