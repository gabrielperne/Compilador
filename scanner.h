typedef struct {
    char classe[20];
    char lexema[50];
    char tipo[20];
    int indice;
} TOKEN;

typedef struct Node {
    TOKEN token;
    struct Node* prox;
} Node;

extern int linha,coluna;
extern int linhaantiga,colunaantiga;
extern FILE* arquivo;

TOKEN scanner();
void imprimir_tabela_simbolos();

