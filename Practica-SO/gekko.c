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
InfoVentes ventes[35];
IpInfo stIP;
int sockTumb, nPeticio = 0;
struct sockaddr_in servTumb;
AccioXML ibexXML[35];

//Prova git

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
    int i = 0;
    for (i = 0; i < 35; i++) {
        LlistaPDIVenta_destrueix(&ventes[i].llista);
    }
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
        nPeticio++;
        if (nPeticio == 1){
            //Guardem preus inicials
            for (i = 0; i < 35; i++) {
                strcpy(ibexXML[i].cTicker, ibex[i].cTicker);
                ibexXML[i].fPreuInicial = ibex[i].fPreu;
                ibexXML[i].fPreuMaxim = ibex[i].fPreu;
                ibexXML[i].fPreuMinim = ibex[i].fPreu;
            }
        }else{
            //Anar actualitzant preus maxims i minims
            for (i = 0; i < 35; i++) {
                if (ibex[i].fPreu > ibexXML[i].fPreuMaxim) {
                    ibexXML[i].fPreuMaxim = ibex[i].fPreu;
                }
                if (ibex[i].fPreu < ibexXML[i].fPreuMaxim) {
                    ibexXML[i].fPreuMinim = ibex[i].fPreu;
                }
            }
            if(nPeticio == stIP.nPeticio){
                for (i = 0; i < 35; i++) {
                    ibexXML[i].fPreuFinal = ibex[i].fPreu;
                }
                //Actualitzem el fitxer
                //Fitxer_actualitzaXML(ibexXML);
                nPeticio = 0;
            }
        }
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
    int i = 0;
    char sText[100];
    long long auxNumAccions;
    //Preparar la trama
    strcpy(trama.Origen, "Gekko");
    trama.Tipus = 'X';
    
    //Preparar les dades i enviar
    for (i = 0; i < 35; i++) {
        auxNumAccions = ibex[i].llAccions;
        //Accions de les llistes
        LlistaPDIVenta_vesInici(&ventes[i].llista);
        while (!LlistaPDIVenta_fi(ventes[i].llista)) {
            auxNumAccions += LlistaPDIVenta_consulta(ventes[i].llista).nNumAccions;
            LlistaPDIVenta_avanca(&ventes[i].llista);
        }
        bzero(sText, sizeof(sText));
        sprintf(sText, "%s\t%f\t%lld", ibex[i].cTicker, ibex[i].fPreu, auxNumAccions);
        strcpy(trama.Data, sText);
        //Enviar
        write(fdDozer, &trama, sizeof(trama));
    }
    
}

