//
//  Shell.c
//  Fase 1
//
//  Created by Manel Roca & Guillermo Martínez-Ubierna on 2/11/14.
//  Copyright (c) 2014 Guille Martínez-Ubierna i Manel Roca. All rights reserved.
//

#include "Shell.h"
#define SORIGEN 14
#define SDADES 100
/*********************************************************************************************************
 *
 *   @Nombre: analitzaComanda
 *   @Def: Función que sirve para crear la shell y analizar la operación introducida
 *   @Arg:   In:    sortir -> sirve para indicar al main cuando terminar el programa.
                    stOperador -> estructura que guarda la información del programa.
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void Shell_analitzaComanda(int * sortir, Operador* stOperador, int sockGekko, sem_t * semafor){
    char sText[100], sAux[100];
    char *sComanda, *sAccio, *sNombreAccions, *sTicker;
    char cAux;
    int num = 0, trobat = 0;
    int i = 0, j = 0, mida = 0, s= 0;
    Accio a;
    Trama trama;
    
    //Mostar la shell
    bzero(sText, sizeof(sText));
    sprintf(sText, "%s>", stOperador->cNom);
    write(1, sText, sizeof(sText));
    
    //Llegir la comanda
    sComanda = (char*)malloc(sizeof(char));
    num = 0;
    read(0, &cAux, 1);
    while (cAux != '\n') {
        sComanda[num] = cAux;
        num++;
        sComanda = (char*)realloc(sComanda, sizeof(char) * (num+1));
        read(0, &cAux, 1);
    }
    sComanda[num] = '\0';
    
    //insensitive
    mida = (int)strlen(sComanda);
    for(i = 0; i < mida; i++){
        if (sComanda[i] >= 'A' && sComanda[i] <= 'Z') {
            sComanda[i] = sComanda[i] + 32;
        }
    }
    
    //Analitza Comanda
    if (!strcmp("exit", sComanda)) {
        write(1, "\nSayonara\n", sizeof("\nSayonara\n"));
        LlistaPDIAccio_destrueix(&stOperador->llistaAccions);
        *sortir = 1;
    }else if(!strcmp("show me the money", sComanda)){
        bzero(sText, sizeof(sText));
        sprintf(sText, "\n%.2f€\n\n", stOperador->fDinersTotals);
        write(1, sText, sizeof(sText));
    }else if(!strcmp("show stock", sComanda)){
        LlistaPDIAccio_vesInici(&stOperador->llistaAccions);
        write(1, "\n", sizeof("\n"));
        while (!LlistaPDIAccio_fi(stOperador->llistaAccions)) {
            a = LlistaPDIAccio_consulta(stOperador->llistaAccions);
            bzero(sText, sizeof(sText));
            sprintf(sText, "%s-%d\n",a.cTicker, a.nAccions);
            write(1, sText, sizeof(sText));
            LlistaPDIAccio_avanca(&stOperador->llistaAccions);
        }
        write(1, "\n", sizeof("\n"));
        
    }else if (!strcmp("show ibex", sComanda)){
        strcpy(trama.Origen, stOperador->cNom);
        for (i = strlen(trama.Origen); i < SORIGEN; i++) {
            trama.Origen[i] = '\0';
        }
        trama.Tipus = 'X';
        strcpy(trama.Data, "PETICIO IBEX35");
        for (i=14; i < SDADES; i++) {
            trama.Data[i] = '\0';
        }
        //Enviar
        write(sockGekko, &trama, sizeof(trama));
        //Lock del thread
        sem_wait(semafor);
    }else{
        i = 0;
        j = 0;
        sAccio = (char*)malloc(sizeof(char));
        while (sComanda[i] != ' ' && sComanda[i] != '\0') {
            sAccio[j] = sComanda[i];
            i++;
            j++;
            sAccio = (char*)realloc(sAccio, sizeof(char) * (j+1));
        }
        sAccio[j] = '\0';
        
        if(sComanda[i] == '\0'){
            write(1, "\nComanda incorrecta\n\n", sizeof("\nComanda incorrecta\n\n"));
        }else{
            i++;
            j = 0;
            sTicker = (char*)malloc(sizeof(char));
            while (sComanda[i] != ' ' && sComanda[i] != '\0') {
                sTicker[j] = sComanda[i];
                i++;
                j++;
                sTicker = (char*)realloc(sTicker, sizeof(char) * (j+1));
            }
            sTicker[j] = '\0';
            
            if(sComanda[i] == '\0'){
                write(1, "\nComanda incorrecta\n\n", sizeof("\nComanda incorrecta\n\n"));
            }else{
                i++;
                j = 0;
                sNombreAccions = (char*)malloc(sizeof(char));
                while (sComanda[i] != ' ' && sComanda[i] != '\0') {
                    sNombreAccions[j] = sComanda[i];
                    i++;
                    j++;
                    sNombreAccions = (char*)realloc(sNombreAccions, sizeof(char) * (j+1));
                }
                sNombreAccions[j] = '\0';
                
                if(sComanda[i] == '\0'){
                    if (!strcmp(sAccio, "buy")) {
                        //buy
                        //Comprovar que tingui 5 o menys lletres el ticker
                        if (strlen(sTicker) < 6) {
                            //Comprovar si han entrat nombre enter d'accions
                            i = 0;
                            while (sNombreAccions[i] >= '0' && sNombreAccions[i] <= '9') {
                                i++;
                            }
                            if (sNombreAccions[i] == '\0' && strlen(sNombreAccions) < 47) {
                                strcpy(trama.Origen, stOperador->cNom);
                                for (i = strlen(trama.Origen); i < SORIGEN; i++) {
                                    trama.Origen[i] = '\0';
                                }
                                trama.Tipus = 'B';
                                strcpy(sText, sTicker);
                                i = strlen(sText);
                                sText[i] = '-';
                                sText[i+1] = '\0';
                                strcat(sText, sNombreAccions);
                                i = strlen(sText);
                                sText[i] = '-';
                                sText[i+1] = '\0';
                                //Diners totals to string
                                sprintf(sAux,"%.2f",stOperador->fDinersTotals);
                                if (strlen(sAux) < 47) {
                                    strcat(sText, sAux);
                                    strcpy(trama.Data, sText);
                                    for (i = strlen(trama.Data); i < SDADES; i++) {
                                        trama.Data[i] = '\0';
                                    }
                                    //Enviar
                                    write(sockGekko, &trama, sizeof(trama));
                                    //Lock del thread
                                    sem_wait(semafor);
                                }else{
                                    write(1, "\nComanda incorrecta, tens masses diners per enviar la trama al Gekko.\n\n", sizeof("\nComanda incorrecta, tens masses diners per enviar la trama al Gekko.\n\n"));
                                }
                            }else if(sNombreAccions[i] != '\0'){
                                write(1, "\nComanda incorrecta, has d'entrar un nombre enter d'accions.\n\n", sizeof("\nComanda incorrecta, has d'entrar un nombre enter d'accions.\n\n"));
                            }else{
                                write(1, "\nComanda incorrecta, has d'entrar un nombre d'accions mes petit.\n\n", sizeof("\nComanda incorrecta, has d'entrar un nombre d'accions mes petit.\n\n"));
                            }
                        }else{
                            write(1, "\nComanda incorrecta, has d'entrar un nombre de ticker existent.\n\n", sizeof("\nComanda incorrecta, has d'entrar un nombre de ticker existent.\n\n"));
                        }
                    }else if (!strcmp(sAccio, "sell")){
                        //sell
                        //Comprovar que tingui 5 o menys lletres el ticker
                        if (strlen(sTicker) < 6) {
                            //Comprovar si han entrat nombre enter d'accions
                            i = 0;
                            while (sNombreAccions[i] >= '0' && sNombreAccions[i] <= '9') {
                                i++;
                            }
                            //comprovar que el nombre d'accions sigui suficientment petit per poder enviar-lo per la trama
                            if (sNombreAccions[i] == '\0' && strlen(sNombreAccions) < 94) {
                                //comprovar si tinc el tiker de laccio
                                LlistaPDIAccio_vesInici(&stOperador->llistaAccions);
                                while (!LlistaPDIAccio_fi(stOperador->llistaAccions) && !trobat) {
                                    a = LlistaPDIAccio_consulta(stOperador->llistaAccions);
                                    if(!strcasecmp(a.cTicker,sTicker)){
                                        //Enviar trama
                                        strcpy(trama.Origen, stOperador->cNom);
                                        for (i = strlen(trama.Origen); i < SORIGEN; i++) {
                                            trama.Origen[i] = '\0';
                                        }
                                        trama.Tipus = 'S';
                                        bzero(sText, sizeof(sText));
                                        strcpy(sText, sTicker);
                                        i = strlen(sText);
                                        sText[i] = '-';
                                        sText[i+1] = '\0';
                                        strcat(sText, sNombreAccions);
                                        strcpy(trama.Data, sText);
                                        for (i = strlen(sText); i < SDADES; i++) {
                                            trama.Data[i] = '\0';
                                        }
                                        printf("%s\n",trama.Data);
                                        write(sockGekko, &trama, sizeof(trama));
                                        //Lock del thread
                                        sem_wait(semafor);
                                        trobat = 1;
                                    }
                                    LlistaPDIAccio_avanca(&stOperador->llistaAccions);
                                }
                                if(!trobat) write(1, "\nComanda incorrecta, has d'entrar un nombre de ticker existent.\n\n", sizeof("\nComanda incorrecta, has d'entrar un nombre de ticker existent.\n\n"));
                            }else if(sNombreAccions[i] != '\0'){
                                write(1, "\nComanda incorrecta, has d'entrar un nombre enter d'accions.\n\n", sizeof("\nComanda incorrecta, has d'entrar un nombre enter d'accions.\n\n"));
                            }else{
                                write(1, "\nComanda incorrecta, has d'entrar un nombre d'accions mes petit.\n\n", sizeof("\nComanda incorrecta, has d'entrar un nombre d'accions mes petit.\n\n"));
                            }
                        }else{
                            write(1, "\nComanda incorrecta, has d'entrar un nombre de ticker existent.\n\n", sizeof("\nComanda incorrecta, has d'entrar un nombre de ticker existent.\n\n"));
                        }
                    }else if (!strcmp(sAccio, "erase")) {
                        //erase
                        //comprovar si hi ha numero enter
                        
                        //Lock del thread
                        sem_wait(semafor);
                    }else{
                        write(1, "\nComanda incorrecta\n\n", sizeof("\nComanda incorrecta\n\n"));
                    }
                }else{
                    write(1, "\nComanda incorrecta\n\n", sizeof("\nComanda incorrecta\n\n"));
                }
                free(sNombreAccions);
            }
            free(sAccio);
            free(sTicker);
        }
    }
    free(sComanda);
}
