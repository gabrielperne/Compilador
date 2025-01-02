#include "parcer.h"
#include "scanner.h"
#include "semantico.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define StringPartSize 5

static int contadorVariavelTemporararia = 0;

static int contadorLabel = 0;

static PilhaSemantica *pilhaSemantica;

static FilaComum *tipoDasTemporarias;

char *atrasoDeEXP_R;

int contErroSemantico = 0;

FILE* temp;


static char *simbolicaTipagem[] = {
    "int ",
    "double ",
    "literal ",
};

static char *simbolicaScanf[] = {
    "scanf(\"\%d\",&",
    "scanf(\"\%lf\",&",
    "scanf(\"\%s\",",
};

static char *simbolicaPrintf[] = {
    "\"\%d\",",
    "\"\%lf\",",
    "\"\%s\","
};

int conversaoDeTipo[] = {
    1,
    2,
    3
};

void SemanticoIniciar()
{
    pilhaSemantica = (PilhaSemantica*) malloc(sizeof(PilhaSemantica));
    pilhaSemantica->topo = NULL;
    pilhaSemantica->tamanho = 0;
    tipoDasTemporarias = (FilaComum*) malloc(sizeof(FilaComum));
    tipoDasTemporarias->inicio = NULL;
    tipoDasTemporarias->fim = NULL;
    tipoDasTemporarias->tamanho = 0;

    temp = fopen("temp.txt","w+");
}

void enfileirarComum(int tipo)
{
    NoComum *novo = (NoComum*)malloc(sizeof(NoComum));
    novo->tipoVar = tipo;
    novo->proximo = NULL;
    if(!tipoDasTemporarias->tamanho)
    {
        tipoDasTemporarias->inicio=novo;
        tipoDasTemporarias->fim=novo;
    }
    else
    {
        tipoDasTemporarias->fim->proximo = novo;
        tipoDasTemporarias->fim = novo;
    }
    tipoDasTemporarias->tamanho++;
}

void empilharSemantica(TOKEN token)
{
    NoPilhaSemantica *novo = (NoPilhaSemantica*) malloc(sizeof(NoPilhaSemantica));
    novo->proximo = pilhaSemantica->topo;
    novo->eLinha = linhaantiga;
    novo->eColuna = colunaantiga;
    pilhaSemantica->topo = novo;
    novo->token = token;
    pilhaSemantica->tamanho++;
}

void desempilharSemantica()
{
    NoPilhaSemantica *aux = pilhaSemantica->topo;
    pilhaSemantica->topo = aux->proximo;
    free(aux);
    pilhaSemantica->tamanho--;
}

void printarNoArquivo(char* conteudo)
{
    fwrite(conteudo,strlen(conteudo),1,temp);
}

void finalizarECriarArquivo()
{
    FILE *saida = fopen("PROGRAMA.C","w+");
    char* cabecalho = "#include<stdio.h>\ntypedef char literal[256];\nvoid main(void)\n{\n";
    fwrite(cabecalho,strlen(cabecalho),1,saida);
    NoComum *aux = tipoDasTemporarias->inicio;
    int i=0;
    while(aux!=NULL)
    {
        char buffer[30];
        sprintf(buffer,"%sT%d;\n",simbolicaTipagem[aux->tipoVar],i++);
        fwrite(buffer,strlen(buffer),1,saida);
        aux = aux->proximo;
    }
    fseek(temp,0,SEEK_SET);
    char buffer = getc(temp);
    while(buffer!=EOF)
    {
        fputc(buffer,saida);
        buffer = getc(temp);
    }
    fwrite("}",1,1,saida);
    fclose(saida);
}

