#include "parcer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"

Pilha* pilha;
int colunas = 44;
Acao ACTION[80][44];

RegraDeProducao RegrasDeProducao[40]={
{.simbolica = "NUNCACAI",                       .tamanho = 1,   .reducao = P},
{.simbolica = "NUNCACAI",                       .tamanho = 1,   .reducao = P},
{.simbolica = "P-> inicio V A",                 .tamanho = 3,   .reducao = P},
{.simbolica = "V-> varincio LV",                .tamanho = 2,   .reducao = V},
{.simbolica = "LV-> D LV",                      .tamanho = 2,   .reducao = LV},
{.simbolica = "LV-> varfim pt_v",               .tamanho = 2,   .reducao = LV},
{.simbolica = "D-> TIPO L pt_v",                .tamanho = 3,   .reducao = D},
{.simbolica = "L-> id vir L",                   .tamanho = 3,   .reducao = L},
{.simbolica = "L-> id",                         .tamanho = 1,   .reducao = L},
{.simbolica = "TIPO-> inteiro",                 .tamanho = 1,   .reducao = TIPO},
{.simbolica = "TIPO-> real",                    .tamanho = 1,   .reducao = TIPO},
{.simbolica = "TIPO-> literal",                 .tamanho = 1,   .reducao = TIPO},
{.simbolica = "A-> ES A",                       .tamanho = 2,   .reducao = A},
{.simbolica = "ES-> leia id pt_v",              .tamanho = 3,   .reducao = ES},
{.simbolica = "ES-> escreva ARG pt_v",          .tamanho = 3,   .reducao = ES},
{.simbolica = "ARG-> lit",                      .tamanho = 1,   .reducao = ARG},
{.simbolica = "ARG-> num",                      .tamanho = 1,   .reducao = ARG},
{.simbolica = "ARG-> id",                       .tamanho = 1,   .reducao = ARG},
{.simbolica = "A-> CMD A",                      .tamanho = 2,   .reducao = A},
{.simbolica = "CMD-> id rcb LD pt_v",           .tamanho = 4,   .reducao = CMD},
{.simbolica = "LD-> OPRD opm OPRD",             .tamanho = 3,   .reducao = LD},
{.simbolica = "LD-> OPRD",                      .tamanho = 1,   .reducao = LD},
{.simbolica = "OPRD-> id",                      .tamanho = 1,   .reducao = OPRD},
{.simbolica = "OPRD-> num",                     .tamanho = 1,   .reducao = OPRD},
{.simbolica = "A-> COND A",                     .tamanho = 2,   .reducao = A},
{.simbolica = "COND-> CAB CP",                  .tamanho = 2,   .reducao = COND},
{.simbolica = "CAB-> se ab_p EXP_R fc_p entao", .tamanho = 5,   .reducao = CAB},
{.simbolica = "EXP_R-> OPRD opr OPRD",          .tamanho = 3,   .reducao = EXP_R},
{.simbolica = "CP-> ES CP",                     .tamanho = 2,   .reducao = CP},
{.simbolica = "CP-> CMD CP",                    .tamanho = 2,   .reducao = CP},
{.simbolica = "CP-> COND CP",                   .tamanho = 2,   .reducao = CP},
{.simbolica = "CP-> fimse",                     .tamanho = 1,   .reducao = CP},
{.simbolica = "A-> R A",                        .tamanho = 2,   .reducao = A},
{.simbolica = "R -> CABR CPR",                  .tamanho = 2,   .reducao = R},
{.simbolica = "CABR-> repita ab_p EXP_R fc_p",  .tamanho = 4,   .reducao = CABR},
{.simbolica = "CPR-> ES CPR",                   .tamanho = 2,   .reducao = CPR},
{.simbolica = "CPR-> CMD CPR",                  .tamanho = 2,   .reducao = CPR},
{.simbolica = "CPR-> COND CPR",                 .tamanho = 2,   .reducao = CPR},
{.simbolica = "CPR-> fimrepita",                .tamanho = 1,   .reducao = CPR},
{.simbolica = "A-> fim",                        .tamanho = 1,   .reducao = A},
};

void empilhar(int valor)
{
    NoPilha* No = (NoPilha*)malloc(sizeof(NoPilha));
    No->proximo = NULL;
    No->valor = valor;
    if(!pilha->tamanho)
        pilha->topo = No;
    else
    {
        No->proximo = pilha->topo;
        pilha->topo = No;
    }
    pilha->tamanho++;
}

