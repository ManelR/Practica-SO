//
//  main.c
//  Fase 1
//
//  Created by Manel Roca & Guillermo Martínez-Ubierna on 2/11/14.
//  Copyright (c) 2014 Guille Martínez-Ubierna i Manel Roca. All rights reserved.
//

#include "LlistaPDIAccio.h"
#include "Tipus_dozer.h"
#include "Fitxer_dozer.h"
#include "Shell.h"

#define SORIGEN 14
#define SDADES 100

Operador stOperador;
IpInfo stIP;
int sockGekko;
struct sockaddr_in servGekko;
sem_t semafor;
pthread_t thread_id;

int connexio();
void desconnexio();

/*********************************************************************************************************
 *
 *   @Nombre: ksighandler y kctrlc
 *   @Def: Función que controla los signal del sistema.
 *   @Arg:   In: signum -> numero de interrupción
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void kctrlc(){
    pthread_cancel(thread_id);
    desconnexio();
    Fitxer_guardaFitxerStock(&stOperador);
    LlistaPDIAccio_destrueix(&stOperador.llistaAccions);
    sem_destroy(&semafor);
    exit(0);
}

void ksighandler(int signum){
    switch (signum) {
        case SIGINT:
            kctrlc();
            break;
        default:
            break;
    }
}

/*********************************************************************************************************
 *
 *   @Nombre: connexio()
 *   @Def: Función que permite la conexión del operador con el Gekko.
 *   @Arg:   In: -
 *           Out: -
 *   @Ret: retorna un int para indicar si se ha podido establecer la conexión
 *
 *********************************************************************************************************/

int connexio(){
    Trama trama;
    int i;
    
    //Crear el socket
    sockGekko = socket(AF_INET, SOCK_STREAM, 0);
    if (sockGekko < 0) {
        write(1, "Error al obrir el socket", strlen("Error al obrir el socket"));
        return -1;
    }
    
    servGekko.sin_addr.s_addr = inet_addr(stIP.sIP);
    servGekko.sin_family = AF_INET;
    servGekko.sin_port = htons(stIP.nPort);
    
    if (connect(sockGekko, (struct sockaddr*)&servGekko, sizeof(servGekko)) < 0) {
        write(1, "Error al obrir el socket\n", strlen("Error al obrir el socket\n"));
        return -1;
    }
    
    //Preparar la trama
    strcpy(trama.Origen, stOperador.cNom);
    for (i = strlen(trama.Origen); i < SORIGEN; i++) {
        trama.Origen[i] = '\0';
    }
    trama.Tipus = 'C';
    strcpy(trama.Data, "CONNEXIO");
    for (i=8; i < SDADES; i++) {
        trama.Data[i] = '\0';
    }
    //Enviar
    write(sockGekko, &trama, sizeof(trama));
    //Llegir resposta i comprovar si s'ha pogut establir la connexio
    read(sockGekko, &trama, sizeof(trama));
    if (trama.Tipus != 'O') {
        write(1, "Error amb la connexió del servidor\n", strlen("Error amb la connexió del servidor\n"));
        return -1;
    }else{
        return 0;
    }
    
}

