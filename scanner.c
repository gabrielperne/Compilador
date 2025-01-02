#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "scanner.h"

#define TAMANHO_HASH 19

Node* tabela_simbolos[TAMANHO_HASH] = {NULL};

int hash(char* lexema) {
    int hash = 0;
    for (int i = 0; lexema[i] != '\0'; i++) {
        hash += lexema[i];
    }
    return hash % TAMANHO_HASH;
}

void inserir_na_tabela(TOKEN token) {
    int index = hash(token.lexema);

    Node* atual = tabela_simbolos[index];
    while (atual != NULL) {
        if (strcmp(atual->token.lexema, token.lexema) == 0) {
            return;
        }
        atual = atual->prox;
    }

    Node* novo = (Node*)malloc(sizeof(Node));
    if (novo == NULL) {
        printf("Erro ao alocar memória.\n");
        exit(EXIT_FAILURE);
    }
    novo->token = token;
    novo->prox = tabela_simbolos[index];
    tabela_simbolos[index] = novo;
}

int buscar_na_tabela(TOKEN* token) {
    int index = hash(token->lexema);

    Node* atual = tabela_simbolos[index];
    while (atual != NULL) {
        if (strcmp(atual->token.lexema, token->lexema) == 0) {
            strcpy(token->classe, atual->token.classe);
            token->tipo = atual->token.tipo;
            return 1;
        }
        atual = atual->prox;
    }

    return 0;
}

void atualizarTipoToken(TOKEN token,int tipo)
{
    int index = hash(token.lexema);

    Node* atual = tabela_simbolos[index];
    while (atual != NULL) {
        if (strcmp(atual->token.lexema, token.lexema) == 0) {
            atual->token.tipo = tipo;
            return;
        }
        atual = atual->prox;
    }
}

int linha = 1, coluna = 0;
int linhaantiga = 1, colunaantiga = 2;

void setLinhaColuna(TOKEN *token)
{
    token->linha = linha;
    token->coluna = coluna;
}

