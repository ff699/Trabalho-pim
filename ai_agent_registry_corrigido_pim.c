#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#define MAX_AGENTES 50
#define MAX_CATEGORIAS 50

#define TAM_NOME_AGENTE 81
#define TAM_SCRIPT 1001
#define TAM_NOME_CATEGORIA 41
#define TAM_DESCRICAO 201

/* Vetores paralelos: cada indice representa um registro. */
/* Categorias primeiro: cada agente pertence a uma categoria. */
int ids_categorias[MAX_CATEGORIAS];
char nomes_categorias[MAX_CATEGORIAS][TAM_NOME_CATEGORIA];
char descricoes_categorias[MAX_CATEGORIAS][TAM_DESCRICAO];
int total_categorias = 0;

/* Agentes: cada um guarda o ID da categoria a que pertence. */
int ids[MAX_AGENTES];
char nomes[MAX_AGENTES][TAM_NOME_AGENTE];
char scripts[MAX_AGENTES][TAM_SCRIPT];
int notas[MAX_AGENTES];
int id_categorias_agentes[MAX_AGENTES];
int total_agentes = 0;

/* Remove espacos no inicio e no fim do texto. */
void remover_espacos(char texto[]) {
    size_t inicio = 0;
    size_t fim = strlen(texto);

    while (texto[inicio] != '\0' && isspace((unsigned char)texto[inicio])) {
        inicio++;
    }

    while (fim > inicio && isspace((unsigned char)texto[fim - 1])) {
        fim--;
    }

    if (inicio > 0) {
        memmove(texto, texto + inicio, fim - inicio);
    }

    texto[fim - inicio] = '\0';
}

/* Le uma linha e permite texto vazio apenas quando solicitado. */
void ler_texto(const char *mensagem, char texto[], int tamanho, int obrigatorio) {
    int caractere;
    size_t comprimento;

    while (1) {
        printf("%s", mensagem);

        if (fgets(texto, tamanho, stdin) == NULL) {
            /* Fim da entrada (EOF): encerra para nao entrar em loop infinito.
               Os dados ja foram gravados a cada operacao concluida. */
            printf("\nEntrada encerrada. Saindo.\n");
            exit(0);
        }

        comprimento = strlen(texto);

        if (comprimento > 0 && texto[comprimento - 1] == '\n') {
            texto[comprimento - 1] = '\0';
        } else if (!feof(stdin)) {
            /* O buffer encheu sem encontrar '\n'. Se o proximo caractere for
               o fim da linha, o texto cabe exatamente e e valido. */
            caractere = getchar();

            if (caractere != '\n' && caractere != EOF) {
                while ((caractere = getchar()) != '\n' && caractere != EOF) {
                    /* Descarta o restante da linha. */
                }
                printf("Texto muito longo (maximo de %d caracteres). Tente novamente.\n",
                       tamanho - 1);
                continue;
            }
        }

        remover_espacos(texto);

        if (obrigatorio && texto[0] == '\0') {
            printf("Este campo nao pode ficar vazio. Tente novamente.\n");
            continue;
        }

        return;
    }
}

/* Le um numero inteiro, rejeitando entradas invalidas e valores fora do tipo int. */
int ler_inteiro(const char *mensagem) {
    char entrada[100];
    char *fim;
    long valor;

    while (1) {
        ler_texto(mensagem, entrada, sizeof(entrada), 1);
        errno = 0;
        valor = strtol(entrada, &fim, 10);

        while (*fim != '\0' && isspace((unsigned char)*fim)) {
            fim++;
        }

        if (entrada[0] != '\0' && *fim == '\0' &&
            errno != ERANGE && valor >= INT_MIN && valor <= INT_MAX) {
            return (int)valor;
        }

        printf("Entrada invalida. Digite um numero inteiro valido.\n");
    }
}

int localizar_id_agente(int id) {
    int i;
    for (i = 0; i < total_agentes; i++) {
        if (ids[i] == id) return i;
    }
    return -1;
}