/*********************************************************************************************************
 *
 *   @Nombre: desconnexio()
 *   @Def: Función que permite la desconexión del operador con el Gekko.
 *   @Arg:   In: -
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void desconnexio(){
    Trama trama;
    int i;
    
    //Preparar trama de peticio
    strcpy(trama.Origen, stOperador.cNom);
    for (i = strlen(trama.Origen); i < SORIGEN; i++) {
        trama.Origen[i] = '\0';
    }
    trama.Tipus = 'Q';
    strcpy(trama.Data, "DESCONNEXIO");
    for (i=11; i<SDADES; i++) {
        trama.Data[i] = '\0';
    }
    write(sockGekko, &trama, sizeof(trama));
    close(sockGekko);
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
    write(1, sText, strlen(sText));
}

/*********************************************************************************************************
 *
 *   @Nombre: buy
 *   @Def: Función que muestra los datos de compra de las acciones
 *   @Arg:   In:  Trama que recibe del Gekko de tipo 'B' y ha de mostrar los datos
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void buy(Trama trama){
    char *sAux;
    char sText[100];
    int i = 0, j = 0, trobat = 0;
    float preu;
    Accio a;
    
    if (trama.Data[0] == 'E') {
        strcpy(sText, trama.Data);
        strcat(sText, "\n");
        write(1,sText,strlen(sText));
    }else{
        //rebem preu-ticker-nombre accions
        sAux = (char*)malloc(sizeof(char));
        while (trama.Data[i] != '-') {
            sAux[i] = trama.Data[i];
            i++;
            sAux = (char*)realloc(sAux, sizeof(char) * (i+1));
        }
        sAux[i] = '\0';
        preu = atof(sAux);
        //actualitzem el preu
        stOperador.fDinersTotals = stOperador.fDinersTotals - preu;
        i++;
        while (trama.Data[i] != '-') {
            sAux[j] = trama.Data[i];
            i++;
            j++;
            sAux = (char*)realloc(sAux, sizeof(char) * (j+1));
        }
        sAux[j] = '\0';
        i++;
        //Buscar ticker a la llista
        LlistaPDIAccio_vesInici(&stOperador.llistaAccions);
        while (!LlistaPDIAccio_fi(stOperador.llistaAccions)) {
            a = LlistaPDIAccio_consulta(stOperador.llistaAccions);
            if(!strcasecmp(a.cTicker,sAux)){
                trobat = 1;
                break;
            }
            if(!trobat) LlistaPDIAccio_avanca(&stOperador.llistaAccions);
        }
        strcpy(a.cTicker,sAux);
        j = 0;
        while (trama.Data[i] != '\0') {
            sAux[j] = trama.Data[i];
            i++;
            j++;
            sAux = (char*)realloc(sAux, sizeof(char) * (j+1));
        }
        sAux[j] = '\0';
        for(i = 0; i < (int) strlen(a.cTicker);i++) {
            if ((a.cTicker[i] >= 'a' ) && (a.cTicker[i] <= 'z')) a.cTicker[i] -= ('a'-'A');
        }
        if(!trobat){
            //afegir a la llista
            a.nAccions = atoi(sAux);
        }else{
            //actualitzar nombre accions
            LlistaPDIAccio_esborra(&stOperador.llistaAccions);
            a.nAccions = a.nAccions + atoi(sAux);
        }
        LlistaPDIAccio_insereix(&stOperador.llistaAccions, a);
        bzero(sText, sizeof(sText));
        sprintf(sText, "Compra realitzada. Cost: %.2f €\n", preu);
        write(1, sText, strlen(sText));
        free(sAux);
    }
}

/*********************************************************************************************************
 *
 *   @Nombre: sell
 *   @Def: Función que muestra los datos de las acciones puestas a la venta
 *   @Arg:   In:  Trama que recibe del Gekko de tipo 'S' y ha de mostrar los datos
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void sell(Trama trama){
    char sText[100];
    char *sAux;
    int i, j, nAccions;
    Accio a;
    
    if (trama.Data[0] == 'E') {
        strcpy(sText, trama.Data);
        strcat(sText, "\n");
        write(1,sText,strlen(sText));
    }else{
        //Rebo Nombre accions-ticker
        sAux = (char*)malloc(sizeof(char));
        i = 0;
        while (trama.Data[i] != '-') {
            sAux[i] = trama.Data[i];
            i++;
            sAux = (char*)realloc(sAux, sizeof(char) * (i+1));
        }
        sAux[i] = '\0';
        i++;
        nAccions = atoi(sAux);
        //Llegir ticker
        j = 0;
        while (trama.Data[i] != '\0') {
            sAux[j] = trama.Data[i];
            i++;
            j++;
            sAux = (char*)realloc(sAux, sizeof(char) * (j+1));
        }
        sAux[j] = '\0';
        for(i = 0; i < (int) strlen(a.cTicker);i++) {
            if ((a.cTicker[i] >= 'a' ) && (a.cTicker[i] <= 'z')) a.cTicker[i] -= ('a'-'A');
        }
        //Buscar ticker a la llista
        LlistaPDIAccio_vesInici(&stOperador.llistaAccions);
        while (!LlistaPDIAccio_fi(stOperador.llistaAccions)) {
            a = LlistaPDIAccio_consulta(stOperador.llistaAccions);
            if(!strcasecmp(a.cTicker,sAux)){
                LlistaPDIAccio_esborra(&stOperador.llistaAccions);
                a.nAccions = a.nAccions - nAccions;
                if(a.nAccions != 0) LlistaPDIAccio_insereix(&stOperador.llistaAccions, a);
                break;
            }
            LlistaPDIAccio_avanca(&stOperador.llistaAccions);
        }
        free(sAux);
        write(1,"OK. Accions posades a la venda.\n",strlen("OK. Accions posades a la venda.\n"));
    }
}

/*********************************************************************************************************
 *
 *   @Nombre: esborra
 *   @Def: Función que muestra si se han podido cancelar acciones puestas a la venta anteriormente
 *   @Arg:   In:  Trama que recibe del Gekko de tipo 'D' y ha de mostrar los datos
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void esborra(Trama trama){
    char sText[100];
    char *sAux;
    int i, j, nAccions, trobat = 0;
    Accio a;
    
    if (trama.Data[0] == 'E') {
        strcpy(sText, trama.Data);
        strcat(sText, "\n");
        write(1,sText,strlen(sText));
    }else{
        //Rebo Nombre accions-ticker
        sAux = (char*)malloc(sizeof(char));
        i = 0;
        while (trama.Data[i] != '-') {
            sAux[i] = trama.Data[i];
            i++;
            sAux = (char*)realloc(sAux, sizeof(char) * (i+1));
        }
        sAux[i] = '\0';
        i++;
        nAccions = atoi(sAux);
        //Llegir ticker
        j = 0;
        while (trama.Data[i] != '\0') {
            sAux[j] = trama.Data[i];
            i++;
            j++;
            sAux = (char*)realloc(sAux, sizeof(char) * (j+1));
        }
        sAux[j] = '\0';
        //Buscar ticker a la llista
        LlistaPDIAccio_vesInici(&stOperador.llistaAccions);
        while (!LlistaPDIAccio_fi(stOperador.llistaAccions)) {
            a = LlistaPDIAccio_consulta(stOperador.llistaAccions);
            if(!strcasecmp(a.cTicker,sAux)){
                trobat = 1;
            }
            if(!trobat){
                LlistaPDIAccio_avanca(&stOperador.llistaAccions);
            }else{
                break;
            }
        }
        strcpy(a.cTicker,sAux);
        for(i = 0; i < (int) strlen(a.cTicker);i++) {
            if ((a.cTicker[i] >= 'a' ) && (a.cTicker[i] <= 'z')) a.cTicker[i] -= ('a'-'A');
        }
        if(!trobat){
            //afegir a la llista
            a.nAccions = nAccions;
        }else{
            //actualitzar nombre accions
            LlistaPDIAccio_esborra(&stOperador.llistaAccions);
            a.nAccions = a.nAccions + nAccions;
        }
        LlistaPDIAccio_insereix(&stOperador.llistaAccions, a);
        free(sAux);
        write(1,"Venda anul·lada.\n",strlen("Venda anul·lada.\n"));
    }
}

/*********************************************************************************************************
 *
 *   @Nombre: vengut
 *   @Def: Función que muestra por cuanto se han vendido las acciones que ya estaban a la venta
 *   @Arg:   In:  Trama que recibe del Gekko de tipo 'M' y ha de mostrar los datos
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void vengut(Trama trama){
    char sAux[100], sText[100], sTicker[10];
    int numAccions = 0;
    int i, j;
    float preu;
    
    //Rebo ticker-naccions-preu
    i = 0;
    while (trama.Data[i] != '-') {
        sAux[i] = trama.Data[i];
        i++;
    }
    sAux[i] = '\0';
    strcpy(sTicker, sAux);
    i++;
    j = 0;
    while (trama.Data[i] != '-') {
        sAux[j] = trama.Data[i];
        i++;
        j++;
    }
    sAux[j] = '\0';
    numAccions = atoi(sAux);
    i++;
    j = 0;
    while (trama.Data[i] != '\0') {
        sAux[j] = trama.Data[i];
        i++;
        j++;
    }
    sAux[j] = '\0';
    preu = atof(sAux);
    //Mostrem missatge per pantalla
    bzero(sText, sizeof(sText));
    sprintf(sText, "\n\n[GEKKO]: Venta realitzada. %s %d accions. %.2f €.\n\n", sTicker, numAccions, preu);
    write(1, sText, strlen(sText));
    //Actualitzem preu
    stOperador.fDinersTotals = stOperador.fDinersTotals + preu;
}

/*********************************************************************************************************
 *
 *   @Nombre: escoltaGekko
 *   @Def: Función que se encarga de escuchar el socket del Gekko y decide que hacer en función de la
 *         trama que recibe.
 *   @Arg:   In: int sockGekko -> fd del socket que conecta con el Gekko.
 *           Out: -
 *   @Ret: -
 *
 *********************************************************************************************************/