/*********************************************************************************************************
 *
 *   @Nombre: sendToDozerBuy
 *   @Def: Función que sirve para enviar la información de la venta al Dozer.
 *   @Arg:   In: fdDozer -> fd del Dozer que solicita la información.
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void sendToDozerBuy(int fdDozer, char* sNom, int nNumAccions, float fDiners){
    Trama trama;
    char sMissatge[100];
    
    strcpy(trama.Origen, "Gekko");
    trama.Tipus = 'M';
    bzero(sMissatge, sizeof(sMissatge));
    sprintf(sMissatge, "%s-%d-%f", sNom, nNumAccions, fDiners);
    strcpy(trama.Data, sMissatge);
    write(fdDozer, &trama, sizeof(trama));
}

/*********************************************************************************************************
 *
 *   @Nombre: buy
 *   @Def: Función que sirve para gestionar las ventas del dozer.
 *   @Arg:   In: fdDozer -> fd del Dozer que solicita la información.
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void buy(int fdDozer, Trama trama){
    char sText[100];
    char sNom[10];
    int sortirAccions = 0;
    int nNumAccions = 0;
    int nError = 0;
    float fDiners = 0.0;
    int i = 0, j = 0;
    Trama tramaEnviar;
    Venta auxVenta;
    
    strcpy(tramaEnviar.Origen, "Gekko");
    tramaEnviar.Tipus = 'B';
    
    //Analitzar Dades
    while (trama.Data[i] != '-') {
        sText[i] = trama.Data[i];
        i++;
    }
    sText[i] = '\0';
    strcpy(sNom, sText);
    j = 0;
    i++;
    while (trama.Data[i] != '-') {
        sText[j] = trama.Data[i];
        i++;
        j++;
    }
    sText[j] = '\0';
    nNumAccions = atoi(sText);
    j = 0;
    i++;
    while (trama.Data[i] != '\0') {
        sText[j] = trama.Data[i];
        i++;
        j++;
    }
    sText[j] = '\0';
    fDiners = atof(sText);
    
    //Comprovar si les accions existeixen
    j = -1;
    for (i = 0; i < 35; i++) {
        if(strcasecmp(ibex[i].cTicker, sNom) == 0){
            j = i;
        }
    }
    if (j == -1) {
        strcpy(tramaEnviar.Data, "Error. L'acció no està dins IBEX35");
    }else{
        //Comprovar disponiblitat de les Accions a les llistes dels Dozers
        LlistaPDIVenta_vesInici(&ventes[j].llista);
        while (!LlistaPDIVenta_fi(ventes[j].llista) && sortirAccions == 0) {
            //S'ha trobat accions per comprar
            auxVenta = LlistaPDIVenta_consulta(ventes[j].llista);
            if (auxVenta.nNumAccions == nNumAccions) {
                //Hi ha les accions necessaries
                LlistaPDIVenta_esborra(&ventes[j].llista);
                //Enviar al Dozer que té les accions en venta
                sendToDozerBuy(auxVenta.nSocket, ibex[j].cTicker, nNumAccions, ibex[j].fPreu * nNumAccions);
                sortirAccions = 1;
            }else if (auxVenta.nNumAccions > nNumAccions){
                //Es pot comprar pero cal modificar la llista
                auxVenta.nNumAccions = auxVenta.nNumAccions - nNumAccions;
                LlistaPDIVenta_esborra(&ventes[j].llista);
                LlistaPDIVenta_insereix(&ventes[j].llista, auxVenta);
                //Enviar al Dozer que té les accions en venta
                sendToDozerBuy(auxVenta.nSocket, ibex[j].cTicker, nNumAccions, ibex[j].fPreu * nNumAccions);
                sortirAccions = 1;
            }else if (auxVenta.nNumAccions < nNumAccions){
                //Es pot comprar d'aqui pero cal avançar
                LlistaPDIVenta_esborra(&ventes[j].llista);
                //Enviar al Dozer que té les accions en venta
                sendToDozerBuy(auxVenta.nSocket, ibex[j].cTicker, nNumAccions, ibex[j].fPreu * nNumAccions);
                //Decrementar el numero d'accions que encara s'han de comprar
                nNumAccions = nNumAccions - auxVenta.nNumAccions;
                LlistaPDIVenta_avanca(&ventes[j].llista);
            }
        }
        //Si falten agafar del Gekko
        if (sortirAccions == 0) {
            if (ibex[j].llAccions >= nNumAccions) {
                ibex[j].llAccions = ibex[j].llAccions - nNumAccions;
            }else{
                nError = 1;
            }
        }
        //Comprovar diners totals
        if( nError == 0){
            if (ibex[j].fPreu * nNumAccions > fDiners) {
                strcpy(tramaEnviar.Data, "Error. Capital insuficient");
            }else{
                bzero(sText, sizeof(sText));
                sprintf(sText, "%f-%s-%d", ibex[j].fPreu * nNumAccions, ibex[j].cTicker, nNumAccions);
                strcpy(tramaEnviar.Data, sText);
            }
        }else{
            strcpy(tramaEnviar.Data, "Error. No hi ha suficients accions");
        }
    }
    //Enviar missatge
    write(fdDozer, &tramaEnviar, sizeof(tramaEnviar));
}


void sell(int fdDozer, Trama trama){
    Trama tramaEnviar;
    char sText[100];
    int nContador = 0;
    char sTicker[10];
    int nPosicio = 0;
    int numAccions = 0;
    int trobat = 0;
    int i;
    Venta auxVenta;
    
    strcpy(tramaEnviar.Origen, "Gekko");
    tramaEnviar.Tipus = 'S';
    
    //Analitzar les dades
    while (trama.Data[nContador] != '-' && nContador < 100) {
        sText[nContador] = trama.Data[nContador];
        nContador++;
    }
    sText[nContador] = '\0';
    nContador++;
    nPosicio = 0;
    strcpy(sTicker, sText);
    while (trama.Data[nContador] != '\0' && nContador < 100) {
        sText[nPosicio] = trama.Data[nContador];
        nContador++;
        nPosicio++;
    }
    sText[nPosicio] = '\0';
    numAccions = atoi(sText);
    
    //Comprovar les dades
    for (nContador = 0; nContador < 35 && !trobat; nContador++) {
        if (!strcasecmp(sTicker, ibex[nContador].cTicker)) {
            trobat = 1;
            nPosicio = nContador;
        }
    }
    
    if (trobat == 1) {
        sprintf(tramaEnviar.Data, "%d-%s", numAccions, sTicker);
        for (i = strlen(tramaEnviar.Data); i < SDADES; i++) {
            tramaEnviar.Data[i] = '\0';
        }
        //Guardar la venta
        //Afegir al final
        while (!LlistaPDIVenta_fi(ventes[nPosicio].llista)) {
            LlistaPDIVenta_avanca(&ventes[nPosicio].llista);
        }
        auxVenta.nNumAccions = numAccions;
        auxVenta.nSocket = fdDozer;
        strcpy(auxVenta.sOperador, trama.Origen);
        LlistaPDIVenta_insereix(&ventes[nPosicio].llista, auxVenta);
    }else{
        strcpy(tramaEnviar.Data, "Error amb la venta.");
    }
    
    //Enviar
    write(fdDozer, &tramaEnviar, sizeof(tramaEnviar));
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
                case 'B':
                    //BUY
                    buy((int)data, trama);
                    break;
                case 'S':
                    sell((int)data, trama);
                    break;
                default:
                    break;
            }
        }
    }
    close((int) data);
    bzero(sFrase, sizeof(sFrase));
    sprintf(sFrase, "%s desconnectat\n", sOperador);
    write(1, sFrase, strlen(sFrase));
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
    
    //Inicialització de les llistes de ventes
    for (i = 0; i < 35; i++) {
        ventes[i].llista = LlistaPDIVenta_crea();
    }
    
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
    Fitxer_carregaFitxerIbex(file_ibex, ibex, ventes);
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
