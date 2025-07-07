// hash.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

static unsigned int hash_function1(const char* key, int table_size) {
    unsigned long hash = 0;
    while (*key) {
        hash = hash * 31 + *key++;
    }
    return hash % table_size;
}

static unsigned int hash_function2(const char* key, int table_size) {
    unsigned long hash = 0;
    while (*key) {
        hash = hash * 37 + *key++;
    }
    return (hash % (table_size - 1)) + 1;
}

static void redimensionar_hash_table(HASHTABLE* ht, TIPO_HASH tipo) {
    int old_cap = ht->capacidade;
    HASH_ITEM* old_tab = ht->tabela;

    ht->capacidade *= 2;
    ht->num_elementos = 0;
    ht->tabela = calloc(ht->capacidade, sizeof(HASH_ITEM));

    if (!ht->tabela) {
        perror("Erro ao redimensionar a tabela");
        ht->tabela = old_tab;
        ht->capacidade = old_cap;
        return;
    }

    for (int i = 0; i < old_cap; i++) {
        if (old_tab[i].ocupado) {
            inserir_hash(ht, old_tab[i].cep_inicial, old_tab[i].cidade, old_tab[i].estado, tipo);
        }
    }
    free(old_tab);
}

HASHTABLE* criar_hash_table(int capacidade_inicial, float taxa_ocupacao_limite) {
    HASHTABLE* ht = malloc(sizeof(HASHTABLE));
    if (!ht) return NULL;

    ht->capacidade = capacidade_inicial;
    ht->num_elementos = 0;
    ht->taxa_ocupacao_limite = taxa_ocupacao_limite;
    ht->tabela = calloc(capacidade_inicial, sizeof(HASH_ITEM));

    return ht;
}

void liberar_hash_table(HASHTABLE* ht) {
    if (ht) {
        free(ht->tabela);
        free(ht);
    }
}

void inserir_hash(HASHTABLE* ht, const char* cep, const char* cidade, const char* estado, TIPO_HASH tipo) {
    if ((float)(ht->num_elementos + 1) / ht->capacidade > ht->taxa_ocupacao_limite) {
        redimensionar_hash_table(ht, tipo);
    }

    unsigned int index = hash_function1(cep, ht->capacidade);
    unsigned int step = (tipo == HASH_DUPLO) ? hash_function2(cep, ht->capacidade) : 1;

    for (int i = 0; i < ht->capacidade; i++) {
        int idx = (index + i * step) % ht->capacidade;
        if (!ht->tabela[idx].ocupado) {
            strncpy(ht->tabela[idx].cep_inicial, cep, 5);
            ht->tabela[idx].cep_inicial[5] = '\0';
            strncpy(ht->tabela[idx].cidade, cidade, MAX_CIDADE_LEN - 1);
            ht->tabela[idx].cidade[MAX_CIDADE_LEN - 1] = '\0';
            strncpy(ht->tabela[idx].estado, estado, MAX_ESTADO_LEN - 1);
            ht->tabela[idx].estado[MAX_ESTADO_LEN - 1] = '\0';
            ht->tabela[idx].ocupado = true;
            ht->tabela[idx].removido = false;
            ht->num_elementos++;
            return;
        }
    }
    fprintf(stderr, "Erro: tabela cheia, nao foi possivel inserir %s\n", cep);
}

HASH_ITEM* buscar_hash(HASHTABLE* ht, const char* cep, TIPO_HASH tipo) {
    unsigned int index = hash_function1(cep, ht->capacidade);
    unsigned int step = (tipo == HASH_DUPLO) ? hash_function2(cep, ht->capacidade) : 1;

    for (int i = 0; i < ht->capacidade; i++) {
        int idx = (index + i * step) % ht->capacidade;
        HASH_ITEM* item = &ht->tabela[idx];

        if (!item->ocupado && !item->removido)
            return NULL;

        if (item->ocupado && strncmp(item->cep_inicial, cep, 5) == 0)
            return item;
    }
    return NULL;
}

float calcular_taxa_ocupacao(HASHTABLE* ht) {
    return (float)ht->num_elementos / ht->capacidade;
}
