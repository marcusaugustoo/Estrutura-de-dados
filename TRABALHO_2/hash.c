#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h> // Para medir o tempo e para srand

#define SEED 0x12345678
#define TAMANHO_TABELA_FIXA 6100
#define TAMANHO_INICIAL_PEQUENO 1000

// =================================================================================
// 1. ESTRUTURAS E TIPOS DE DADOS
// =================================================================================

// Enum para escolher a estratégia de tratamento de colisão
typedef enum {
    LINEAR_PROBING, // Sondagem Linear (Hash Simples)
    DOUBLE_HASHING  // Hash Duplo
} HashStrategy;

// Estrutura da Tabela Hash (modificada para o trabalho)
typedef struct {
    uintptr_t * table;        // A tabela de ponteiros
    int size;                 // Número de elementos atualmente na tabela
    int max;                  // Tamanho máximo da tabela (número de buckets)
    uintptr_t deleted;        // Marcador para um elemento removido (tombstone)
    char * (*get_key)(void *); // Ponteiro de função para obter a chave de um registro
    
    // -- Adições para o trabalho --
    HashStrategy strategy;    // Estratégia de sondagem (Linear ou Dupla)
    float max_load_factor;    // Taxa de ocupação máxima antes de redimensionar
} thash;

// Estrutura para armazenar os dados do CEP
typedef struct {
    char cep_prefix[6]; // Chave: 5 dígitos + '\0'
    char cidade[100];
    char estado[3];   // Ex: "SP" + '\0'
} tcep;


// =================================================================================
// 2. FUNÇÕES DE HASH
// =================================================================================

