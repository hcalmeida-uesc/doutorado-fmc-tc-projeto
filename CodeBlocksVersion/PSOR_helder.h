#ifndef PSOR_HELDER_H_INCLUDED
#define PSOR_HELDER_H_INCLUDED

//casas decimais
#define DC 8

/*
    Estrutura para armazenar os dados de entrada
*/
typedef struct{
    double w, LX, LY, DT, G, f, m, Err;
    int n, NX, NY;
} DataIn;

/*
    Estrutura para armazenar os dados de saída
*/
typedef struct{
    double absErr;
    int interations;
    double* xk; //vetor x(k) com chute inicial
    double* xkm1; //vetor x(k+1) com cada aproximação
} DataOut;

/*
    Estrutura para armazenar os valores de a, b e c
*/

typedef struct{
    double* a; //vetor da diagonal principal
    double* b; //vetor da diagonal b
    double* c; //vetor da diagonal c
    double* d; //vetor de resultados d
} DataArrays;

/*
    Função para pegar os dados de entrada
    de um arquivo, um por linha, na ordem:
    n           - inteiro
    w(omega)    - double
    LX          - couble
    LY          - couble
    DT          - couble
    G           - couble
    f           - couble
    m           - couble
    E (erro)    - couble

*/
DataIn getDataIn(char*);

/*
    Função para salvar os dados de saída em um arquivo
*/
void saveDataOut(char*, DataOut, int);

/*
*   Calcula as diagonais
*/
DataArrays initDiagonals(DataIn);

/*
    Função para calcular o erro absoluto
*/
double absoluteError(double*, double*, int);

/*
    Algoritmo SOR Modificado para o escopo do projeto
*/
DataOut SORModif(DataArrays, DataIn, int);

/*
*   Para gerar um arquivo CSV com a matriz. Para fins de visualização e teste
*/
void generateCSVMatriz(int, DataArrays, DataOut, char*, char*);

#endif // PSOR_HELDER_H_INCLUDED
