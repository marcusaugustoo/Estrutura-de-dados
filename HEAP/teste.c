#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct 
{
	int *v;
	int max;
	int n;
}theap;

int filho_esquerda(int i){
	return 2*i+1;
}

int filho_direita(int i){
	return 2*i+2;
}

int pai(int i){
	if(i>0)
		return (i-1)/2;
	else
		return 0;
}

void constroi_heap(theap *heap, int max){
	heap->v = malloc(sizeof(int) * max);
	heap->max = max;
	heap->n = 0;
}

void destroi_heap(theap *heap){
	free(heap->v);
	heap->v = NULL;
	heap->max = 0;
	heap->n = 0;
}

void troca(int *a, int *b){
	int aux = *a;
	*a = *b;
	*b = aux;
}

void descer(theap *heap, int i){
	int esquerda = filho_esquerda(i);
	int direita = filho_direita(i);
	int imax = i;

	if(esquerda < heap->n && heap->v[esquerda] > heap->v[imax])
		imax = esquerda;
	if(direita < heap->n && heap->v[direita] > heap->v[imax])
		imax = direita;
	if(imax != i){
		troca(&(heap->v[imax]), &(heap->v[i]));
		descer(heap, imax);
	}
}

void descer_min(theap *heap, int i){
	int esquerda = filho_esquerda(i);
	int direita = filho_direita(i);
	int imin = i;

	if(esquerda < heap->n && heap->v[esquerda] < heap->v[imin])
		imin = esquerda;
	if(direita < heap->n && heap->v[direita] < heap->v[imin])
		imin = direita;
	if(imin != i){
		troca(&(heap->v[imin]), &(heap->v[i]));
		descer_min(heap, imin);
	}
}


void subir(theap *heap, int i){
	int _pai = pai(i);
	if((i > 0) && (i < heap->n) && heap->v[_pai] < heap->v[i]){
		troca(&(heap->v[_pai]), &(heap->v[i]));
		subir(heap, _pai);
	}
}

int insere_heap(theap *heap, int n){
	if(heap->n < heap->max){
		heap->v[heap->n] = n;
		heap->n++;
		subir(heap, heap->n-1);
		return EXIT_SUCCESS;
	}
	else
		return EXIT_FAILURE;
}

int acessa_MAX(theap *heap){
	return heap->v[0];
}

int extrai_MAX(theap *heap){
	int max = heap->v[0];
	int index = heap->n;
	heap->v[0] = heap->v[heap->n-1];
	heap->n = index-1;
	descer(heap, 0);
	return max;
}

void constroi_MAX_HEAP(theap *heap){
	for(int i = pai(heap->n-1); i>=0; i--)
		descer(heap, i);
}

void constroi_MIN_HEAP(theap *heap){
	for(int i = pai(heap->n-1); i>=0; i--)
		descer_min(heap, i);
}

void heapsort(theap *heap){
	int original = heap->n;
	constroi_MAX_HEAP(heap);
	for(int i = heap->n-1; i>0; i--){
		troca(&(heap->v[0]), &(heap->v[i]));
		heap->n--;
		descer(heap, 0);
	}

	heap->n = original;
}


int main()
{
	theap *heap = malloc(sizeof(theap));
	constroi_heap(heap, 10);
	heap->n = 7;

		int dados[] = { 50, 40, 30, 10, 8, 4, 3 };
	for (int i = 0; i < heap->n; i++) {
		heap->v[i] = dados[i];
	}

	printf("Antes de construir a MIN-HEAP:\n");
	for (int i = 0; i < heap->n; i++)
		printf("%d ", heap->v[i]);
	printf("\n");

	heapsort(heap);

	printf("Depois de construir a MIN-HEAP:\n");
	for (int i = 0; i < heap->n; i++)
		printf("%d ", heap->v[i]);
	printf("\n");

	return 0;
}