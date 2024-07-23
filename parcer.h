typedef enum
{
    P=25,
    V,
    LV,
    D,
    L,
    TIPO,
    A,
    ES,
    ARG,
    CMD,
    LD,
    OPRD,
    COND,
    CAB,
    EXP_R,
    CP,
    R,
    CABR,
    CPR,
}nonTerminal;

typedef enum
{
    shift,
    reduce,
    _goto,
    accept,
    erro
}Opcao;

typedef struct
{
   Opcao action;
   int valor;
   char* erro;
}Acao;

typedef struct noPilha
{
    int valor;
    struct noPilha* proximo;
}NoPilha;

typedef struct
{
    NoPilha* topo;
    int tamanho;
}Pilha;

typedef struct
{
    char* simbolica;
    int tamanho;
    nonTerminal reducao;
}RegraDeProducao;

void criarTabelaSintese();
void iniciarSintatico();
void parser();
void Msgerro();
int RecuperarFrase();
void Erro();

