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
 *   @Nombre: actualitzaXML
 *   @Def: Función que sirve para actualizar la información del fichero stocks.xml
 *   @Arg:   In:    ibex[35] -> array que contiene la informacion para inserir en el fichero xml
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void Fitxer_actualitzaXML(AccioXML ibex[35]){
    int file_xml, i = 0, nActualitzaFitxer = 0;
    char cAux;
    char *sAux, sText[100];
    
    //Creacio del fitxer xml
    file_xml = open("stocks.xml", O_RDWR | O_CREAT, 0666);
    if (file_xml < 0) {
        write(1, "Error al crear el fichero xml.\n", strlen("Error al crear el fichero xml.\n"));
    }else{
        //Si esta buit escrivim <stock></stock>
        if (read(file_xml, &cAux, 1) == 0) {
            write(file_xml, "<stock>\n</stock>", sizeof("<stock>\n</stock>"));
        }else{
            //llegeixo el fitxer i el guardo en un string
            sAux = (char*)malloc(sizeof(char));
            sAux[i] = cAux;
            i++;
            while (read(file_xml, &cAux, 1) != 0) {
                sAux = (char*)realloc(sAux, sizeof(char) * (i+1));
                sAux[i] = cAux;
                i++;
            }
            sAux = (char*)realloc(sAux, sizeof(char) * (i+1));
            sAux[i] = '\0';
            sAux[strlen(sAux) - 8] = '\0';
            //Concatenar al string la informacio nova
            sAux = (char*) realloc(sAux, (strlen(sAux) + strlen("\t<period_summary>\n")));
            strcat(sAux, "\t<period_summary>\n");
            for (i = 0; i < 35; i++) {
                strcpy(sText, ibex[i].cTicker);
                sAux = (char*)realloc(sAux, (strlen(sAux) + strlen("\t\t<ticker>\n\t\t\t<name></name>\n") + strlen(sText)));
                strcat(sAux, "\t\t<ticker>\n");
                strcat(sAux, "\t\t\t<name>");
                strcat(sAux, sText);
                strcat(sAux, "</name>\n");
                
                sprintf(sText, "%.2f", ibex[i].fPreuInicial);
                sAux = (char*) realloc(sAux, (strlen(sAux) + strlen("\t\t\t<open></open>\n") + strlen(sText)));
                strcat(sAux, "\t\t\t<open>");
                strcat(sAux, sText);
                strcat(sAux, "</open>\n");
                
                sprintf(sText, "%.2f", ibex[i].fPreuFinal);
                sAux = (char*) realloc(sAux, (strlen(sAux) + strlen("\t\t\t<close></close>\n") + strlen(sText)));
                strcat(sAux, "\t\t\t<close>");
                strcat(sAux, sText);
                strcat(sAux, "</close>\n");
                
                sprintf(sText, "%.2f", ibex[i].fPreuMaxim);
                sAux = (char*)realloc(sAux, (strlen(sAux) + strlen("\t\t\t<high></high>\n") + strlen(sText)));
                strcat(sAux, "\t\t\t<high>");
                strcat(sAux, sText);
                strcat(sAux, "</high>\n");
                
                sprintf(sText, "%.2f", ibex[i].fPreuMinim);
                sAux = (char*)realloc(sAux, (strlen(sAux) + strlen("\t\t\t<low></low>\n\t\t</ticker>\n\t</period_summary>\n</stock>") + strlen(sText)));
                strcat(sAux, "\t\t\t<low>");
                strcat(sAux, sText);
                strcat(sAux, "</low>\n");
                strcat(sAux, "\t\t</ticker>\n");
            }
            strcat(sAux, "\t</period_summary>\n</stock>");
            nActualitzaFitxer = 1;
        }
        close(file_xml);
    }
    if(nActualitzaFitxer == 1){
        file_xml = open("stocks.xml", O_TRUNC | O_WRONLY, 0666);
        if (file_xml < 0) {
            write(1, "Error al crear el fichero xml.\n", strlen("Error al crear el fichero xml.\n"));
        }else{
            i = 0;
            while (sAux[i] != '\0') {
                write(file_xml, &sAux[i], 1);
                i++;
            }
            close(file_xml);
        }
        free(sAux);
    }
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
    read(file_config, &cAux, 1);
    while (cAux != '\n') {
        sText[nContador] = cAux;
        read(file_config, &cAux, 1);
        nContador++;
    }
    sText[nContador] = '\0';
    stIP->nPort = atoi(sText);
    nContador = 0;
    read(file_config, &cAux, 1);
    while (cAux != '\n') {
        sText[nContador] = cAux;
        read(file_config, &cAux, 1);
        nContador++;
    }
    sText[nContador] = '\0';
    stIP->nPeticio = atoi(sText);
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
