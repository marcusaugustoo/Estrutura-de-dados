#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

#define LINHA_MAX 256

void carregar_ceps(const char* arquivo, HASHTABLE* ht, TIPO_HASH tipo, int limite) {
    FILE* f = fopen(arquivo, "r");
    if (!f) {
        perror("Erro ao abrir CSV");
        exit(EXIT_FAILURE);
    }

    char linha[LINHA_MAX];
    int inseridos = 0;

    while (fgets(linha, sizeof(linha), f)) {
        char cep[6], cidade[MAX_CIDADE_LEN], estado[MAX_ESTADO_LEN];
        if (sscanf(linha, "%5[^;];%99[^;];%2s", cep, cidade, estado) == 3) {
            inserir_hash(ht, cep, cidade, estado, tipo);
            inseridos++;
            if (limite > 0 && inseridos >= limite) break;
        }
    }

    fclose(f);
}


void busca_taxa(float taxa, TIPO_HASH tipo) {
    int capacidade = 6100;
    int limite = (int)(capacidade * taxa);

    HASHTABLE* ht = criar_hash_table(capacidade, taxa);
    carregar_ceps("ceps.csv", ht, tipo, limite);

    for (int i = 0; i < 50000000; i++) {
        buscar_hash(ht, "12345", tipo);
    }

    liberar_hash_table(ht);
}

void busca10() { busca_taxa(0.10, HASH_SIMPLES); }
void busca20() { busca_taxa(0.20, HASH_SIMPLES); }
void busca30() { busca_taxa(0.30, HASH_SIMPLES); }
void busca40() { busca_taxa(0.40, HASH_SIMPLES); }
void busca50() { busca_taxa(0.50, HASH_SIMPLES); }
void busca60() { busca_taxa(0.60, HASH_SIMPLES); }
void busca70() { busca_taxa(0.70, HASH_SIMPLES); }
void busca80() { busca_taxa(0.80, HASH_SIMPLES); }
void busca90() { busca_taxa(0.90, HASH_SIMPLES); }
void busca99() { busca_taxa(0.99, HASH_SIMPLES); }

void busca10_duplo() { busca_taxa(0.10, HASH_DUPLO); }
void busca20_duplo() { busca_taxa(0.20, HASH_DUPLO); }
void busca30_duplo() { busca_taxa(0.30, HASH_DUPLO); }
void busca40_duplo() { busca_taxa(0.40, HASH_DUPLO); }
void busca50_duplo() { busca_taxa(0.50, HASH_DUPLO); }
void busca60_duplo() { busca_taxa(0.60, HASH_DUPLO); }
void busca70_duplo() { busca_taxa(0.70, HASH_DUPLO); }
void busca80_duplo() { busca_taxa(0.80, HASH_DUPLO); }
void busca90_duplo() { busca_taxa(0.90, HASH_DUPLO); }
void busca99_duplo() { busca_taxa(0.99, HASH_DUPLO); }


void insere6100() {
    HASHTABLE* ht = criar_hash_table(6100, 0.9); 
    carregar_ceps("ceps.csv", ht, HASH_SIMPLES, -1);
    liberar_hash_table(ht);
}

void insere1000() {
    HASHTABLE* ht = criar_hash_table(1000, 0.7); 
    carregar_ceps("ceps.csv", ht, HASH_SIMPLES, -1);
    liberar_hash_table(ht);
}

int main() {
    printf("Executando testes de busca e insercao\n");

    //busca10();
    //busca20();
    //busca30();
    //busca40();
    //busca50();
    //busca60();
    //busca70();
    //busca80();
    //busca90();
    busca99();

    //busca10_duplo();
    //busca20_duplo();
    //busca30_duplo();
    //busca40_duplo();
    //busca50_duplo();
    //busca60_duplo();
    //busca70_duplo();
    //busca80_duplo();
    //busca90_duplo();
    busca99_duplo();

    insere6100();
    insere1000();

    printf("Testes Finalizados!\n");
    return 0;
}
