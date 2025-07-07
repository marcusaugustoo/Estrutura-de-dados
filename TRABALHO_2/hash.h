#ifndef HASH_H
#define HASH_H

#include <stdbool.h>

#define MAX_CEP_LEN 6          
#define MAX_CIDADE_LEN 100
#define MAX_ESTADO_LEN 3       

typedef enum {
    HASH_SIMPLES,
    HASH_DUPLO
} TIPO_HASH;

typedef struct {
    char cep_inicial[MAX_CEP_LEN];
    char cidade[MAX_CIDADE_LEN];
    char estado[MAX_ESTADO_LEN];
    bool ocupado;
    bool removido;
} HASH_ITEM;

typedef struct {
    HASH_ITEM* tabela;
    int capacidade;
    int num_elementos;
    float taxa_ocupacao_limite;
} HASHTABLE;

//Cria uma nova tabela hash
HASHTABLE* criar_hash_table(int capacidade_inicial, float taxa_ocupacao_limite);

//Libera a memória da tabela hash
void liberar_hash_table(HASHTABLE* ht);

//Insere um item na tabela (com hash simples ou duplo)
void inserir_hash(HASHTABLE* ht, const char* cep, const char* cidade, const char* estado, TIPO_HASH tipo);

//Busca um item na tabela pelo cep
HASH_ITEM* buscar_hash(HASHTABLE* ht, const char* cep, TIPO_HASH tipo);

//Calcula a taxa de ocupação da tabela
float calcular_taxa_ocupacao(HASHTABLE* ht);

#endif