FILE* arquivo = NULL;
TOKEN scanner() {
    TOKEN token;
    token.lexema = (char *)calloc(100,1);
    token.tipo = -1;
    char caractere;
    linhaantiga = linha;
    colunaantiga = coluna;
    caractere = fgetc(arquivo);

    while (caractere == ' ' || caractere == '\n' || caractere == '\t' || caractere == '{') {
        if (caractere == '{') {
            while (caractere != '}' && caractere != EOF) {
                caractere = fgetc(arquivo);
                if (caractere == '\n') {
                    linha++;
                    coluna = 1;
                } else {
                    coluna++;
                }
            }
        }
        if (caractere == '\n') {
            linha++;
            coluna = 1;
        } else {
            coluna++;
        }
        caractere = fgetc(arquivo);
    }

    if (caractere == EOF) {
        strcpy(token.classe, "EOF");
        strcpy(token.lexema, "");
        token.indice = 4;
        setLinhaColuna(&token);
        return token;
    }

    if (caractere == '<' || caractere == '>' || caractere == '=') {

        char proximo_caractere = fgetc(arquivo);
        coluna++;

        if (caractere == '<' && proximo_caractere == '-') {
            token.lexema[0] = caractere;
            token.lexema[1] = '-';
            token.lexema[2] = '\0';
            strcpy(token.classe, "RCB");
            token.indice = 6;
            setLinhaColuna(&token);
            return token;
        }

        if (proximo_caractere == '=') {
            token.lexema[0] = caractere;
            token.lexema[1] = '=';
            token.lexema[2] = '\0';
            strcpy(token.classe, "OPR");
            token.indice = 5;
            setLinhaColuna(&token);
            return token;
        } else {
            ungetc(proximo_caractere, arquivo);
            token.lexema[0] = caractere;
            token.lexema[1] = '\0';
            strcpy(token.classe, "OPR");
            token.indice = 5;
            setLinhaColuna(&token);
            return token;
        }
    }

    if (isdigit(caractere)) {
        int i = 0;
        token.lexema[i++] = caractere;
        caractere = fgetc(arquivo);
        coluna++;

        while (isdigit(caractere) || caractere == '.') {
            token.lexema[i++] = caractere;
            caractere = fgetc(arquivo);
            coluna++;
        }

        if (caractere == 'E' || caractere == 'e') {
            token.lexema[i++] = caractere;
            caractere = fgetc(arquivo);
            coluna++;

            if (caractere == '+' || caractere == '-') {
                token.lexema[i++] = caractere;
                caractere = fgetc(arquivo);
                coluna++;
            }

            while (isdigit(caractere)) {
                token.lexema[i++] = caractere;
                caractere = fgetc(arquivo);
                coluna++;
            }
        }

        token.lexema[i] = '\0';


        if (strchr(token.lexema, '.') == NULL && strchr(token.lexema, 'E') == NULL && strchr(token.lexema, 'e') == NULL) {
            strcpy(token.classe, "NUM");
            token.indice = 1;
            token.tipo = 0;
        } else {
            strcpy(token.classe, "NUM");
            token.indice = 1;
            token.tipo = 1;
        }

        ungetc(caractere, arquivo);
        setLinhaColuna(&token);
        return token;
    }

    if (isalpha(caractere)) {
        int i = 0;
        token.lexema[i++] = caractere;
        caractere = fgetc(arquivo);
        coluna++;

        while (isalpha(caractere) || isdigit(caractere) || caractere == '_') {
            token.lexema[i++] = caractere;
            caractere = fgetc(arquivo);
            coluna++;
        }
        token.lexema[i] = '\0';
        ungetc(caractere, arquivo);

        if (strcmp(token.lexema, "inicio") == 0 || strcmp(token.lexema, "varinicio") == 0 ||
            strcmp(token.lexema, "varfim") == 0 || strcmp(token.lexema, "escreva") == 0 ||
            strcmp(token.lexema, "leia") == 0 || strcmp(token.lexema, "se") == 0 ||
            strcmp(token.lexema, "entao") == 0 || strcmp(token.lexema, "fimse") == 0 ||
            strcmp(token.lexema, "repita") == 0 || strcmp(token.lexema, "fimrepita") == 0 ||
            strcmp(token.lexema, "fim") == 0 || strcmp(token.lexema, "inteiro") == 0 ||
            strcmp(token.lexema, "literal") == 0 || strcmp(token.lexema, "real") == 0) {


        if (strcmp(token.lexema, "inicio") == 0) {
            token.indice = 12;
        } else if (strcmp(token.lexema, "varinicio") == 0) {
            token.indice = 13;
        } else if (strcmp(token.lexema, "varfim") == 0) {
            token.indice = 14;
        } else if (strcmp(token.lexema, "escreva") == 0) {
            token.indice = 15;
        } else if (strcmp(token.lexema, "leia") == 0) {
            token.indice = 16;
        } else if (strcmp(token.lexema, "se") == 0 ){
            token.indice = 17;
        } else if (strcmp(token.lexema, "entao") == 0){
            token.indice = 18;
        } else if (strcmp(token.lexema, "fimse") == 0) {
            token.indice = 19;
        } else if (strcmp(token.lexema, "repita") == 0) {
            token.indice = 20;
        } else if (strcmp(token.lexema, "fimrepita") == 0) {
            token.indice = 21;
        } else if (strcmp(token.lexema, "fim") == 0) {
            token.indice = 22;
        } else if (strcmp(token.lexema, "inteiro") == 0) {
            token.tipo = 0;
            token.indice = 23;
        } else if (strcmp(token.lexema, "literal") == 0) {
            token.tipo = 2;
            token.indice = 24;
        } else if (strcmp(token.lexema, "real") == 0) {
            token.tipo = 1;
            token.indice = 25;
        }
        strcpy(token.classe, token.lexema);
        } else {
            strcpy(token.classe, "ID");
            token.indice = 3;
        }

        setLinhaColuna(&token);
        int encontrado = buscar_na_tabela(&token);
        if (encontrado) {
            return token;
        } else {
            inserir_na_tabela(token);
        }

        return token;
    }

    if (caractere == '"' || caractere == '\'') {
        int i = 0;
        token.lexema[i++] = caractere;
        caractere = fgetc(arquivo);
        coluna++;

        while (caractere != '"' && caractere != '\'' && caractere != EOF) {
            token.lexema[i++] = caractere;
            caractere = fgetc(arquivo);
            coluna++;
        }
        token.lexema[i++] = caractere;
        token.lexema[i] = '\0';

        strcpy(token.classe, "LIT");
        token.indice = 2;
        token.tipo = 2;
        setLinhaColuna(&token);
        return token;
    }

    if (caractere == '+' || caractere == '-' || caractere == '*' || caractere == '/') {
        coluna++;
        token.lexema[0] = caractere;
        token.lexema[1] = '\0';
        strcpy(token.classe, "OPM");
        token.indice = 7;
        setLinhaColuna(&token);
        return token;
    }

    if (caractere == '(' || caractere == '.') {
        coluna++;
        token.lexema[0] = caractere;
        token.lexema[1] = '\0';
        strcpy(token.classe, "AB_P");
        token.indice = 8;
        setLinhaColuna(&token);
        return token;
    }

    if (caractere == ')') {
        coluna++;
        token.lexema[0] = caractere;
        token.lexema[1] = '\0';
        strcpy(token.classe, "FC_P");
        token.indice = 9;
        setLinhaColuna(&token);
        return token;
    }

    if (caractere == ';') {
        token.lexema[0] = caractere;
        token.lexema[1] = '\0';
        strcpy(token.classe, "PT_V");
        token.indice = 10;
        setLinhaColuna(&token);
        return token;
    }

    if (caractere == ',') {
        coluna++;
        token.lexema[0] = caractere;
        token.lexema[1] = '\0';
        strcpy(token.classe, "VIR");
        token.indice = 11;
        setLinhaColuna(&token);
        return token;
    }

    printf("ERRO Caractere invalido na linguagem, linha %d, coluna %d\n", linha, coluna);
    strcpy(token.classe, "ERRO");
    strcpy(token.lexema, "NULO");
    token.indice = 31;
    setLinhaColuna(&token);
    return token;
}


void imprimir_tabela_simbolos() {
    printf("================ Tabela de Simbolos ================\n");

    for (int i = 0; i < TAMANHO_HASH; ++i) {
        printf("%d\n", i);
        Node* atual = tabela_simbolos[i];
        while (atual != NULL) {
            printf("Classe: %s, Lexema: %s, Tipo: %d\n", atual->token.classe, atual->token.lexema, atual->token.tipo);
            atual = atual->prox;
        }
    }

    printf("===================================\n");
}

