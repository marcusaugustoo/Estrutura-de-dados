#include <stdlib.h>
#include <stdio.h>

typedef struct _no{
	int valor;
	struct _no *esq;
	struct _no *dir;
}tnode;

typedef struct _arv{
	tnode *raiz;
}tarv;


void _constroi_abb(tarv *arv){
	arv->raiz = NULL;
}

void _destroi_arvore(tnode *no){
	if(no != NULL){
		_destroi_arvore(no->esq);
		_destroi_arvore(no->dir);
		free(no);
	}
}

void destroi_arvore(tarv *arv){
	_destroi_arvore(arv->raiz);
}

tnode *_insere_abb(tnode *raiz, int valor){
	if(raiz == NULL){
		tnode *novo = (tnode*) malloc(sizeof(tnode));
		novo->valor = valor;
		novo->dir = novo->esq = NULL;
		return novo;
	}
	if(valor > raiz->valor)
		raiz->dir = _insere_abb(raiz->dir, valor);
	else if(valor < raiz->valor)
		raiz->esq = _insere_abb(raiz->esq, valor);

	return raiz;
}


void insere_abb(tarv *arv, int valor){
	arv->raiz = _insere_abb(arv->raiz, valor);
}


tnode **busca(tarv *arv, tnode **atual, int valor){
	if((*atual) != NULL){
		if(((*atual)->valor) == valor){
			return atual;
		}
		if(valor > ((*atual)->valor)){
			return busca(arv, &((*atual)->dir), valor);
		}

	return busca(arv, &((*atual)->esq), valor);
	}
	return NULL;
}


tnode **_busca(tarv *arv, int valor){
	return busca(arv, &((arv)->raiz), valor);
}


int main(){

	return EXIT_SUCCESS;
}