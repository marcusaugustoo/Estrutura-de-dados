#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>

#define EMBEDDING_SIZE 128
#define ID_SIZE 100

typedef struct _face{
    float embedding[EMBEDDING_SIZE];
    char id[ID_SIZE];
}tface;

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
}tarv;

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

    if(va < vb) 
        return -1;
    else if (va > vb) 
        return 1;
    else
        return 0;
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
    if(*raiz == NULL) {
        *raiz = malloc(sizeof(tnode));
        (*raiz)->key = key;
        (*raiz)->esq = NULL;
        (*raiz)->dir = NULL;
    } 
    else{
        int pos = profund % k;
        if(cmp(key, (*raiz)->key, pos) < 0){
            _kdtree_insere(&((*raiz)->esq), key, cmp, profund + 1, k);
        } 
        else{
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
}heap_item;

typedef struct {
    heap_item *dados;
    int tamanho;
    int capacidade;
}heap;

void heap_init(heap *h, int capacidade) {
    h->dados = malloc(sizeof(heap_item) * capacidade);
    h->tamanho = 0;
    h->capacidade = capacidade;
}

void heap_insere(heap *h, void *key, double dist) {
    if(h->tamanho < h->capacidade){
        h->dados[h->tamanho].key = key;
        h->dados[h->tamanho].dist = dist;
        h->tamanho++;
        
        // Subir com o novo elemento
        int i = h->tamanho - 1;
        while (i > 0 && h->dados[(i-1)/2].dist < h->dados[i].dist) {
            heap_item tmp = h->dados[(i-1)/2];
            h->dados[(i-1)/2] = h->dados[i];
            h->dados[i] = tmp;
            i = (i-1)/2;
        }
    } 
    else if(dist < h->dados[0].dist){
        h->dados[0].key = key;
        h->dados[0].dist = dist;
        
        // Descer com a raiz
        int i = 0;
        while (1) {
            int maior = i;
            int esq = 2*i + 1;
            int dir = 2*i + 2;
            
            if(esq < h->tamanho && h->dados[esq].dist > h->dados[maior].dist)
                maior = esq;
            if(dir < h->tamanho && h->dados[dir].dist > h->dados[maior].dist)
                maior = dir;
                
            if(maior == i) break;
            
            heap_item tmp = h->dados[i];
            h->dados[i] = h->dados[maior];
            h->dados[maior] = tmp;
            i = maior;
        }
    }
}

void _kdtree_busca_n(tarv *arv, tnode *no, void *query, int profund, heap *h) {
    if(no == NULL) return;

    double dist = arv->dist(query, no->key);
    heap_insere(h, no->key, dist);

    int pos = profund % arv->k;
    int comp = arv->cmp(query, no->key, pos);

    // Visitar primeiro o lado provável onde está o vizinho mais próximo
    if(comp < 0) {
        _kdtree_busca_n(arv, no->esq, query, profund + 1, h);
    }else {
        _kdtree_busca_n(arv, no->dir, query, profund + 1, h);
    }

    // Verificar se precisa visitar o outro lado 
    float diff = fabs(((tface *)query)->embedding[pos] - ((tface *)no->key)->embedding[pos]);
    if(h->tamanho < h->capacidade || diff < h->dados[0].dist){
        if(comp < 0){
            _kdtree_busca_n(arv, no->dir, query, profund + 1, h);
        } 
        else{
            _kdtree_busca_n(arv, no->esq, query, profund + 1, h);
        }
    }
}


void kdtree_busca_n_vizinhos(tarv *arv, void *query, int n, heap *h) {
    heap_init(h, n);
    _kdtree_busca_n(arv, arv->raiz, query, 0, h);
}

// Libera memória
void libera_no(tnode *no) {
    if(no){
        libera_no(no->esq);
        libera_no(no->dir);
        free(no->key);
        free(no);
    }
}

void kdtree_libera(tarv *arv) {
    libera_no(arv->raiz);
    arv->raiz = NULL;
}

void carregar_faces_de_arquivo(tarv *arv, const char *nome_arquivo) {
    FILE *fp = fopen(nome_arquivo, "r");
    if(!fp)
        perror("Erro ao abrir o arquivo");

    char id[ID_SIZE];
    float emb[EMBEDDING_SIZE];

    while(fscanf(fp, "%s", id) == 1) {
        for(int i = 0; i < EMBEDDING_SIZE; i++) {
            if(fscanf(fp, "%f", &emb[i]) != 1){
                fprintf(stderr, "Erro ao ler embedding\n");
            }
        }
        void *face = aloca_face(emb, id);
        kdtree_insere(arv, face);
    }

    fclose(fp);
}

void* carregar_query(const char *nome_arquivo) {
    FILE *fp = fopen(nome_arquivo, "r");
    if (!fp)
        perror("Erro ao abrir o arquivo da query");

    char id[ID_SIZE];
    float emb[EMBEDDING_SIZE];

    fscanf(fp, "%s", id);
    for (int i = 0; i < EMBEDDING_SIZE; i++) {
        fscanf(fp, "%f", &emb[i]);
    }

    fclose(fp);
    return aloca_face(emb, id);
}


int main() {

    tarv arv;
    kdtree_constroi(&arv, comparador_face, distancia_face, EMBEDDING_SIZE);

    carregar_faces_de_arquivo(&arv, "F:\\ESTRUTURA_DE_DADOS\\trabalho_1\\embeddings.txt");

    void *query = carregar_query("F:\\ESTRUTURA_DE_DADOS\\trabalho_1\\query.txt");

    heap h;
    kdtree_busca_n_vizinhos(&arv, query, 1, &h);


    // Imprime os vizinhos
    int N = 5;
    kdtree_busca_n_vizinhos(&arv, query, N, &h);
    printf("Vizinhos mais proximos:\n");
    for (int i = 0; i < h.tamanho; i++) {
        tface *vizinho = (tface *)h.dados[i].key;
        printf("ID: %s | Distancia: %.4f\n", vizinho->id, h.dados[i].dist);
    }

    free(h.dados);
    kdtree_libera(&arv);
    free(query);
    return 0;
}
