#include <stdio.h>
#include <stdlib.h>

typedef struct{
	int *v;
	int max;
	int n;
}theap;

int filho_esquerda(int i){return 2*i+1;}
int filho_direita(int i){return 2*i+2;}

int pai(int i){
	if(i>0)
		return (i-1)/2;
	else
		return 0;
}

int troca(int *a, int *b){
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

void constroi_heap(theap *heap, int max){
	heap->v = malloc(sizeof(int) * max);
	heap->n = 0;
	heap->max = max;
}

void constroi_MAX_HEAP(theap *heap){
	for(int i = pai(heap->n-1); i>=0; i--)
		descer(heap, i);
}

void subir(theap *heap, int i){
	int _pai = pai(i);
	if((i > 0) && (i < heap->n) && (heap->v[_pai] < heap->v[i])){
		troca(&(heap->v[_pai]), &(heap->v[i]));
		subir(heap, _pai);
	}
}


void insere_heap(theap *heap, int n){
	if(heap->n < heap->max){
		heap->v[heap->n] = n;
		heap->n++;
		subir(heap, heap->n-1);
	}
}

int extrai_MAX(theap *heap){
	int max = heap->v[0];
	heap->v[0] = heap->v[heap->n-1];
	heap->n--;
	descer(heap, 0);
	return max;
}

void imprime_heap(theap *heap){
	for(int i = 0; i < heap->n; i++)
		printf("%d ", heap->v[i]);
}

void prioridade(theap *heap, int n, int i){
	heap->v[i] = n;   
	int pain = pai(i);
	if(heap->v[pain] < heap->v[i]){
		subir(heap, i);
	}else
		descer(heap, i);
}


int main(){
	int vetor[] = {1, 5, 7, 6, 8, 9, 10, 12, 13, 15, 11, 14};

	theap *heap = malloc(sizeof(theap));
	constroi_heap(heap, 16);
	heap->n = 12;

	for(int i = 0; i < heap->n; i++){
		heap->v[i] = vetor[i];
	}

	//exercicio a
	printf("\n\nMAX HEAP\n");
	constroi_MAX_HEAP(heap);
	for(int i = 0; i < heap->n; i++){
		printf("%d ",heap->v[i]);
	}

	//exercicio b
	insere_heap(heap, 20);
	insere_heap(heap, 3);
	insere_heap(heap, 18);

	printf("\n\nAPOS INSERCAO\n");
	//constroi_MAX_HEAP(heap);
	imprime_heap(heap);

	//exercicio c
	printf("\n");
	int extrai = extrai_MAX(heap);
	printf("MAX EXTRAIDO: %d\n", extrai);
	extrai = extrai_MAX(heap);
	printf("MAX EXTRAIDO: %d\n", extrai);
	extrai = extrai_MAX(heap);
	printf("MAX EXTRAIDO: %d\n", extrai);

	imprime_heap(heap);

	//exercicio d
	prioridade(heap, 2, 1);
	printf("\n");
	imprime_heap(heap);

	//exercicio e
	prioridade(heap, 100, 4);
	printf("\n");
	imprime_heap(heap);
}