/***********************************************************************
    Método SOR para cálculo de matriz simétrica e pentadiagonal

    Projeto desenvolvido por Hélder Almeida - hcalmeida@uesc.br
    Para fins de crédido no módulo de Técnicas Computacionais
    lecionado pelo prof. Grazione de Souza
    parte da disciplina de Fundamentos de Modelagem Computacional

    Período: 2021.2

    ARQUIVO projeto.c - função main
***********************************************************************/
#include <stdlib.h>
#include <locale.h>

//a biblioteca criada para o projeto
#include "PSOR_helder.h"

int main(void){
    //para usar a ',' ao invés de '.' no decimal. Inclusive no arquivo de entrada
    setlocale(LC_ALL, "Portuguese");
    DataIn entrada;
    DataOut saida;
    DataArrays valores_matriz;
    char filename[255] = "entrada.txt";

    printf("---------------------------------------------------------------------\n");
    printf("    Método SOR para cálculo de matriz simétrica e pentadiagonal.     \n");
    printf("    Projeto desenvolvido por Hélder Almeida - hcalmeida@uesc.br      \n");
    printf("    Para fins de crédido no módulo de Técnicas Computacionais        \n");
    printf("    lecionado pelo prof. Grazione de Souza                           \n");
    printf("    parte da disciplina de Fundamentos de Modelagem Computacional    \n");
    printf("    Período: 2021.2                                                  \n");
    printf("---------------------------------------------------------------------\n");

    printf("\n --- Etapa 1 de 4 (Entrada de dados) --- \n");
    printf("Informe o nome do arquivo de entrada\n");
    printf("Tecle enter para aceitar 'entrada.txt': ");
    gets(filename);
    if(filename[0]=='\0')
        strcpy(filename,"entrada.txt");

    entrada = getDataIn(filename);

    printf("\n --- Etapa 2 de 4 (Criação dos vetores) --- \n");
    valores_matriz = initDiagonals(entrada);


    printf("\n --- Etapa 3 de 4 (Calculando vetor x) --- \n");
    saida = SORModif(valores_matriz,entrada);

    printf("\n --- Etapa 4 de 4 (Salvando a saída) --- \n");
    printf("Informe o nome do arquivo de saída\n");
    printf("Tecle enter para aceitar 'saida.txt': ");
    gets(filename);
    if(filename[0]=='\0')
        strcpy(filename,"saida.txt");

    saveDataOut(filename,saida);

    printf("\nDeseja exportar a matriz para um arquivo .csv?\n");
    printf("(s-sim / n-não): ");
    gets(filename);
    if(filename[0]!='n' || filename[0]=='N'){
        printf("Informe o nome do arquivo csv\n");
        printf("Tecle enter para aceitar 'saida.csv': ");
        gets(filename);
        if(filename[0]=='\0')
            strcpy(filename,"saida.csv");
        generateCSVMatriz(valores_matriz, saida, filename,";");
    }

    printf("\n --- Fim da Execução - OBRIGADO! --- \n");
    getchar();

    return 0;

}