void callSemantico(int regra)
{
    switch(regra)
    {
    case 6:;
        NoPilhaSemantica *Tipo = pilhaSemantica->topo;
        while(Tipo->token.indice!=500)
            Tipo = Tipo->proximo;
        NoPilhaSemantica *Id = pilhaSemantica->topo;
        while(Id!=Tipo)
        {
            if(Id->token.indice == 3)
            {
                if(Id->token.tipo!=-1)
                {
                    printf("\n%d:ERRO SEMANTICO, Variavel %s declarada mais de uma vez (Linha %d Coluna %d)\n\n",regra,Id->token.lexema,Id->token.linha,Id->token.coluna);
                    ErroRR = 1;
                    contErroSemantico++;
                }
                else
                {
                    Id->token.tipo = Tipo->token.tipo;
                    char *Tipagem = simbolicaTipagem[Id->token.tipo];
                    printarNoArquivo(Tipagem);
                    printarNoArquivo(Id->token.lexema);
                    printarNoArquivo(";\n");
                    atualizarTipoToken(Id->token,Tipo->token.tipo);
                }
            }
            Id = Id->proximo;
        }
        break;
    case 9:
    case 10:
    case 11:
        pilhaSemantica->topo->token.indice = 500;
        break;
    case 13:;
        NoPilhaSemantica *leia = pilhaSemantica->topo->proximo;
        if(leia->token.tipo==-1)
        {
            printf("\n%d:ERRO SEMANTICO, Variavel %s nao declarada (Linha %d Coluna %d)\n\n",regra,leia->token.lexema,leia->token.linha,leia->token.coluna);
            ErroRR = 1;
            contErroSemantico++;
            break;
        }
        char *TipoScanf = simbolicaScanf[leia->token.tipo];
        printarNoArquivo(TipoScanf);
        printarNoArquivo(leia->token.lexema);
        printarNoArquivo(");\n");
        break;
    case 14:;
        NoPilhaSemantica *escreva = pilhaSemantica->topo->proximo;
        printarNoArquivo("printf(");
        printarNoArquivo(escreva->token.lexema);
        printarNoArquivo(");\n");
        break;
    case 15:
    case 16:
    case 17:;
        TOKEN verificar = pilhaSemantica->topo->token;
        if(verificar.indice==3)
        {
            if(verificar.tipo==-1)
            {
                printf("\n%d:ERRO SEMANTICO, Variavel %s nao declarada (Linha %d Coluna %d)\n\n",regra,verificar.lexema,verificar.linha,verificar.coluna);
                ErroRR = 1;
                contErroSemantico++;
                break;
            }
            TOKEN substituicao;
            substituicao.lexema = (char*)calloc(strlen(verificar.lexema)+8,sizeof(char));
            strcat(substituicao.lexema,simbolicaPrintf[verificar.tipo]);
            strcat(substituicao.lexema,verificar.lexema);
            desempilharSemantica();
            empilharSemantica(substituicao);
        }
        break;
    case 19:;
        NoPilhaSemantica *recepitor = pilhaSemantica->topo->proximo->proximo->proximo;
        NoPilhaSemantica *conteudo = pilhaSemantica->topo->proximo;
        if(recepitor->token.tipo==-1)
        {
            printf("\n%d:ERRO SEMANTICO, Variavel %s nao declarada (Linha %d Coluna %d)\n\n",regra,recepitor->token.lexema,recepitor->token.linha,recepitor->token.coluna);
            ErroRR = 1;
            contErroSemantico++;
            break;
        }
        else if(conteudo->token.tipo==-1)
        {
            printf("\n%d:ERRO SEMANTICO, Variavel %s nao declarada (Linha %d Coluna %d)\n\n",regra,conteudo->token.lexema,conteudo->token.linha,conteudo->token.coluna);
            ErroRR = 1;
            contErroSemantico++;
            break;
        }
        else if(conteudo->token.tipo!=recepitor->token.tipo)
        {
            printf("\n%d:ERRO SEMANTICO, Tipos diferentes na atribuicao (Linha %d Coluna %d)\n\n",regra,conteudo->token.linha,conteudo->token.coluna);
            ErroRR = 1;
            contErroSemantico++;
            break;
        }
        printarNoArquivo(recepitor->token.lexema);
        printarNoArquivo("=");
        printarNoArquivo(conteudo->token.lexema);
        printarNoArquivo(";\n");
        break;
    case 20:;
        NoPilhaSemantica *var2 = pilhaSemantica->topo;
        NoPilhaSemantica *operadorMatematico = var2->proximo;
        NoPilhaSemantica *var1 = operadorMatematico->proximo;
        if(var2->token.tipo==-1)
        {
            printf("\n%d:ERRO SEMANTICO, Variavel %s nao declarada (Linha %d Coluna %d)\n\n",regra,var2->token.lexema,var2->token.linha,var2->token.coluna);
            ErroRR = 1;
            contErroSemantico++;
            break;
        }
        else if(var1->token.tipo==-1)
        {
            printf("\n%d:ERRO SEMANTICO, Variavel %s nao declarada (Linha %d Coluna %d)\n\n",regra,var1->token.lexema,var1->token.linha,var1->token.coluna);
            ErroRR = 1;
            contErroSemantico++;
            break;
        }
        else if(var1->token.tipo==2)
        {
            printf("\n%d:ERRO SEMANTICO, Operacao matematica com literal nao permitida (Linha %d Coluna %d)\n\n",regra,var1->token.linha,var1->token.coluna);
            ErroRR = 1;
            contErroSemantico++;
            break;
        }
        else if(var2->token.tipo==2)
        {
            printf("\n%d:ERRO SEMANTICO, Operacao matematica com literal nao permitida (Linha %d Coluna %d)\n\n",regra,var2->token.linha,var2->token.coluna);
            ErroRR = 1;
            contErroSemantico++;
            break;
        }
        else if(var1->token.tipo!=var2->token.tipo)
        {
            printf("\n%d:ERRO SEMANTICO, Variaveis %s e %s nao compativeis (Linha %d Coluna %d)\n\n",regra,var1->token.lexema,var2->token.lexema,var2->token.linha,var2->token.coluna);
            ErroRR = 1;
            contErroSemantico++;
            break;
        }
        enfileirarComum(var1->token.tipo);
        TOKEN novo;
        char *buffer = (char*)calloc(strlen(var1->token.lexema)+strlen(var2->token.lexema)+strlen(operadorMatematico->token.lexema)+7,sizeof(char));
        sprintf(buffer,"T%d = %s%s%s;\n",contadorVariavelTemporararia++,var1->token.lexema,operadorMatematico->token.lexema,var2->token.lexema);
        printarNoArquivo(buffer);
        novo.tipo = var2->token.tipo;
        novo.lexema = (char*)calloc(10,sizeof(char));
        sprintf(buffer,"T%d",contadorVariavelTemporararia-1);
        strcat(novo.lexema,buffer);
        desempilharSemantica();
        desempilharSemantica();
        desempilharSemantica();
        empilharSemantica(novo);
        break;
    case 25:
        printarNoArquivo("}\n");
        break;
    case 26:;
        TOKEN relacionado = pilhaSemantica->topo->proximo->proximo->token;
        printarNoArquivo("if(");
        printarNoArquivo(relacionado.lexema);
        printarNoArquivo(")\n{\n");
        break;
    case 27:;
        NoPilhaSemantica *varf2 = pilhaSemantica->topo;
        NoPilhaSemantica *operadorLogico = varf2->proximo;
        NoPilhaSemantica *varf1 = operadorLogico->proximo;
        NoPilhaSemantica *tipoDeUso = varf1->proximo->proximo;
        if(varf1->token.tipo==2)
        {
            printf("\n%d:ERRO SEMANTICO, Operacao logica com literal nao permitida (Linha %d Coluna %d)\n\n",regra,varf1->token.linha,varf1->token.coluna);
            ErroRR = 1;
            contErroSemantico++;
            break;
        }
        else if(varf2->token.tipo==2)
        {
            printf("\n%d:ERRO SEMANTICO, Operacao logica com literal nao permitida (Linha %d Coluna %d)\n\n",regra,varf2->token.linha,varf2->token.coluna);
            ErroRR = 1;
            contErroSemantico++;
            break;
        }
        else if(varf1->token.tipo!=varf2->token.tipo)
        {
            printf("\n%d:ERRO SEMANTICO, Variaveis %s e %s nao compativeis (Linha %d Coluna %d)\n\n",regra,varf1->token.lexema,varf2->token.lexema,varf2->token.linha,varf2->token.coluna);
            ErroRR = 1;
            contErroSemantico++;
        }
        if(!strcmp(operadorLogico->token.lexema,"="))
        {
            operadorLogico->token.lexema = (char*)calloc(3,sizeof(char));
            strcpy(operadorLogico->token.lexema,"==");
        }
        enfileirarComum(varf1->token.tipo);
        TOKEN novof;
        char *bufferf = (char*)calloc(strlen(varf1->token.lexema)+strlen(varf2->token.lexema)+strlen(operadorLogico->token.lexema)+7,sizeof(char));
        sprintf(bufferf,"T%d = %s%s%s;\n",contadorVariavelTemporararia++,varf1->token.lexema,operadorLogico->token.lexema,varf2->token.lexema);
        if(tipoDeUso->token.indice!=20)
            printarNoArquivo(bufferf);
        else
            atrasoDeEXP_R = strdup(bufferf);
        novof.tipo = varf2->token.tipo;
        novof.lexema = (char*)calloc(10,sizeof(char));
        sprintf(bufferf,"T%d",contadorVariavelTemporararia-1);
        strcat(novof.lexema,bufferf);
        desempilharSemantica();
        desempilharSemantica();
        desempilharSemantica();
        empilharSemantica(novof);
        break;
    case 33:;
        char *bufferfr = (char*)calloc(6,sizeof(char));
        itoa(contadorLabel++,bufferfr,10);
        printarNoArquivo("goto INICIO");
        printarNoArquivo(bufferfr);
        printarNoArquivo(";\n");
        printarNoArquivo("FIM");
        printarNoArquivo(bufferfr);
        printarNoArquivo(":\n");
        break;
    case 34:;
        TOKEN relacionadoRepeticao = pilhaSemantica->topo->proximo->token;
        printarNoArquivo("INICIO");
        char *bufferr = (char*)calloc(6,sizeof(char));
        itoa(contadorLabel,bufferr,10);
        printarNoArquivo(bufferr);
        printarNoArquivo(":\n");
        printarNoArquivo(atrasoDeEXP_R);
        printarNoArquivo("if(!");
        printarNoArquivo(relacionadoRepeticao.lexema);
        printarNoArquivo(") goto FIM");
        printarNoArquivo(bufferr);
        printarNoArquivo(";\n");
        break;
    }
}
