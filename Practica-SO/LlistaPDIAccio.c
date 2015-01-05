//
//  LlistaPDIAccio.c
//
//
//  Created by Manel Roca & Guillermo Martínez-Ubierna on 2/11/14.
//  Copyright (c) 2014 Guille Martínez-Ubierna i Manel Roca. All rights reserved.
//

#include "LlistaPDIAccio.h"
#include "Tipus_dozer.h"

LlistaPDIAccio LlistaPDIAccio_crea(){
    LlistaPDIAccio stLlista;
    stLlista.pstCap = (Nodec *)malloc(sizeof(Nodec));
    if (stLlista.pstCap == NULL) {
        write(1, "Error al crear la llista!\n", strlen("Error al crear la llista!\n"));
    }else{
        stLlista.pstAnt = stLlista.pstCap;
        stLlista.pstCap->pstSeg = NULL;
    }
    return stLlista;
}

void LlistaPDIAccio_insereix(LlistaPDIAccio * l, Accio a){
    Nodec * aux;
    l->pstAnt = l->pstCap;
    //Anar al final
    while (l->pstAnt->pstSeg != NULL) {
        l->pstAnt = l->pstAnt->pstSeg;
    }
    //inserir
    aux = (Nodec*)malloc(sizeof(Nodec));
    if (aux == NULL) {
         write(1, "Error al inserir!\n", strlen("Error al inserir!\n"));
    }else{
        aux->a.nAccions = a.nAccions;
        strcpy(aux->a.cTicker, a.cTicker);
        aux->pstSeg = l->pstAnt->pstSeg;
        l->pstAnt->pstSeg = aux;
        l->pstAnt = aux;
    }
    
}

Accio LlistaPDIAccio_consulta(LlistaPDIAccio l){
    Accio a;
    a.nAccions = -1;
    if (l.pstAnt->pstSeg != NULL) {
        a = l.pstAnt->pstSeg->a;
    }
    return a;
}

void LlistaPDIAccio_esborra(LlistaPDIAccio * l){
    Nodec * aux;
    if(l->pstAnt->pstSeg == NULL){
        write(1,"Error al esborrar!\n",strlen("Error al esborrar!\n"));
    }else{
        aux = l->pstAnt->pstSeg;
        l->pstAnt->pstSeg = aux->pstSeg;
        free(aux);
    }
}

void LlistaPDIAccio_avanca(LlistaPDIAccio * l){
    if (l->pstAnt->pstSeg == NULL) {
        write(1,"No es pot avançar!\n",strlen("No es pot avançar!\n"));
    }else{
        l->pstAnt = l->pstAnt->pstSeg;
    }
}
void LlistaPDIAccio_vesInici(LlistaPDIAccio * l){
    l->pstAnt = l->pstCap;
}

int LlistaPDIAccio_fi(LlistaPDIAccio l){
    return l.pstAnt->pstSeg == NULL;
}

int LlistaPDIAccio_buida(LlistaPDIAccio l){
    return l.pstCap->pstSeg == NULL;
}

void LlistaPDIAccio_destrueix(LlistaPDIAccio * l){
    Nodec * aux;
    while (l->pstCap != NULL) {
        aux = l->pstCap;
        l->pstCap = l->pstCap->pstSeg;
        free(aux);
    }
    l->pstAnt = NULL;
}
