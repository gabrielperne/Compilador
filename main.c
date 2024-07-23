#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"
#include "parcer.h"

int main() {
    FILE* arquivoScanner = fopen("codigo_fonte.txt", "r");
    if (arquivoScanner == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return EXIT_FAILURE;
    }
    FILE* tabela = fopen("tabela.csv","r");
    iniciarSintatico(tabela);
    arquivo = arquivoScanner;
    TOKEN token;
    /*do {
        token = scanner();
        if (strcmp(token.classe, "ERRO") != 0 && strcmp(token.classe, "EOF") != 0) {
            printf("Classe: %s, Lexema: %s, Tipo: %s\n", token.classe, token.lexema, token.tipo);
        }
    } while (strcmp(token.classe, "EOF") != 0);*/
    parser();
    //imprimir_tabela_simbolos();

    fclose(arquivoScanner);
    return EXIT_SUCCESS;
}