int desempilhar()
{
    int aux = pilha->topo->valor;
    NoPilha* help = pilha->topo;
    pilha->topo = pilha->topo->proximo;
    free(help);
    pilha->tamanho--;
    return aux;
}

void criarTabelaSintese(FILE *file)
{
    char* buffer = (char *)calloc(100,1);
    char* inicio = buffer;
    char next;
    int stringsize = 0;
    int quantidade = 0;
    while(next!='\n'&&next!=EOF)
        next = fgetc(file);
    while(next!='%'&&next!=EOF)
        next = fgetc(file);
    do
    {
        next = fgetc(file);
        if(next=='%'||next=='\n')
        {
            int x,y;
            x = (quantidade)/colunas;
            y = (quantidade)%colunas;
            if(!stringsize)
            {
                ACTION[x][y].action = erro;
                ACTION[x][y].erro = NULL;
                quantidade++;
            }
            else
            {
                if(buffer[0]=='s')
                {
                    ACTION[x][y].action = shift;
                    buffer++;
                }
                else if(buffer[0]=='a')
                {
                    ACTION[x][y].action = accept;
                }
                else if(buffer[0]=='r')
                {
                    ACTION[x][y].action = reduce;
                    buffer++;
                }
                ACTION[quantidade/colunas][quantidade%colunas].valor = atoi(buffer);
                buffer = inicio;
                memset(buffer,0,stringsize);
                stringsize = 0;
                quantidade++;
            }
            if(next=='\n')
            while(next!='%'&&next!=EOF)
                next = fgetc(file);
        }
        else
        {
            buffer[stringsize++] = next;
            if(stringsize>99)
            {
                printf("CELULA DA TABELA MAIOR QUE 100 CARACTERES");
                exit(0);
            }
        }

    }while(!feof(file));
    free(inicio);
}

void iniciarSintatico(FILE* file)
{
    criarTabelaSintese(file);
    pilha = (Pilha*)malloc(sizeof(Pilha));
    pilha->tamanho=0;
    empilhar(0);
}

int contadorTokenDeAtraso = 0;
TOKEN tokenDeAtraso;
TOKEN proximoToken()
{
    if(contadorTokenDeAtraso)
    {
        contadorTokenDeAtraso=0;
        return tokenDeAtraso;
    }
    return scanner();
}

int contadorerro = 0;
TOKEN next;
void parser()
{
    next = proximoToken();
    int valorTopo;
    while(1)
    {
        valorTopo = pilha->topo->valor;
        Acao acao = ACTION[valorTopo][next.indice - 1];
        if(acao.action == shift)
        {
            empilhar(acao.valor);
            next = proximoToken();
        }
        else if(acao.action == reduce)
        {
            RegraDeProducao regra = RegrasDeProducao[acao.valor];
            for(int i=0;i<regra.tamanho;i++)
                desempilhar();
            valorTopo = pilha->topo->valor;
            acao = ACTION[valorTopo][regra.reducao];
            empilhar(acao.valor);
            printf("%s\n",regra.simbolica);
        }
        else if(acao.action == accept)
        {
            FINAL:
            printf("Fim\n");
            printf("Total de erros: %d", contadorerro);
            return;
        }
        else
        {
            /*if(next.indice == 30){
                printf("ENTROU. ");
                next = proximoToken ();
                continue;
            }*/
            Msgerro(valorTopo);
            if(!RecuperarFrase(valorTopo))
            {
                while (ACTION[valorTopo][next.indice - 1].action != shift && ACTION[valorTopo][next.indice - 1].action != reduce && ACTION[valorTopo][next.indice - 1].action != accept)
                {
                    next = proximoToken ();
                    if (next.indice == 4) goto FINAL;
                }
            }
        }
    }
}
void Msgerro(int valorTopo)
{
    contadorerro ++;
    printf("ERRO%d: ", valorTopo);
    switch(valorTopo){
        case 0:
            printf("INICIO esperado. ");
            break;
        case 2:
            printf("VARINICIO esperado. ");
            break;
        case 9:
            printf("EOFL Codigo fora do programa");
            break;
        case 10:
            printf("ID esperado. ");
            break;
        case 11:
            printf("ID ou LIT ou num esperado.");
            break;
        case 12:
            printf("RCB esperado. ");
            break;
        case 14:
        case 16:
            printf("AB_P esperado. ");
            break;
        case 21:
        case 23:
        case 24:
        case 25:
        case 26:
            printf("Ponto e virgula esperado. ");
            break;
        case 28:
            printf("NUM ou ID esperado. ");
            break;
        case 29:
            printf("Ponto e virgula esperado. ");
            break;
        case 31:
            printf("OPM ou Ponto e Virgula esperado. ");
            break;
        case 32:
            printf("ID ou NUM esperado. ");
            break;
        case 33:
        case 34:
            printf("FC_P, PT_V, OPM, OPR esperado. ");
            break;
        case 35:
            printf("Ponto e virgula esperado. ");
            break;
        case 37:
        case 38:
        case 39:
            printf("Fimse esperado. ");
            break;
        case 44:
            printf("ID ou NUM esperado. ");
            break;
        case 45:
            printf("FC_P esperado. ");
            break;
        case 46:
            printf("entao esperado. ");
            break;
        case 48:
            printf("OPR esperado. ");
            break;
        case 49:
            printf("NUM ou ID esperado. ");
            break;
        case 50:
            printf("FC_P esperado. ");
            break;
        case 52:
        case 53:
        case 54:
            printf("fimrepita esperado. ");
            break;
        case 55:
            printf("erro desconhecido. ");
            break;
        case 59:
            printf("NUM ou ID. ");
            break;
        case 60:
            printf("FC_P. ");
            break;
        case 65:
        case 67:
            printf("TIPO ou Varfim. ");
            break;
        case 68:
            printf("Ponto e virgula esperado. ");
            break;
        case 71:
        case 72:
        case 73:
        case 74:
            printf("ID esperado. ");
            break;
        case 75:
            printf("Ponto e virgula esperado. ");
            break;
        case 76:
            printf("TIPO ou varfim esperado. ");
            break;
        case 78:
            printf("ID esperado. ");
            break;
        case 79:
            printf("Ponto e virgula ou virgula esperado. ");
            break;

    }
    printf("linha %d, coluna %d\n", linhaantiga, colunaantiga);

}

