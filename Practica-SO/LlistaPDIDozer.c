//
//  LlistaPDIDozer.c
//
//
//  Created by Manel Roca & Guillermo Martínez-Ubierna on 2/11/14.
//  Copyright (c) 2014 Guille Martínez-Ubierna i Manel Roca. All rights reserved.
//

#include "LlistaPDIDozer.h"

LlistaPDIDozer LlistaPDIDozer_crea(){
    LlistaPDIDozer stLlista;
    stLlista.pstCap = (Noded *)malloc(sizeof(Noded));
    if (stLlista.pstCap == NULL) {
        write(1, "Error al crear la llista!\n", strlen("Error al crear la llista!\n"));
    }else{
        stLlista.pstAnt = stLlista.pstCap;
        stLlista.pstCap->pstSeg = NULL;
    }
    return stLlista;
}

void LlistaPDIDozer_insereix(LlistaPDIDozer * l, Dozer v){
    Noded * aux;
    l->pstAnt = l->pstCap;
    //Anar al final
    while (l->pstAnt->pstSeg != NULL) {
        l->pstAnt = l->pstAnt->pstSeg;
    }
    //inserir
    aux = (Noded*)malloc(sizeof(Noded));
    if (aux == NULL) {
         write(1, "Error al inserir!\n", strlen("Error al inserir!\n"));
    }else{
        aux->d.nSocket = v.nSocket;
        aux->d.trama = v.trama;
        strcpy(aux->d.sOperador, v.sOperador);
        aux->pstSeg = l->pstAnt->pstSeg;
        l->pstAnt->pstSeg = aux;
        l->pstAnt = aux;
    }
    
}

Dozer LlistaPDIDozer_consulta(LlistaPDIDozer l){
    Dozer v;
    v.nSocket = -1;
    if (l.pstAnt->pstSeg != NULL) {
        v.trama = l.pstAnt->pstSeg->d.trama;
        v.nSocket = l.pstAnt->pstSeg->d.nSocket;
        strcpy(v.sOperador, l.pstAnt->pstSeg->d.sOperador);
    }
    return v;
}

void LlistaPDIDozer_esborra(LlistaPDIDozer * l){
    Noded * aux;
    if(l->pstAnt->pstSeg == NULL){
        write(1,"Error al esborrar!\n",strlen("Error al esborrar!\n"));
    }else{
        aux = l->pstAnt->pstSeg;
        l->pstAnt->pstSeg = aux->pstSeg;
        free(aux);
    }
}

void LlistaPDIDozer_avanca(LlistaPDIDozer * l){
    if (l->pstAnt->pstSeg == NULL) {
        write(1,"No es pot avançar!\n",strlen("No es pot avançar!\n"));
    }else{
        l->pstAnt = l->pstAnt->pstSeg;
    }
}
void LlistaPDIDozer_vesInici(LlistaPDIDozer * l){
    l->pstAnt = l->pstCap;
}

int LlistaPDIDozer_fi(LlistaPDIDozer l){
    return l.pstAnt->pstSeg == NULL;
}

int LlistaPDIDozer_buida(LlistaPDIDozer l){
    return l.pstCap->pstSeg == NULL;
}

void LlistaPDIDozer_destrueix(LlistaPDIDozer * l){
    Noded * aux;
    while (l->pstCap != NULL) {
        aux = l->pstCap;
        l->pstCap = l->pstCap->pstSeg;
        free(aux);
    }
    l->pstAnt = NULL;
}
