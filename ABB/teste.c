#include <stdio.h>
#include <stdlib.h>

typedef struct _no{
	int valor;
	struct _no *esq;
	struct _no *dir;
}tnode;

typedef struct _arvore{
	tnode *raiz;
}tarv;


void constroi_abb(tarv *arv){
	arv->raiz = NULL;
}


void destroi_abb(tnode *node){
	if(node != NULL){
		destroi_abb(node->dir);
		destroi_abb(node->esq);
		free(node);
	}
}

void _destroi_abb(tarv *arv){
	destroi_abb(arv->raiz);
}


tnode *insere_abb(tnode *arv, int valor){
	if(arv == NULL){
		tnode *novo = (tnode*) malloc(sizeof(tnode));
		novo->dir = novo->esq = NULL;
		novo->valor = valor;
		return novo;
	}
	if(valor > arv->valor)
		arv->dir = insere_abb(arv->dir, valor);
	else if(valor < arv->valor)
		arv->esq = insere_abb(arv->esq, valor);

	return arv;
}

void _insere_abb(tarv *arv, int valor){
	arv->raiz = insere_abb(arv->raiz, valor);
}


tnode **busca_abb(tarv *arv, tnode **atual, int valor){
	if((*atual) != NULL){
		if((*atual)->valor == valor)
			return atual;
		if((*atual)->valor > valor)
			return busca_abb(arv, &(*atual)->esq, valor);

	return busca_abb(arv, &(*atual)->dir, valor);
	}
	return NULL;
}

tnode **_busca(tarv *arv, int valor){
	return busca_abb(arv, &(arv->raiz), valor);
}


void _remove_raiz(tnode ** raiz){
    tnode **sucessor;
    tnode **predecessor;
    tnode *aux;
    tnode *praiz;
    /*caso 1: no folha*/
    if ((((tnode *)*raiz)->esq == NULL) &&(((tnode *)*raiz)->dir == NULL)){
        free(*raiz);
        *raiz = NULL;
    }else if ((*raiz)->dir != NULL){ /*caso 2: remove sucessor*/
        sucessor = &((*raiz)->dir);
        while( (*sucessor)->esq != NULL)
            sucessor = &(*sucessor)->esq;
        aux = *sucessor;
        *sucessor = (*sucessor)->dir;
        if (aux->dir  != (*raiz)->dir)
            aux->dir  = (*raiz)->dir;
        aux->esq  = (*raiz)->esq;
        free(*raiz);
        *raiz = aux;
    }else{ /*caso 3: remove predecessor*/
        predecessor = &((*raiz)->esq);
        while( (*predecessor)->dir != NULL)
            predecessor = &(*predecessor)->dir;
        aux = *predecessor;
        *predecessor = (*predecessor)->esq;
        if (aux->esq  != (*raiz)->esq)
            aux->esq  = (*raiz)->esq;
        aux->dir  = (*raiz)->dir;
        free(*raiz);
        *raiz = aux;
    }
}

int abb_remove(tarv *arv, int valor){
    tnode ** node = abb_busca(arv,valor);
    int ret = EXIT_FAILURE;
    if (node != NULL){
        _remove_raiz(node);
        ret = EXIT_SUCCESS;
    }
    return ret;
}



int main(){

}