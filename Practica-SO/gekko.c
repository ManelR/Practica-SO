//
//  main.c
//  Gekko
//
//  Created by Manel Roca & Guillermo Martínez-Ubierna on 2/11/14.
//  Copyright (c) 2014 Guille Martínez-Ubierna i Manel Roca. All rights reserved.
//

#include <stdio.h>
#include "Tipus_gekko.h"
#include "Fitxer_gekko.h"

#define SORIGEN 14
#define SDADES 100
#define INTENTSCONNEXIO 5

Accio ibex[35];
IpInfo stIP;
int sockTumb;
struct sockaddr_in servTumb;

void actualitzarInformacio();
int connexio();
void desconnexio();
void escoltaDozers();
void nouDozer(int newSockDozer);

/*********************************************************************************************************
 *
 *   @Nombre: ksighandler, kctrlc y kalarm
 *   @Def: Función que controla los signal del sistema.
 *   @Arg:   In: signum -> numero de interrupción
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void kctrlc(){
    desconnexio();
    exit(0);
}

void kalarm(){
    actualitzarInformacio();
    alarm(stIP.nSegons);
}

void ksighandler(int signum){
    switch (signum) {
        case SIGINT:
            kctrlc();
            break;
        case SIGALRM:
            kalarm();
            break;
        default:
            break;
    }
}

/*********************************************************************************************************
 *
 *   @Nombre: connexio
 *   @Def: Función que sirve para conectar Gekko con el servicio TumblingDice.
 *   @Arg:   In: -
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

int connexio(){
    Trama trama;
    int i;
    
    //Crear el socket
    sockTumb = socket(AF_INET, SOCK_STREAM, 0);
    if (sockTumb < 0) {
        write(1, "Error al crear el socket", strlen("Error al obrir el socket"));
        return -1;
    }
    
    servTumb.sin_addr.s_addr = inet_addr(stIP.sIP);
    servTumb.sin_family = AF_INET;
    servTumb.sin_port = htons(stIP.nPort);
    
    if (connect(sockTumb, (struct sockaddr*)&servTumb, sizeof(servTumb)) < 0) {
        write(1, "Error al obrir el socket\n", strlen("Error al obrir el socket\n"));
        return -1;
    }
    
    //Preparar la trama
    strcpy(trama.Origen, "Gekko");
    for (i = 5; i < SORIGEN; i++) {
        trama.Origen[i] = '\0';
    }
    trama.Tipus = 'C';
    strcpy(trama.Data, "CONNEXIO");
    for (i=8; i<SDADES; i++) {
        trama.Data[i] = '\0';
    }
    //Enviar
    write(sockTumb, &trama, sizeof(trama));
    //Llegir resposta
    read(sockTumb, &trama, sizeof(trama));
    if (trama.Tipus != 'O') {
        write(1, "Error amb la connexió del servidor\n", strlen("Error amb la connexió del servidor\n"));
        return -1;
    }else{
        return 0;
    }
    
}

/*********************************************************************************************************
 *
 *   @Nombre: actualitzarInformació
 *   @Def: Función que sirve para actualizar las dadas recibidas del servicio TumblingDice a la variable global ibex.
 *   @Arg:   In: -
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void actualitzarInformacio(){
    int i, max;
    int error = 0, sortir = 0;
    int nContador = 0, nContador2 = 0;
    char aux[100];
    char cOperacio;
    float fIncrement = 0.0;
    Trama trama;

    //Preparar trama de peticio
    strcpy(trama.Origen, "Gekko");
    for (i = 5; i < SORIGEN; i++) {
        trama.Origen[i] = '\0';
    }
    trama.Tipus = 'P';
    strcpy(trama.Data, "PETICIO");
    for (i=7; i<SDADES; i++) {
        trama.Data[i] = '\0';
    }
    
    write(sockTumb, &trama, sizeof(trama));
    for (i = 0; i < 35 && error == 0; i++) {
        if (read(sockTumb, &trama, sizeof(trama)) <= 0) {
            write(1, "Error al llegir les dades de l'IBEX\n", strlen("Error al llegir les dades de l'IBEX\n"));
            error = 1;
        }else{
            //Guardar les dades
            if (trama.Tipus == 'E') {
                write(1, "Error amb la peticio d'actualització\n", strlen("Error amb la peticio d'actualització\n"));
                error = 1;
            }else{
                nContador = 0;
                //Avançar el contador fins trobar el + o el -
                while (trama.Data[nContador] != '+' && trama.Data[nContador] != '-'){
                    nContador++;
                }
                //Guardar l'operació
                cOperacio = trama.Data[nContador++];
                nContador2 = 0;
                while (trama.Data[nContador] != '\0') {
                    aux[nContador2++] = trama.Data[nContador++];
                }
                aux[nContador2] = '\0';
                fIncrement = atof(aux);
                //Actualitzar les dades
                if (cOperacio == '+') {
                    ibex[i].fPreu += fIncrement;
                }else if (cOperacio == '-'){
                    ibex[i].fPreu -= fIncrement;
                }
            }
        }
    }
    nContador = 0;
    if(error == 1){
        write(1, "Error al enviar la petició al servidor\n\n", strlen("Error al enviar la petició al servidor\n"));
        //reconnexió
        sortir = -1;
        max = INTENTSCONNEXIO;
        while (nContador < INTENTSCONNEXIO && sortir == -1) {
            nContador++;
            bzero(aux, sizeof(aux));
            sprintf(aux, "Intentant reconnectar... (%d/%d)\n", nContador, max);
            write(1, aux, strlen(aux));
            sortir = connexio();
            sleep(5);
        }
        if (sortir == -1) {
            exit(0);
        }
    }else{
        write(1, "Dades actualitzades\n", strlen("Dades actualitzades\n"));
    }
}

/*********************************************************************************************************
 *
 *   @Nombre: desconnexio
 *   @Def: Función que sirve para desconectar Gekko con el servicio TumblingDice.
 *   @Arg:   In: -
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void desconnexio(){
    Trama trama;
    int i;

    //Preparar trama de peticio
    strcpy(trama.Origen, "Gekko");
    for (i = 5; i < SORIGEN; i++) {
        trama.Origen[i] = '\0';
    }
    trama.Tipus = 'Q';
    strcpy(trama.Data, "DESCONNEXIO");
    for (i=11; i<SDADES; i++) {
        trama.Data[i] = '\0';
    }
    write(sockTumb, &trama, sizeof(trama));
    close(sockTumb);
}

/*********************************************************************************************************
 *
 *   @Nombre: showIbex
 *   @Def: Función que sirve para enviar al Dozer el estado actual del IBEX
 *   @Arg:   In: fdDozer -> fd del Dozer que solicita la información.
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void showIbex(int fdDozer){
    Trama trama;
    
}

/*********************************************************************************************************
 *
 *   @Nombre: dozer
 *   @Def: Función que utilizan los diferentes threads para funcionar.
 *   @Arg:   In: int newSocket -> fd del socket que conecta con el dozer.
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void* dozer(void * data){
    Trama trama;
    int sortir = 0;
    char sFrase[50];
    char sOperador[14];
    
    if(read((int) data, &trama, sizeof(trama)) < 0){
        perror("ERROR reading from socket");
        return NULL;
    }else{
        if(trama.Tipus == 'C' && strcmp("CONNEXIO", trama.Data) == 0){
            strcpy(sOperador,trama.Origen);
            trama.Tipus = 'O';
            strcpy(trama.Data, "CONNEXIO OK");
            strcpy(trama.Origen, "Gekko");
            write((int) data, &trama, sizeof(trama));
        }else{
            trama.Tipus = 'E';
            strcpy(trama.Data, "ERROR");
            strcpy(trama.Origen, "Gekko");
        }
    }
    while (!sortir) {
        if(read((int) data, &trama, sizeof(trama)) < 0){
            perror("ERROR reading from socket");
            return NULL;
        }else{
            //S'ha llegit la trama
            switch (trama.Tipus) {
                case 'Q':
                    sortir = 1;
                    break;
                case 'X':
                    //Show IBEX
                    showIbex((int)data);
                    break;
                default:
                    break;
            }
        }
    }
    close((int) data);
    bzero(sFrase, sizeof(sFrase));
    sprintf(sFrase, "%s desconnectat\n", sOperador);
    write(1, sFrase, sizeof(sFrase));
    return NULL;
}

/*********************************************************************************************************
 *
 *   @Nombre: nouDozer
 *   @Def: Función que sirve para crear una nueva conexión.
 *   @Arg:   In: int newSocket -> fd del socket que conecta con el dozer.
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void nouDozer(int newSockDozer){
    pthread_t thread_id;
    pthread_attr_t attr;
    
    pthread_attr_init(&attr);
    pthread_create(&thread_id, NULL, dozer, (void *)newSockDozer);
}

/*********************************************************************************************************
 *
 *   @Nombre: escoltaDozers
 *   @Def: Función que utiliza Gekko para esperar la conexión de los diferentes Dozers.
 *   @Arg:   In: -
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void escoltaDozers(){
    int sockDozer;
    int newSockDozer;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    
    sockDozer = socket(AF_INET, SOCK_STREAM, 0);
    if (sockDozer < 0)
    {
        perror("ERROR opening socket");
        exit(-3);
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(8310);
    if (bind(sockDozer, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding");
        exit(-3);
    }
    listen(sockDozer,20);
    clilen = sizeof(cli_addr);

    while (1) {
        newSockDozer = accept(sockDozer, (struct sockaddr *) &cli_addr, &clilen);
        
        if (newSockDozer < 0) {
            write(1,"Error a l'establir la connexio\n",strlen("Error a l'establir la connexio\n"));
            close(sockDozer);
        }else{
            nouDozer(newSockDozer);
        }
        
    }

}

int main() {
    int file_config, file_ibex;
    int i;
    char sText[100];
    
    signal(SIGINT, ksighandler);
    signal(SIGALRM, ksighandler);
    
    file_config = open("config_tumblingdice.dat", O_RDONLY);
    if (file_config < 0) {
        write(1, "Error al abrir el fichero\n", strlen("Error al abrir el fichero\n"));
        exit(-1);
    }
    Fitxer_carregaFitxerConfig(file_config, &stIP);
    
    file_ibex = open("ibex.dat", O_RDONLY);
    if (file_ibex < 0) {
        write(1, "Error al abrir el fichero\n", strlen("Error al abrir el fichero\n"));
        exit(-1);
    }
    Fitxer_carregaFitxerIbex(file_ibex, ibex);
    write(1, "IBEX INFO\n", strlen("IBEX INFO\n"));
    for(i = 0; i<35;i++){
        bzero(sText, sizeof(sText));
        sprintf(sText, "%s\t%f\t%lld\n",ibex[i].cTicker,ibex[i].fPreu,ibex[i].llAccions);
        write(1, sText, strlen(sText));
    }
    
    bzero(sText, sizeof(sText));
    sprintf(sText, "\nIP INFO\n %s --- %d -- %d\n\n", stIP.sIP, stIP.nPort, stIP.nSegons);
    write(1, sText, strlen(sText));
    
    
    //Connexió
    if(connexio() < 0){
        exit(-1);
    }
    
    alarm(stIP.nSegons);
    
    escoltaDozers();
    
    return 0;
}
