#include <stdio.h>
#include <stdlib.h>

/*
* Estrutura para armazenar os dados de entrada
*/
typedef struct{
    double LX, LY, DT, G, f, m, Err;
    int NX, NY;
} DataIn;

/*
* Estrutura para armazenar os valores de a, b e c
*/

typedef struct{
    double a; //deverá ser um vetor dinâmico
    double b; //precisa de um vetor? acho que os valores são sempre os mesmos
    double c; //precisa de um vetor? acho que os valores são sempre os mesmos
    double d; //deverá ser um vetor dinâmico
} DataArrays;

/*
*  Função para pegar os dados de entrada
*  Deverá pegar esses dados de um arquivo
*/
DataIn getDataIn(){
    DataIn di;

    di.LX = 3;
    di.LY = 3;
    di.DT = 1;
    di.G = 1;
    di.f = 0;
    di.m = 0;
    di.Err = 0;
    di.NX = 4;
    di.NY = 4;

    return di;
}

/*
*   Calcula as diagonais
*/
DataArrays caculateData(DataIn in){
    DataArrays da;

    //dúvida: os valores de b e c são sempre iguais? precisa criar vetor?
    da.b = -1/((in.LX/in.NX)*(in.LX/in.NX));
    da.c = -1/((in.LY/in.NY)*(in.LY/in.NY));
    da.a = 1; //todo
    //da.b = 2;
    //da.c = 3;

    return da;
}

/*
*   Para gerar um arquivo CSV com a matriz. Para fins de visualização e teste
*/
void generateCSVMatriz(int n, DataArrays data, char * filename, char * separator){
    FILE *fp;
    int i,j;

    if((fp=fopen(filename,"w"))==NULL){
        printf("Ero na abertura do arquivo %s",filename);
        exit(-1);
    }

    fprintf(fp, " %s",separator);
    for(i=0; i<n*n; i++)
        fprintf(fp, "%d%s",i,separator);

    fprintf(fp, "\n");
    for(i=0; i<n*n; i++){
        fprintf(fp, "%d%s",i,separator);
        for(j=0; j<n*n; j++){
            if(i==j)
                fprintf(fp, "%.1lf%s",data.a,separator);
            else if(((i==j+1) && (i%(n)!=0)) || ((j==i+1) && (j%(n)!=0)))
                fprintf(fp, "%.1lf%s",data.b,separator);
            else if(i==j+n || j==i+n)
                fprintf(fp, "%.1lf%s",data.c,separator);
            else
                fprintf(fp, "%s%s"," ",separator);
        }
        fprintf(fp, "\n");
    }

}

/*
*   Imprime a matriz no prompt
*/
void printmatriz(int n, DataArrays data){
    int i,j;

    printf("      ");
    for(i=0; i<n*n; i++)
        printf("%8d",i);

    printf("\n\n");
    for(i=0; i<n*n; i++){
        printf("%2d%8s",i," ");
        for(j=0; j<n*n; j++){
            if(i==j)
                printf("%8s","a");
            else if(((i==j+1) && (i%(n)!=0)) || ((j==i+1) && (j%(n)!=0)))
                printf("%8.1lf",data.b);
            else if(i==j+n || j==i+n)
                printf("%8.1lf",data.c);
            else
                printf("%8s","0");
        }
        printf("\n\n");
    }

}

int main(void){

    DataIn entrada;
    DataArrays valores_matriz;

    entrada = getDataIn();
    valores_matriz = caculateData(entrada);

    //criando um arquivo CSV na raiz do executável para visualizar a matriz
    generateCSVMatriz(entrada.NX, valores_matriz, "teste.csv",";");

    return 0;

}
