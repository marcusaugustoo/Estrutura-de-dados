#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>

#define EMBEDDING_SIZE 128
#define ID_SIZE 100

typedef struct _face {
    float embedding[EMBEDDING_SIZE];
    char id[ID_SIZE];
} tface;

typedef struct _node {
    void *key;
    struct _node *esq;
    struct _node *dir;
}tnode;

typedef struct _arv {
    tnode *raiz;
    int k;
    int (*cmp)(void*, void*, int);
    double (*dist)(void*, void*);
} tarv;

// ======================= ALOCAÇÃO E COMPARAÇÃO =======================

void *aloca_face(float embedding[], char id[]) {
    tface *f = malloc(sizeof(tface));
    memcpy(f->embedding, embedding, sizeof(float) * EMBEDDING_SIZE);
    strcpy(f->id,id);
    return f;
}

int comparador_face(void *a, void *b, int pos) {
    float va = ((tface *)a)->embedding[pos];
    float vb = ((tface *)b)->embedding[pos];

    if (va < vb) {
        return -1;
    } else if (va > vb) {
        return 1;
    } else {
        return 0;
    }
}

double distancia_face(void *a, void *b) {
    float *ea = ((tface *)a)->embedding;
    float *eb = ((tface *)b)->embedding;
    double soma = 0;
    for (int i = 0; i < EMBEDDING_SIZE; i++) {
        double diff = ea[i] - eb[i];
        soma += diff * diff;
    }
    return soma;
}

// ======================= CONSTRUÇÃO DA ÁRVORE =======================

void kdtree_constroi(tarv *arv, int (*cmp)(void *, void *, int), double (*dist)(void *, void *), int k) {
    arv->raiz = NULL;
    arv->cmp = cmp;
    arv->dist = dist;
    arv->k = k;
}

void _kdtree_insere(tnode **raiz, void *key, int (*cmp)(void *, void *, int), int profund, int k) {
    if (*raiz == NULL) {
        *raiz = malloc(sizeof(tnode));
        (*raiz)->key = key;
        (*raiz)->esq = NULL;
        (*raiz)->dir = NULL;
    } else {
        int pos = profund % k;
        if (cmp(key, (*raiz)->key, pos) < 0) {
            _kdtree_insere(&((*raiz)->esq), key, cmp, profund + 1, k);
        } else {
            _kdtree_insere(&((*raiz)->dir), key, cmp, profund + 1, k);
        }
    }
}

void kdtree_insere(tarv *arv, void *key) {
    _kdtree_insere(&(arv->raiz), key, arv->cmp, 0, arv->k);
}

// ======================= BUSCA N VIZINHOS COM HEAP =======================

typedef struct {
    double dist;
    void *key;
} heap_item;

typedef struct {
    heap_item *dados;
    int tamanho;
    int capacidade;
} heap;

void heap_init(heap *h, int capacidade) {
    h->dados = malloc(sizeof(heap_item) * capacidade);
    h->tamanho = 0;
    h->capacidade = capacidade;
}

void heap_insere(heap *h, void *key, double dist) {
    if (h->tamanho < h->capacidade) {
        h->dados[h->tamanho].key = key;
        h->dados[h->tamanho].dist = dist;
        h->tamanho++;
    } else if (dist < h->dados[0].dist) {
        h->dados[0].key = key;
        h->dados[0].dist = dist;
    }

    // Max-heap parcial (bubble down)
    for (int i = h->tamanho / 2 - 1; i >= 0; i--) {
        int maior = i;
        int esq = 2 * i + 1;
        int dir = 2 * i + 2;

        if (esq < h->tamanho && h->dados[esq].dist > h->dados[maior].dist)
            maior = esq;
        if (dir < h->tamanho && h->dados[dir].dist > h->dados[maior].dist)
            maior = dir;

        if (maior != i) {
            heap_item tmp = h->dados[i];
            h->dados[i] = h->dados[maior];
            h->dados[maior] = tmp;
        }
    }
}

void _kdtree_busca_n(tarv *arv, tnode *no, void *query, int profund, heap *h) {
    if (no == NULL) return;

    double dist = arv->dist(query, no->key);
    heap_insere(h, no->key, dist);

    int pos = profund % arv->k;
    int comp = arv->cmp(query, no->key, pos);

    if (comp < 0) {
        _kdtree_busca_n(arv, no->esq, query, profund + 1, h);
        _kdtree_busca_n(arv, no->dir, query, profund + 1, h);  // opcional: verificar split
    } else {
        _kdtree_busca_n(arv, no->dir, query, profund + 1, h);
        _kdtree_busca_n(arv, no->esq, query, profund + 1, h);  // opcional
    }
}

void kdtree_busca_n_vizinhos(tarv *arv, void *query, int n, heap *h) {
    heap_init(h, n);
    _kdtree_busca_n(arv, arv->raiz, query, 0, h);
}

// ======================= TESTES =======================

void teste() {
    tarv arv;
    kdtree_constroi(&arv, comparador_face, distancia_face, EMBEDDING_SIZE);

    float v1[EMBEDDING_SIZE] = {0};
    float v2[EMBEDDING_SIZE] = {1};
    float v3[EMBEDDING_SIZE] = {2};
    float v4[EMBEDDING_SIZE] = {3};
    float v5[EMBEDDING_SIZE] = {4};

    kdtree_insere(&arv, aloca_face(v1, "Pessoa_0"));
    kdtree_insere(&arv, aloca_face(v2, "Pessoa_1"));
    kdtree_insere(&arv, aloca_face(v3, "Pessoa_2"));
    kdtree_insere(&arv, aloca_face(v4, "Pessoa_3"));
    kdtree_insere(&arv, aloca_face(v5, "Pessoa_4"));

    float consulta[EMBEDDING_SIZE] = {1.5}; // próximo de Pessoa_1 e Pessoa_2
    void *query = aloca_face(consulta, "consulta");

    heap h;
    kdtree_busca_n_vizinhos(&arv, query, 3, &h);

    printf("3 vizinhos mais próximos:\n");
    for (int i = 0; i < h.tamanho; i++) {
        tface *f = (tface *)h.dados[i].key;
        printf("ID: %s, Distância: %lf\n", f->id, h.dados[i].dist);
    }

    free(query);
    free(h.dados);
}

int main() {
    teste();
    return 0;
}
