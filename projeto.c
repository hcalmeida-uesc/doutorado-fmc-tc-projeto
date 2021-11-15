#include <stdio.h>
#include <stdlib.h>

#define DC 2

/*
* Estrutura para armazenar os dados de entrada
*/
typedef struct{
    double n, LX, LY, DT, G, f, m, Err;
    int NX, NY;
} DataIn;

/*
* Estrutura para armazenar os valores de a, b e c
*/

typedef struct{
    double* a;
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

    di.n = 6;
    di.LX = 4;
    di.LY = 8;
    di.DT = 1;
    di.G = 0;
    di.f = 0;
    di.m = 0;
    di.Err = 0;
    di.NX = di.n;
    di.NY = di.n;

    return di;
}

/*
*   Função para arredondamento do double 'x' em 'n' casas
*/

double roundCases(double x, int n){
    unsigned int mult = 1;

    while(n > 0){
        mult *= 10;
        n--;
    }

    return round(x*mult)/mult;
}

/*
*   Calcula as diagonais
*/
DataArrays initializeABC(DataIn in){
    DataArrays da;
    int i, j,tam;

    tam = in.NX*in.NX;

    //dúvida: os valores de b e c são sempre iguais? precisa criar vetor?
    da.b = -1/((in.LX/in.NX)*(in.LX/in.NX));
    da.c = -1/((in.LY/in.NY)*(in.LY/in.NY));

    //alocando os elementos do vetor da diagonal principal
    da.a = calloc(tam, sizeof(double));
    //inicializando todos os elementos iguais a G/DT
    for(i=0; i<tam;i++)
        *(da.a+i) = in.G/in.DT;

    //primeira e última linha da matriz têm mesma lógica.
    da.a[0] += fabs(da.b) + fabs(da.c);
    da.a[tam-1] += fabs(da.b) + fabs(da.c);

    da.a[tam-1] = da.a[in.NX-1] = da.a[tam-in.NX] = da.a[0];

    //da segunda até a (NX-1)-ésima e da (N-NX+2)-ésima até (N-1)-ésima linha
    for(i=1, j=tam-2; i<in.NX-1;i++, j--){
        *(da.a+i) += 2*fabs(da.b) + fabs(da.c);
        *(da.a+j) += 2*fabs(da.b) + fabs(da.c);
    }

    //primeiras e últimas linhas dos dois subgrupos restantes
    for(i=in.NX, j=2*in.NX-1; i<tam-in.NX;i+=in.NX, j+=in.NX){
        *(da.a+i) += fabs(da.b) + 2*fabs(da.c);
        *(da.a+j) += fabs(da.b) + 2*fabs(da.c);
    }

    //Restante da linha que são divididas em n-2 grupos
    for(i=1; i<=in.NX-2; i++){
        for(j=(i*in.NX)+1; j<(i+1)*in.NX-1; j++)
            *(da.a+j) += 2*fabs(da.b) + 2*fabs(da.c);
    }

    /*
    for(i=1; i<tam-1;i++){
         *(da.a+i) = i;
    }
    */
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
                fprintf(fp, "%.*lf%s", DC, data.a[j], separator);
            else if(((i==j+1) && (i%(n)!=0)) || ((j==i+1) && (j%(n)!=0)))
                fprintf(fp, "%.*lf%s",DC, data.b,separator);
            else if(i==j+n || j==i+n)
                fprintf(fp, "%.*lf%s",DC, data.c,separator);
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

    printf("%10s"," ");
    for(i=0; i<n*n; i++)
        printf("%8d",i);

    printf("\n\n");
    for(i=0; i<n*n; i++){
        printf("%2d%8s",i," ");
        for(j=0; j<n*n; j++){
            if(i==j)
                printf("%8.lf",data.a[j]);
            else if(((i==j+1) && (i%(n)!=0)) || ((j==i+1) && (j%(n)!=0)))
                printf("%8.lf",data.b);
            else if(i==j+n || j==i+n)
                printf("%8.lf",data.c);
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
    valores_matriz = initializeABC(entrada);

    printf("%lf", roundCases(0.03435423, 2));

    //criando um arquivo CSV na raiz do executável para visualizar a matriz
    generateCSVMatriz(entrada.NX, valores_matriz, "teste.csv",";");
    //printmatriz(entrada.NX, valores_matriz);

    return 0;

}
