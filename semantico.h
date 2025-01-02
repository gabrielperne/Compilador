#include<stdlib.h>
#include<stdio.h>

typedef struct noPilhaSemantica
{
    TOKEN token;
    int eLinha,eColuna;
    struct noPilhaSemantica* proximo;
}NoPilhaSemantica;

typedef struct
{
    NoPilhaSemantica* topo;
    int tamanho;
}PilhaSemantica;

typedef struct noComum
{
    int tipoVar;
    struct noComum *proximo;
}NoComum;

typedef struct filaComum
{
    NoComum *inicio;
    NoComum *fim;
    int tamanho;
}FilaComum;

extern int contErroSemantico;
extern FILE* temp;
void SemanticoIniciar();
void empilharSemantica(TOKEN);
void desempilharSemantica();
void callSemantico(int);
void finalizarECriarArquivo();
