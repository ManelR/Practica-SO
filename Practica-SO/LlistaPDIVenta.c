//
//  LlistaPDIVenta.c
//
//
//  Created by Manel Roca & Guillermo Martínez-Ubierna on 2/11/14.
//  Copyright (c) 2014 Guille Martínez-Ubierna i Manel Roca. All rights reserved.
//

#include "LlistaPDIVenta.h"
#include "Tipus_dozer.h"

LlistaPDIVenta LlistaPDIVenta_crea(){
    LlistaPDIVenta stLlista;
    stLlista.pstCap = (Nodev *)malloc(sizeof(Nodev));
    if (stLlista.pstCap == NULL) {
        write(1, "Error al crear la llista!\n", strlen("Error al crear la llista!\n"));
    }else{
        stLlista.pstAnt = stLlista.pstCap;
        stLlista.pstCap->pstSeg = NULL;
    }
    return stLlista;
}

void LlistaPDIVenta_insereix(LlistaPDIVenta * l, Venta v){
    Nodev * aux;
    l->pstAnt = l->pstCap;
    //Anar al final
    while (l->pstAnt->pstSeg != NULL) {
        l->pstAnt = l->pstAnt->pstSeg;
    }
    //inserir
    aux = (Nodev*)malloc(sizeof(Nodev));
    if (aux == NULL) {
         write(1, "Error al inserir!\n", strlen("Error al inserir!\n"));
    }else{
        aux->v.nNumAccions = v.nNumAccions;
        aux->v.nSocket = v.nSocket;
        strcpy(aux->v.sOperador, v.sOperador);
        aux->pstSeg = l->pstAnt->pstSeg;
        l->pstAnt->pstSeg = aux;
        l->pstAnt = aux;
    }
    
}

Venta LlistaPDIVenta_consulta(LlistaPDIVenta l){
    Venta v;
    v.nNumAccions = -1;
    if (l.pstAnt->pstSeg != NULL) {
        v.nNumAccions = l.pstAnt->pstSeg->v.nNumAccions;
        v.nSocket = l.pstAnt->pstSeg->v.nSocket;
        strcpy(v.sOperador, l.pstAnt->pstSeg->v.sOperador);
    }
    return v;
}

void LlistaPDIVenta_esborra(LlistaPDIVenta * l){
    Nodev * aux;
    if(l->pstAnt->pstSeg == NULL){
        write(1,"Error al esborrar!\n",strlen("Error al esborrar!\n"));
    }else{
        aux = l->pstAnt->pstSeg;
        l->pstAnt->pstSeg = aux->pstSeg;
        free(aux);
    }
}

void LlistaPDIVenta_avanca(LlistaPDIVenta * l){
    if (l->pstAnt->pstSeg == NULL) {
        write(1,"No es pot avançar!\n",strlen("No es pot avançar!\n"));
    }else{
        l->pstAnt = l->pstAnt->pstSeg;
    }
}
void LlistaPDIVenta_vesInici(LlistaPDIVenta * l){
    l->pstAnt = l->pstCap;
}

int LlistaPDIVenta_fi(LlistaPDIVenta l){
    return l.pstAnt->pstSeg == NULL;
}

int LlistaPDIVenta_buida(LlistaPDIVenta l){
    return l.pstCap->pstSeg == NULL;
}

void LlistaPDIVenta_destrueix(LlistaPDIVenta * l){
    Nodev * aux;
    while (l->pstCap != NULL) {
        aux = l->pstCap;
        l->pstCap = l->pstCap->pstSeg;
        free(aux);
    }
    l->pstAnt = NULL;
}
