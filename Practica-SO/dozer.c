//
//  main.c
//  Fase 1
//
//  Created by Manel Roca & Guillermo Martínez-Ubierna on 2/11/14.
//  Copyright (c) 2014 Guille Martínez-Ubierna i Manel Roca. All rights reserved.
//

#include "LlistaPDI.h"
#include "Tipus_dozer.h"
#include "Fitxer_dozer.h"
#include "Shell.h"

#define SORIGEN 14
#define SDADES 100

Operador stOperador;
IpInfo stIP;
int sockGekko;
struct sockaddr_in servGekko;

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
    LlistaPDI_destrueix(&stOperador.llistaAccions);
    desconnexio();
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
    //Llegir resposta
    read(sockGekko, &trama, sizeof(trama));
    if (trama.Tipus != 'O') {
        write(1, "Error amb la connexió del servidor\n", strlen("Error amb la connexió del servidor\n"));
        return -1;
    }else{
        return 0;
    }
    
}

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

int main() {
    int file_stock, file_config, sortir = 0;
    
    stOperador.llistaAccions = LlistaPDI_crea();
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
    connexio();
    while(sortir == 0){
        Shell_analitzaComanda(&sortir, &stOperador);
    }
    desconnexio();
	return 0;
}

