/*
        Projeto desenvolvido por Hélder Almeida
        Para fins de crédido no módulo de Técnicas Computacionais
        da disciplina de Fundamentos de Modelagem Computacional
        lecionada pelo prof. Grazione de Souza

        Período: 2021.2
*/
#include <stdlib.h>
#include <locale.h>

//a biblioteca criada para o projeto
#include "PSOR_helder.h"



int main(void){
    setlocale(LC_ALL, "Portuguese"); //para usar a , ao invés de . no decimal. Inclusive no arquivo de entrada
    DataIn entrada;
    DataOut saida;
    DataArrays valores_matriz;

    entrada = getDataIn("entrada.txt");
    valores_matriz = initDiagonals(entrada);
    saida = SORModif(valores_matriz,entrada,50);
    saveDataOut("saida.txt",saida,entrada.n*entrada.n);
    //criando um arquivo CSV na raiz do executável para visualizar a matriz
    generateCSVMatriz(entrada.NX, valores_matriz, saida, "teste.csv",";");
    //printmatriz(entrada.NX, valores_matriz);

    return 0;

}