int RecuperarFrase(int valor){
    switch(valor){
        case 0:
            empilhar(2);
            break;
        case 2:
            empilhar(65);
            break;
        case 10:
            empilhar(21);
            break;
        case 11:
            empilhar(26);
            break;
        case 12:
            empilhar(28);
            break;
        case 14:
            empilhar(44);
            break;
        case 16:
            empilhar(59);
            break;
        case 21:
            empilhar(22);
            break;
        case 23:
            empilhar(27);
            break;
        case 24:
        case 25:
        case 26:
            for(int i=0; i<1; i++)
                desempilhar();
            empilhar(ACTION[pilha->topo->valor][ARG].valor);
            tokenDeAtraso = next;
            contadorTokenDeAtraso = 1;
            next.indice = 10;
            break;
        case 28:
            empilhar(33);
            break;
        case 29:
            empilhar(30);
            break;
        case 32:
            empilhar(33);
            break;
        case 35:
            for(int i=0; i<3; i++)
                desempilhar();
            empilhar(ACTION[pilha->topo->valor][LD].valor);
            tokenDeAtraso = next;
            contadorTokenDeAtraso = 1;
            next.indice = 10;
            break;
        case 44:
            empilhar(33);
            break;
        case 45:
            empilhar(46);
            break;
        case 46:
            empilhar(47);
            break;
        case 48:
            empilhar(49);
            break;
        case 49:
            empilhar(33);
            break;
        case 50:
            for(int i=0; i<3; i++)
                desempilhar();
            empilhar(ACTION[pilha->topo->valor][EXP_R].valor);
            tokenDeAtraso = next;
            contadorTokenDeAtraso = 1;
            next.indice = 9;
            break;
        case 59:
            empilhar(33);
            break;
        case 60:
            empilhar(61);
            break;
        case 68:
            empilhar(69);
            break;
        case 71:
        case 72:
        case 73:
            for(int i=0; i<1; i++)
                desempilhar();
            empilhar(ACTION[pilha->topo->valor][TIPO].valor);
            tokenDeAtraso = next;
            contadorTokenDeAtraso = 1;
            next.indice = 3;
            break;
        case 74:
            empilhar(77);
            break;
        case 75:
            empilhar(76);
            break;
        case 78:
            empilhar(77);
            break;
        case 79:
            for(int i=0; i<3; i++)
                desempilhar();
            empilhar(ACTION[pilha->topo->valor][L].valor);
            tokenDeAtraso = next;
            contadorTokenDeAtraso = 1;
            next.indice = 10;
            break;
            default:
            return 0;
    }
    return 1;
}