int localizar_id_categoria(int id) {
    int i;
    for (i = 0; i < total_categorias; i++) {
        if (ids_categorias[i] == id) return i;
    }
    return -1;
}

int localizar_nome_categoria(const char *nome) {
    int i;
    for (i = 0; i < total_categorias; i++) {
        if (strcmp(nomes_categorias[i], nome) == 0) return i;
    }
    return -1;
}

int proximo_id_categoria(void) {
    int i;
    int maior = 0;

    for (i = 0; i < total_categorias; i++) {
        if (ids_categorias[i] > maior) maior = ids_categorias[i];
    }

    if (maior == INT_MAX) return -1;
    return maior + 1;
}

int proximo_id_agente(void) {
    int i;
    int maior = 0;

    for (i = 0; i < total_agentes; i++) {
        if (ids[i] > maior) maior = ids[i];
    }

    if (maior == INT_MAX) return -1;
    return maior + 1;
}

int categoria_tem_agentes(int id_categoria) {
    int i;
    for (i = 0; i < total_agentes; i++) {
        if (id_categorias_agentes[i] == id_categoria) return 1;
    }
    return 0;
}

void salvar_categorias(void) {
    FILE *arquivo = fopen("categorias.dat", "wb");
    int i;

    if (arquivo == NULL) {
        printf("Erro ao salvar categorias.\n");
        return;
    }

    if (fwrite(&total_categorias, sizeof(int), 1, arquivo) != 1) {
        printf("Erro ao gravar categorias.\n");
        fclose(arquivo);
        return;
    }

    for (i = 0; i < total_categorias; i++) {
        if (fwrite(&ids_categorias[i], sizeof(int), 1, arquivo) != 1 ||
            fwrite(nomes_categorias[i], sizeof(char), TAM_NOME_CATEGORIA, arquivo) != TAM_NOME_CATEGORIA ||
            fwrite(descricoes_categorias[i], sizeof(char), TAM_DESCRICAO, arquivo) != TAM_DESCRICAO) {
            printf("Erro ao gravar categorias.\n");
            fclose(arquivo);
            return;
        }
    }

    fclose(arquivo);
}

void salvar_agentes(void) {
    FILE *arquivo = fopen("agentes_v2.dat", "wb");
    int i;

    if (arquivo == NULL) {
        printf("Erro ao salvar agentes.\n");
        return;
    }

    if (fwrite(&total_agentes, sizeof(int), 1, arquivo) != 1) {
        printf("Erro ao gravar agentes.\n");
        fclose(arquivo);
        return;
    }

    for (i = 0; i < total_agentes; i++) {
        if (fwrite(&ids[i], sizeof(int), 1, arquivo) != 1 ||
            fwrite(nomes[i], sizeof(char), TAM_NOME_AGENTE, arquivo) != TAM_NOME_AGENTE ||
            fwrite(scripts[i], sizeof(char), TAM_SCRIPT, arquivo) != TAM_SCRIPT ||
            fwrite(&notas[i], sizeof(int), 1, arquivo) != 1 ||
            fwrite(&id_categorias_agentes[i], sizeof(int), 1, arquivo) != 1) {
            printf("Erro ao gravar agentes.\n");
            fclose(arquivo);
            return;
        }
    }

    fclose(arquivo);
}

void carregar_categorias(void) {
    FILE *arquivo = fopen("categorias.dat", "rb");
    int quantidade;
    int i;

    if (arquivo == NULL) return;

    if (fread(&quantidade, sizeof(int), 1, arquivo) != 1 ||
        quantidade < 0 || quantidade > MAX_CATEGORIAS) {
        printf("Arquivo de categorias invalido. Iniciando sem categorias.\n");
        fclose(arquivo);
        return;
    }

    for (i = 0; i < quantidade; i++) {
        if (fread(&ids_categorias[i], sizeof(int), 1, arquivo) != 1 ||
            fread(nomes_categorias[i], sizeof(char), TAM_NOME_CATEGORIA, arquivo) != TAM_NOME_CATEGORIA ||
            fread(descricoes_categorias[i], sizeof(char), TAM_DESCRICAO, arquivo) != TAM_DESCRICAO) {
            printf("Arquivo de categorias incompleto. Iniciando sem categorias.\n");
            total_categorias = 0;
            fclose(arquivo);
            return;
        }

        nomes_categorias[i][TAM_NOME_CATEGORIA - 1] = '\0';
        descricoes_categorias[i][TAM_DESCRICAO - 1] = '\0';
        total_categorias++;
    }

    fclose(arquivo);
}