// Função de Hash Primária (MurmurHash)
uint32_t hashf1(const char* str, uint32_t h) {
    for (; *str; ++str) {
        h ^= *str;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}

// Função de Hash Secundária (para Hash Duplo)
uint32_t hashf2(const char* str, uint32_t h) {
    // A função djb2 é uma boa escolha, simples e eficaz
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return (uint32_t)hash;
}

// =================================================================================
// 3. FUNÇÕES DA TABELA HASH (com modificações)
// =================================================================================

// Protótipo da função de rehash para que a 'hash_insere' possa encontrá-la
int hash_rehash(thash *h);

// Constrói a tabela hash
int hash_constroi(thash *h, int nbuckets, char * (*get_key)(void *), HashStrategy strategy, float load_factor) {
    h->table = calloc(sizeof(uintptr_t), nbuckets);
    if (h->table == NULL) {
        return EXIT_FAILURE;
    }
    h->max = nbuckets;
    h->size = 0;
    h->deleted = (uintptr_t)&(h->size); // Um endereço único para usar como marcador
    h->get_key = get_key;
    
    // -- Inicializa os novos campos --
    h->strategy = strategy;
    h->max_load_factor = load_factor;

    return EXIT_SUCCESS;
}

// Insere um elemento na tabela hash
int hash_insere(thash *h, void *bucket) {
    // Passo 2: Verifica a taxa de ocupação e faz rehash se necessário
    if (h->max_load_factor > 0) { // Se for 0, o rehash está desativado
        float current_load = (float)h->size / h->max;
        if (current_load >= h->max_load_factor) {
            if (hash_rehash(h) != EXIT_SUCCESS) {
                free(bucket); // Não conseguiu inserir, libera a memória
                return EXIT_FAILURE;
            }
        }
    }

    // Se a tabela estiver cheia mesmo após a tentativa de rehash
    if (h->size == h->max) {
        free(bucket);
        return EXIT_FAILURE;
    }

    const char *key = h->get_key(bucket);
    uint32_t pos = hashf1(key, SEED) % h->max;
    uint32_t step = 1;

    // Passo 1: Define o 'step' com base na estratégia
    if (h->strategy == DOUBLE_HASHING) {
        step = 1 + (hashf2(key, SEED) % (h->max - 1));
    }

    // Procura por uma posição vazia ou deletada
    while (h->table[pos] != 0 && h->table[pos] != h->deleted) {
        pos = (pos + step) % h->max;
    }

    h->table[pos] = (uintptr_t)bucket;
    h->size++;
    return EXIT_SUCCESS;
}

// Busca um elemento na tabela
void *hash_busca(thash h, const char *key) {
    uint32_t pos = hashf1(key, SEED) % h.max;
    uint32_t original_pos = pos;
    uint32_t step = 1;

    if (h.strategy == DOUBLE_HASHING) {
        step = 1 + (hashf2(key, SEED) % (h.max - 1));
    }
    
    // Loop de busca
    while (h.table[pos] != 0) { // Para apenas se achar um bucket realmente vazio (0)
        // Se o bucket não está marcado como deletado, podemos comparar
        if (h.table[pos] != h.deleted) {
            if (strcmp(h.get_key((void *)h.table[pos]), key) == 0) {
                return (void *)h.table[pos];
            }
        }
        pos = (pos + step) % h.max;
        // Se deu a volta completa, o elemento não está na tabela
        if (pos == original_pos) {
            break;
        }
    }
    return NULL;
}

// Remove um elemento
int hash_remove(thash *h, const char *key) {
    uint32_t pos = hashf1(key, SEED) % h->max;
    uint32_t original_pos = pos;
    uint32_t step = 1;

    if (h->strategy == DOUBLE_HASHING) {
        step = 1 + (hashf2(key, SEED) % (h->max - 1));
    }

    while (h->table[pos] != 0) {
        if (h->table[pos] != h->deleted) {
            if (strcmp(h->get_key((void *)h->table[pos]), key) == 0) {
                free((void *)h->table[pos]);
                h->table[pos] = h->deleted; // Marca como deletado
                h->size--;
                return EXIT_SUCCESS;
            }
        }
        pos = (pos + step) % h->max;
        if (pos == original_pos) {
            break;
        }
    }
    return EXIT_FAILURE;
}

// Apaga a tabela hash e libera toda a memória
void hash_apaga(thash *h) {
    for (int i = 0; i < h->max; i++) {
        if (h->table[i] != 0 && h->table[i] != h->deleted) {
            free((void *)h->table[i]);
        }
    }
    free(h->table);
    h->size = 0;
    h->max = 0;
}

// Função de Redimensionamento (Rehashing)
int hash_rehash(thash *h) {
    printf("--- ATENCAO: Redimensionando tabela de %d para %d buckets ---\n", h->max, h->max * 2);

    int old_max = h->max;
    uintptr_t *old_table = h->table;

    int new_max = old_max * 2;
    h->table = calloc(sizeof(uintptr_t), new_max);
    if (!h->table) { // Falha de alocação
        h->table = old_table; // Restaura a tabela antiga
        return EXIT_FAILURE;
    }

    h->max = new_max;
    h->size = 0; // A inserção cuidará de incrementar

    // Reinsere todos os elementos da tabela antiga na nova
    for (int i = 0; i < old_max; i++) {
        if (old_table[i] != 0 && old_table[i] != h->deleted) {
            // A função hash_insere já usa a nova configuração de h->max e h->table
            hash_insere(h, (void *)old_table[i]);
        }
    }

    free(old_table);
    return EXIT_SUCCESS;
}


// =================================================================================
// 4. FUNÇÕES ESPECÍFICAS PARA O TIPO DE DADO (CEP)
// =================================================================================

// Função para obter a chave (prefixo do CEP) de um registro tcep
char * get_key_cep(void *reg) {
    return ((tcep *)reg)->cep_prefix;
}

// Função auxiliar para alocar e preencher uma struct tcep
tcep* aloca_cep(const char* cep_prefix, const char* cidade, const char* estado) {
    tcep *novo_cep = malloc(sizeof(tcep));
    if (novo_cep) {
        strcpy(novo_cep->cep_prefix, cep_prefix);
        strcpy(novo_cep->cidade, cidade);
        strcpy(novo_cep->estado, estado);
    }
    return novo_cep;
}

// Função para buscar um CEP e imprimir o resultado
void busca_e_imprime_cep(thash h, const char* cep_prefixo) {
    tcep* resultado = (tcep*) hash_busca(h, cep_prefixo);
    if (resultado) {
        printf("Busca por '%s': Encontrado -> Cidade: %s, Estado: %s\n", cep_prefixo, resultado->cidade, resultado->estado);
    } else {
        printf("Busca por '%s': Nao encontrado.\n", cep_prefixo);
    }
}

// =================================================================================
// 5. FUNÇÕES PARA OS EXPERIMENTOS (para profiling com gprof)
// =================================================================================

// -------- Experimento 4.1: Tempo de Busca vs Ocupação --------

// Esta função será chamada pelo gprof.
// Você criaria uma para cada taxa/estratégia. Ex: teste_busca_linear_50, teste_busca_duplo_50
void teste_busca_linear_50(thash h, tcep* ceps_para_buscar, int n_buscas) {
    for (int i = 0; i < n_buscas; i++) {
        hash_busca(h, ceps_para_buscar[i].cep_prefix);
    }
}

void teste_busca_duplo_50(thash h, tcep* ceps_para_buscar, int n_buscas) {
    for (int i = 0; i < n_buscas; i++) {
        hash_busca(h, ceps_para_buscar[i].cep_prefix);
    }
}


// -------- Experimento 4.2: Overhead de Inserção --------

// Função que insere todos os CEPs numa tabela pequena, forçando rehash
void teste_insercao_com_rehash(tcep* todos_os_ceps, int total_ceps) {
    thash h;
    // Tabela pequena com load factor de 70% para forçar vários rehashes
    hash_constroi(&h, TAMANHO_INICIAL_PEQUENO, get_key_cep, LINEAR_PROBING, 0.7f);
    
    for (int i = 0; i < total_ceps; i++) {
        // Para o teste, não precisamos guardar os dados, apenas medir a inserção
        tcep* novo = aloca_cep(todos_os_ceps[i].cep_prefix, todos_os_ceps[i].cidade, todos_os_ceps[i].estado);
        hash_insere(&h, novo);
    }
    hash_apaga(&h);
}

// Função que insere todos os CEPs numa tabela já grande, sem rehash
void teste_insercao_sem_rehash(tcep* todos_os_ceps, int total_ceps) {
    thash h;
    // Tabela já grande, o load factor não será atingido
    hash_constroi(&h, TAMANHO_TABELA_FIXA, get_key_cep, LINEAR_PROBING, 0.99f);

    for (int i = 0; i < total_ceps; i++) {
        tcep* novo = aloca_cep(todos_os_ceps[i].cep_prefix, todos_os_ceps[i].cidade, todos_os_ceps[i].estado);
        hash_insere(&h, novo);
    }
    hash_apaga(&h);
}


// =================================================================================
// 6. FUNÇÃO PRINCIPAL
// =================================================================================

int main(int argc, char* argv[]) {
    // Para o trabalho real, você deve carregar os dados de um arquivo.
    // Aqui, criamos um conjunto de dados de exemplo.
    const int TOTAL_CEPS_EXEMPLO = 5000;
    tcep *banco_de_ceps = malloc(sizeof(tcep) * TOTAL_CEPS_EXEMPLO);
    for(int i = 0; i < TOTAL_CEPS_EXEMPLO; i++){
        char cep_str[6], cidade_str[20], estado_str[3];
        sprintf(cep_str, "%05d", i); // Gera CEPs "00000", "00001", etc.
        sprintf(cidade_str, "Cidade_%d", i);
        strcpy(estado_str, "BR");
        strcpy(banco_de_ceps[i].cep_prefix, cep_str);
        strcpy(banco_de_ceps[i].cidade, cidade_str);
        strcpy(banco_de_ceps[i].estado, estado_str);
    }

    printf("=========================================================\n");
    printf("INICIANDO EXPERIMENTO 4.1: BUSCA vs OCUPACAO (Exemplo 50%%)\n");
    printf("=========================================================\n");

    thash h_linear, h_duplo;
    int n_elementos_50 = TAMANHO_TABELA_FIXA * 0.50;

    // Prepara tabelas com 50% de ocupação
    hash_constroi(&h_linear, TAMANHO_TABELA_FIXA, get_key_cep, LINEAR_PROBING, 0.0f); // 0.0f desativa rehash
    hash_constroi(&h_duplo, TAMANHO_TABELA_FIXA, get_key_cep, DOUBLE_HASHING, 0.0f);
    
    for (int i = 0; i < n_elementos_50; i++) {
        hash_insere(&h_linear, aloca_cep(banco_de_ceps[i].cep_prefix, banco_de_ceps[i].cidade, banco_de_ceps[i].estado));
        hash_insere(&h_duplo, aloca_cep(banco_de_ceps[i].cep_prefix, banco_de_ceps[i].cidade, banco_de_ceps[i].estado));
    }
    
    printf("Tabelas criadas com %d elementos (50%% de ocupacao de %d).\n", n_elementos_50, TAMANHO_TABELA_FIXA);
    printf("Executando funcoes de teste para gprof...\n");

    // Estas são as funções que o gprof irá medir
    teste_busca_linear_50(h_linear, banco_de_ceps, n_elementos_50);
    teste_busca_duplo_50(h_duplo, banco_de_ceps, n_elementos_50);

    printf("Buscas para 50%% de ocupacao concluidas.\n");

    // Libera a memória do experimento 4.1
    hash_apaga(&h_linear);
    hash_apaga(&h_duplo);


    printf("\n=========================================================\n");
    printf("INICIANDO EXPERIMENTO 4.2: OVERHEAD DE INSERCAO\n");
    printf("=========================================================\n");
    
    // Estas são as funções que o gprof irá medir
    printf("Executando insercao em tabela grande (sem rehash)...\n");
    teste_insercao_sem_rehash(banco_de_ceps, TOTAL_CEPS_EXEMPLO);
    printf("Concluido.\n");

    printf("Executando insercao em tabela pequena (com rehash)...\n");
    teste_insercao_com_rehash(banco_de_ceps, TOTAL_CEPS_EXEMPLO);
    printf("Concluido.\n");

    free(banco_de_ceps);
    printf("\nExperimentos concluidos. Compile com -pg e use gprof para analisar.\n");

    return 0;
}