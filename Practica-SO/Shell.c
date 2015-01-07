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
 *   @Nombre: escoltaGekko
 *   @Def: Función que se encarga de escuchar el socket del Gekko y decide que hacer en función de la trama                         que recibe.
 *   @Arg:   In: int sockGekko -> fd del socket que conecta con el Gekko.
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void* escoltaGekko(void * data){
    int sortir = 0;
    Trama trama;
    char sText[101];
    
    while (!sortir) {
        read((int) data, &trama, sizeof(trama));
        switch (trama.Tipus) {
            case 'X':
                //Show ibex
                showIbex(trama);
                break;
            case 'B':
                //Buy
                
                break;
            case 'S':
                //Sell
                
                break;
            case 'M':
                //Accions comprades per unaltre operador
                
                break;
            case 'D':
                //Quan s'esborra una venta????
                
                break;
            default:
                write(1, "Error amb la connexió del servidor\n", strlen("Error amb la connexió del servidor\n"));
                break;
        }
    }
    return NULL;
}

/*********************************************************************************************************
 *
 *   @Nombre: showIbex
 *   @Def: Función que muestra los datos del Ibex recibidos por el Gekko
 *   @Arg:   In:  Trama que recibe del Gekko de tipo 'X' y ha de mostrar los datos
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void showIbex(Trama trama){
    char sText[101];
    bzero(sText, sizeof(sText));
    sprintf(sText, "%s\n",trama.Data);
    write(1, sText, sizeof(sText));
}

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

void Shell_analitzaComanda(int * sortir, Operador* stOperador, int sockGekko){
    char sText[100];
    char *sComanda, *sComandes, *sNombreAccions, *sTicker;
    char cAux;
    int num = 0;
    int i = 0, mida = 0;
    Accio a;
    Trama trama;
    
    //Crear el thread que escoltara al Gekko
    pthread_t thread_id;
    pthread_attr_t attr;
    
    pthread_attr_init(&attr);
    pthread_create(&thread_id, NULL, escoltaGekko, (void *)sockGekko);
    
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
    }else{
        sComandes = (char *) strtok(sComanda, " ");
        if (!strcmp(sComandes, "buy")) {
            sComandes = (char*) strtok(NULL," ");
            if (NULL != sComandes) {
                strcpy(sTicker, sComandes);
                sComandes = (char*) strtok(NULL," ");
                if (NULL != sComandes) {
                    strcpy(sNombreAccions, sComandes);
                    sComandes = (char*) strtok(NULL," ");
                    if (NULL != sComandes) {
                        write(1, "\nComanda incorrecta\n\n", sizeof("\nComanda incorrecta\n\n"));
                    }else{
                        //Si hi ha tres paraules
                        //TODO comprovar-les i enviar
                    }
                }else{
                    write(1, "\nComanda incorrecta\n\n", sizeof("\nComanda incorrecta\n\n"));
                }
            }else  {
                write(1, "\nComanda incorrecta\n\n", sizeof("\nComanda incorrecta\n\n"));
            }
        }else if (!strcmp(sComandes, "sell")){
            sComandes = (char*) strtok(NULL," ");
            if (NULL != sComandes) {
                strcpy(sTicker, sComandes);
                sComandes = (char*) strtok(NULL," ");
                if (NULL != sComandes) {
                    strcpy(sNombreAccions, sComandes);
                    sComandes = (char*) strtok(NULL," ");
                    if (NULL != sComandes) {
                        write(1, "\nComanda incorrecta\n\n", sizeof("\nComanda incorrecta\n\n"));
                    }else{
                        //Si hi ha tres paraules
                        //TODO comprovar-les i enviar
                    }
                }else{
                    write(1, "\nComanda incorrecta\n\n", sizeof("\nComanda incorrecta\n\n"));
                }
            }else  {
                write(1, "\nComanda incorrecta\n\n", sizeof("\nComanda incorrecta\n\n"));
            }
        }else if (!strcmp(sComandes, "erase")) {
            sComandes = (char*) strtok(NULL," ");
            if (NULL != sComandes) {
                strcpy(sTicker, sComandes);
                sComandes = (char*) strtok(NULL," ");
                if (NULL != sComandes) {
                    strcpy(sNombreAccions, sComandes);
                    sComandes = (char*) strtok(NULL," ");
                    if (NULL != sComandes) {
                        write(1, "\nComanda incorrecta\n\n", sizeof("\nComanda incorrecta\n\n"));
                    }else{
                        //Si hi ha tres paraules
                        //TODO comprovar-les i enviar
                    }
                }else{
                    write(1, "\nComanda incorrecta\n\n", sizeof("\nComanda incorrecta\n\n"));
                }
            }else  {
                write(1, "\nComanda incorrecta\n\n", sizeof("\nComanda incorrecta\n\n"));
            }
        }else{
            write(1, "\nComanda incorrecta\n\n", sizeof("\nComanda incorrecta\n\n"));
        }
    }
    free(sComanda);
}