void carregar_agentes(void) {
    FILE *arquivo = fopen("agentes_v2.dat", "rb");
    int quantidade;
    int i;

    if (arquivo == NULL) return;

    if (fread(&quantidade, sizeof(int), 1, arquivo) != 1 ||
        quantidade < 0 || quantidade > MAX_AGENTES) {
        printf("Arquivo de agentes invalido. Iniciando sem agentes.\n");
        fclose(arquivo);
        return;
    }

    for (i = 0; i < quantidade; i++) {
        if (fread(&ids[i], sizeof(int), 1, arquivo) != 1 ||
            fread(nomes[i], sizeof(char), TAM_NOME_AGENTE, arquivo) != TAM_NOME_AGENTE ||
            fread(scripts[i], sizeof(char), TAM_SCRIPT, arquivo) != TAM_SCRIPT ||
            fread(&notas[i], sizeof(int), 1, arquivo) != 1 ||
            fread(&id_categorias_agentes[i], sizeof(int), 1, arquivo) != 1) {
            printf("Arquivo de agentes incompleto. Iniciando sem agentes.\n");
            total_agentes = 0;
            fclose(arquivo);
            return;
        }

        nomes[i][TAM_NOME_AGENTE - 1] = '\0';
        scripts[i][TAM_SCRIPT - 1] = '\0';

        if (ids[i] <= 0 || notas[i] < 0 || notas[i] > 5 ||
            localizar_id_categoria(id_categorias_agentes[i]) == -1) {
            printf("Um registro invalido foi encontrado. Os agentes nao foram carregados.\n");
            total_agentes = 0;
            fclose(arquivo);
            return;
        }

        total_agentes++;
    }

    fclose(arquivo);
}

void listar_categorias(void) {
    int i;

    if (total_categorias == 0) {
        printf("Nenhuma categoria cadastrada.\n");
        return;
    }

    printf("\n===== CATEGORIAS =====\n");
    for (i = 0; i < total_categorias; i++) {
        printf("\nID: %d\n", ids_categorias[i]);
        printf("Nome: %s\n", nomes_categorias[i]);
        printf("Descricao: %s\n", descricoes_categorias[i]);
    }
}

void cadastrar_categoria(void) {
    int novo_id;

    if (total_categorias >= MAX_CATEGORIAS) {
        printf("Limite de categorias atingido.\n");
        return;
    }

    novo_id = proximo_id_categoria();
    if (novo_id == -1) {
        printf("Nao foi possivel gerar um novo ID de categoria.\n");
        return;
    }

    printf("\nID gerado automaticamente: %d\n", novo_id);

    ids_categorias[total_categorias] = novo_id;

    while (1) {
        ler_texto("Nome da categoria: ",
                  nomes_categorias[total_categorias],
                  TAM_NOME_CATEGORIA, 1);

        if (localizar_nome_categoria(nomes_categorias[total_categorias]) == -1) {
            break;
        }
        printf("Ja existe uma categoria com esse nome.\n");
    }

    ler_texto("Descricao (opcional): ",
              descricoes_categorias[total_categorias],
              TAM_DESCRICAO, 0);

    total_categorias++;
    salvar_categorias();
    printf("Categoria cadastrada com sucesso.\n");
}