void* escoltaGekko(void * data){
    int* sortir;
    Trama trama;
    int nContador = 0;
    sortir = (int*)data;
    
    while ((*sortir) == 0) {
        if(read(sockGekko, &trama, sizeof(trama)) != 0){
            
            switch (trama.Tipus) {
                case 'X':
                    //Show ibex
                    showIbex(trama);
                    nContador++;
                    if (nContador == 35) {
                        sem_post(&semafor);
                        nContador = 0;
                    }
                    break;
                case 'B':
                    //Buy
                    buy(trama);
                    sem_post(&semafor);
                    break;
                case 'S':
                    //Sell
                    sell(trama);
                    sem_post(&semafor);
                    break;
                case 'M':
                    //Accions comprades per unaltre operador
                    vengut(trama);
                    break;
                case 'D':
                    //Quan s'esborra una venta
                    esborra(trama);
                    sem_post(&semafor);
                    break;
                default:
                    write(1, "Error amb la connexió del servidor\n", strlen("Error amb la connexió del servidor\n"));
                    break;
            }
        }else{
            *sortir = 1;
            //signal perque no es quedi bloquejat.
            sem_post(&semafor);
        }
    }
    return NULL;
}

int main() {
    int file_stock, file_config, sortir = 0, sortir2 = 0;
    //Inicialitzem el semafor pel thread que escoltara les trames del Gekko
    sem_init(&semafor, 0, 0);
    
    stOperador.llistaAccions = LlistaPDIAccio_crea();
    signal(SIGINT, ksighandler);
    
    file_stock = open("stock.dat", O_RDONLY);
    if(file_stock < 0){
        write(1, "Error al abrir el fichero\n", strlen("Error al abrir el fichero\n"));
        exit(-1);
    }
    Fitxer_carregaFitxerStock(file_stock, &stOperador);
    
    file_config = open("config.dat", O_RDONLY);
    if (file_config < 0) {
        write(1, "Error al abrir el fichero\n", strlen("Error al abrir el fichero\n"));
        exit(-1);
    }
    Fitxer_carregaFitxerConfig(file_config, &stIP);
    //Connexió
    if(connexio() < 0){
        exit(-1);
    }
    
    //Crear el thread que escoltara al Gekko
    
    pthread_attr_t attr;
    
    pthread_attr_init(&attr);
    pthread_create(&thread_id, NULL, escoltaGekko, (void *)&sortir);
    
    while(sortir2 == 0 && sortir == 0){
        Shell_analitzaComanda(&sortir2, &stOperador, sockGekko, &semafor);
    }
    if (sortir2 != 0) {
        pthread_cancel(thread_id);
    }else{
        pthread_join(thread_id, NULL);
        write(1, "El gekko ha mort\n", strlen("El gekko ha mort\n"));
        write(1, "\nSayonara\n", strlen("\nSayonara\n"));
    }
    desconnexio();
    Fitxer_guardaFitxerStock(&stOperador);
    LlistaPDIAccio_destrueix(&stOperador.llistaAccions);
    sem_destroy(&semafor);
	return 0;
}

