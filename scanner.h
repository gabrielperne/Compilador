#include <stdio.h>

typedef struct {
    char classe[20];
    char *lexema;
    int tipo;
    int indice;
    int linha;
    int coluna;
} TOKEN;

typedef struct Node {
    TOKEN token;
    struct Node* prox;
} Node;

extern int linha,coluna;
extern int linhaantiga,colunaantiga;
extern FILE* arquivo;

TOKEN scanner();
void atualizarTipoToken(TOKEN,int);
void imprimir_tabela_simbolos();