void editar_categoria(void) {
    int id;
    int posicao;
    char novo_nome[TAM_NOME_CATEGORIA];

    id = ler_inteiro("ID da categoria que deseja editar: ");
    posicao = localizar_id_categoria(id);

    if (posicao == -1) {
        printf("Categoria nao encontrada.\n");
        return;
    }

    while (1) {
        ler_texto("Novo nome da categoria: ", novo_nome, TAM_NOME_CATEGORIA, 1);

        if (strcmp(novo_nome, nomes_categorias[posicao]) == 0 ||
            localizar_nome_categoria(novo_nome) == -1) {
            break;
        }
        printf("Ja existe uma categoria com esse nome.\n");
    }

    strcpy(nomes_categorias[posicao], novo_nome);
    ler_texto("Nova descricao (pode ficar vazia): ",
              descricoes_categorias[posicao], TAM_DESCRICAO, 0);

    salvar_categorias();
    printf("Categoria atualizada com sucesso.\n");
}

void excluir_categoria(void) {
    int id;
    int posicao;
    int i;

    id = ler_inteiro("ID da categoria que deseja excluir: ");
    posicao = localizar_id_categoria(id);

    if (posicao == -1) {
        printf("Categoria nao encontrada.\n");
        return;
    }

    if (categoria_tem_agentes(id)) {
        printf("Nao e possivel excluir: existem agentes vinculados a esta categoria.\n");
        return;
    }

    for (i = posicao; i < total_categorias - 1; i++) {
        ids_categorias[i] = ids_categorias[i + 1];
        strcpy(nomes_categorias[i], nomes_categorias[i + 1]);
        strcpy(descricoes_categorias[i], descricoes_categorias[i + 1]);
    }

    total_categorias--;
    salvar_categorias();
    printf("Categoria excluida com sucesso.\n");
}

void selecionar_categoria(int *id_categoria) {
    int id;

    while (1) {
        listar_categorias();

        if (total_categorias == 0) {
            printf("Cadastre uma categoria antes de cadastrar um agente.\n");
            *id_categoria = -1;
            return;
        }

        id = ler_inteiro("Digite o ID da categoria do agente: ");

        if (localizar_id_categoria(id) != -1) {
            *id_categoria = id;
            return;
        }

        printf("Categoria inexistente. Escolha um ID cadastrado.\n");
    }
}

void imprimir_agente(int posicao) {
    int pos_categoria = localizar_id_categoria(id_categorias_agentes[posicao]);

    printf("\nID: %d\n", ids[posicao]);
    printf("Nome: %s\n", nomes[posicao]);
    printf("Script: %s\n", scripts[posicao]);
    printf("Nota: %d\n", notas[posicao]);

    if (pos_categoria != -1) {
        printf("Categoria: %s (ID: %d)\n",
               nomes_categorias[pos_categoria],
               id_categorias_agentes[posicao]);
    } else {
        printf("Categoria: nao encontrada\n");
    }
}

void cadastrar_agente(void) {
    int id;
    int nota;
    int id_categoria;

    if (total_agentes >= MAX_AGENTES) {
        printf("Limite de 50 agentes atingido.\n");
        return;
    }

    if (total_categorias == 0) {
        printf("Cadastre uma categoria antes de cadastrar um agente.\n");
        return;
    }

    id = proximo_id_agente();
    if (id == -1) {
        printf("Nao foi possivel gerar um novo ID de agente.\n");
        return;
    }

    printf("\nID gerado automaticamente: %d\n", id);

    selecionar_categoria(&id_categoria);
    if (id_categoria == -1) return;

    ids[total_agentes] = id;
    ler_texto("Nome: ", nomes[total_agentes], TAM_NOME_AGENTE, 1);
    ler_texto("Script/instrucoes: ", scripts[total_agentes], TAM_SCRIPT, 1);

    do {
        nota = ler_inteiro("Nota (0 a 5): ");
        if (nota < 0 || nota > 5) {
            printf("A nota deve estar entre 0 e 5.\n");
        }
    } while (nota < 0 || nota > 5);

    notas[total_agentes] = nota;
    id_categorias_agentes[total_agentes] = id_categoria;
    total_agentes++;

    salvar_agentes();
    printf("Agente cadastrado com sucesso.\n");
}

