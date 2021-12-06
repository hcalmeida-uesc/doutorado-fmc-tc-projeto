/***********************************************************************
    M�todo SOR para c�lculo de matriz sim�trica e pentadiagonal

    Projeto desenvolvido por H�lder Almeida - hcalmeida@uesc.br
    Para fins de cr�dido no m�dulo de T�cnicas Computacionais
    lecionado pelo prof. Grazione de Souza
    parte da disciplina de Fundamentos de Modelagem Computacional

    Per�odo: 2021.2

    ARQUIVO projeto.c - fun��o main
***********************************************************************/
#include <stdlib.h>
#include <locale.h>

//a biblioteca criada para o projeto
#include "PSOR_helder.h"

int main(void){
    //para usar a ',' ao inv�s de '.' no decimal. Inclusive no arquivo de entrada
    setlocale(LC_ALL, "Portuguese");
    DataIn entrada;
    DataOut saida;
    DataArrays valores_matriz;
    char filename[255] = "entrada.txt";

    printf("---------------------------------------------------------------------\n");
    printf("    M�todo SOR para c�lculo de matriz sim�trica e pentadiagonal.     \n");
    printf("    Projeto desenvolvido por H�lder Almeida - hcalmeida@uesc.br      \n");
    printf("    Para fins de cr�dido no m�dulo de T�cnicas Computacionais        \n");
    printf("    lecionado pelo prof. Grazione de Souza                           \n");
    printf("    parte da disciplina de Fundamentos de Modelagem Computacional    \n");
    printf("    Per�odo: 2021.2                                                  \n");
    printf("---------------------------------------------------------------------\n");

    printf("\n --- Etapa 1 de 4 (Entrada de dados) --- \n");
    printf("Informe o nome do arquivo de entrada\n");
    printf("Tecle enter para aceitar 'entrada.txt': ");
    gets(filename);
    if(filename[0]=='\0')
        strcpy(filename,"entrada.txt");

    entrada = getDataIn(filename);

    printf("\n --- Etapa 2 de 4 (Cria��o dos vetores) --- \n");
    valores_matriz = initDiagonals(entrada);


    printf("\n --- Etapa 3 de 4 (Calculando vetor x) --- \n");
    saida = SORModif(valores_matriz,entrada);

    printf("\n --- Etapa 4 de 4 (Salvando a sa�da) --- \n");
    printf("Informe o nome do arquivo de sa�da\n");
    printf("Tecle enter para aceitar 'saida.txt': ");
    gets(filename);
    if(filename[0]=='\0')
        strcpy(filename,"saida.txt");

    saveDataOut(filename,saida);

    printf("\nDeseja exportar a matriz para um arquivo .csv?\n");
    printf("(s-sim / n-n�o): ");
    gets(filename);
    if(filename[0]!='n' || filename[0]=='N'){
        printf("Informe o nome do arquivo csv\n");
        printf("Tecle enter para aceitar 'saida.csv': ");
        gets(filename);
        if(filename[0]=='\0')
            strcpy(filename,"saida.csv");
        generateCSVMatriz(valores_matriz, saida, filename,";");
    }

    printf("\n --- Fim da Execu��o - OBRIGADO! --- \n");
    getchar();

    return 0;

}