void listar_agentes(void) {
    int i;

    if (total_agentes == 0) {
        printf("Nenhum agente cadastrado.\n");
        return;
    }

    for (i = 0; i < total_agentes; i++) {
        imprimir_agente(i);
    }
}

void buscar_agente_id(void) {
    int id;
    int posicao;

    id = ler_inteiro("Digite o ID do agente: ");
    posicao = localizar_id_agente(id);

    if (posicao == -1) {
        printf("Agente nao encontrado.\n");
        return;
    }

    imprimir_agente(posicao);
}

void buscar_agente_nome(void) {
    char nome[TAM_NOME_AGENTE];
    int i;
    int encontrado = 0;

    ler_texto("Digite o nome: ", nome, TAM_NOME_AGENTE, 1);

    for (i = 0; i < total_agentes; i++) {
        if (strcmp(nomes[i], nome) == 0) {
            imprimir_agente(i);
            encontrado = 1;
        }
    }

    if (!encontrado) printf("Agente nao encontrado.\n");
}

void editar_agente(void) {
    int id;
    int posicao;
    int nota;
    int id_categoria;

    id = ler_inteiro("ID do agente que deseja editar: ");
    posicao = localizar_id_agente(id);

    if (posicao == -1) {
        printf("Agente nao encontrado.\n");
        return;
    }

    ler_texto("Novo nome: ", nomes[posicao], TAM_NOME_AGENTE, 1);
    ler_texto("Novo script/instrucoes: ", scripts[posicao], TAM_SCRIPT, 1);

    do {
        nota = ler_inteiro("Nova nota (0 a 5): ");
        if (nota < 0 || nota > 5) {
            printf("A nota deve estar entre 0 e 5.\n");
        }
    } while (nota < 0 || nota > 5);

    notas[posicao] = nota;

    printf("Selecione a nova categoria do agente:\n");
    selecionar_categoria(&id_categoria);
    if (id_categoria == -1) return;

    id_categorias_agentes[posicao] = id_categoria;

    salvar_agentes();
    printf("Agente atualizado com sucesso.\n");
}

void excluir_agente(void) {
    int id;
    int posicao;
    int i;

    id = ler_inteiro("ID do agente que deseja excluir: ");
    posicao = localizar_id_agente(id);

    if (posicao == -1) {
        printf("Agente nao encontrado.\n");
        return;
    }

    for (i = posicao; i < total_agentes - 1; i++) {
        ids[i] = ids[i + 1];
        strcpy(nomes[i], nomes[i + 1]);
        strcpy(scripts[i], scripts[i + 1]);
        notas[i] = notas[i + 1];
        id_categorias_agentes[i] = id_categorias_agentes[i + 1];
    }

    total_agentes--;
    salvar_agentes();
    printf("Agente excluido com sucesso.\n");
}

int main(void) {
    int opcao;

    carregar_categorias();
    carregar_agentes();

    do {
        printf("\n===== AI AGENT REGISTRY =====\n");
        printf("1 - Cadastrar categoria\n");
        printf("2 - Listar categorias\n");
        printf("3 - Editar categoria\n");
        printf("4 - Excluir categoria\n");
        printf("5 - Cadastrar agente\n");
        printf("6 - Listar agentes\n");
        printf("7 - Buscar agente por ID\n");
        printf("8 - Buscar agente por nome\n");
        printf("9 - Editar agente\n");
        printf("10 - Excluir agente\n");
        printf("0 - Sair\n");

        opcao = ler_inteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1: cadastrar_categoria(); break;
            case 2: listar_categorias(); break;
            case 3: editar_categoria(); break;
            case 4: excluir_categoria(); break;
            case 5: cadastrar_agente(); break;
            case 6: listar_agentes(); break;
            case 7: buscar_agente_id(); break;
            case 8: buscar_agente_nome(); break;
            case 9: editar_agente(); break;
            case 10: excluir_agente(); break;
            case 0:
                salvar_categorias();
                salvar_agentes();
                printf("Programa encerrado.\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    return 0;
}
