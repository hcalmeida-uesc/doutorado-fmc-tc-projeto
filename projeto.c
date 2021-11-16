#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <math.h>

#define DC 4

/*
* Estrutura para armazenar os dados de entrada
*/
typedef struct{
    double w, LX, LY, DT, G, f, m, Err;
    int n, NX, NY;
} DataIn;

/*
* Estrutura para armazenar os valores de a, b e c
*/

typedef struct{
    double* a;
    double* b; //precisa de um vetor? acho que os valores são sempre os mesmos
    double* c; //precisa de um vetor? acho que os valores são sempre os mesmos
    double* d; //deverá ser um vetor dinâmico
    double* x; //deverá ser um vetor dinâmico
} DataArrays;

/*
*  Função para pegar os dados de entrada
*  Deverá pegar esses dados de um arquivo
*/
DataIn getDataIn(){
    DataIn di;

    di.w = 1;
    di.n = 5;
    di.LX = 4;
    di.LY = 8;
    di.DT = 1;
    di.G = 1;
    di.f = 1.0;
    di.m = 2.0;
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
DataArrays initializeABCD(DataIn in){
    DataArrays da;
    int i, j, tam;
    double aux;

    tam = in.NX*in.NY;

    //alocando os elementos do vetor das diagonais
    da.a = calloc(tam, sizeof(double));
    da.b = calloc(tam-1, sizeof(double));
    da.c = calloc(tam-in.NY, sizeof(double));

    //alocando memória para o vetor d e iniciando com 0
    da.d = calloc(tam, sizeof(double));

    //alocando memória para o vetor x e iniciando com 0
    da.x = calloc(tam, sizeof(double));

    /*
        dúvida: os valores de b e c são sempre iguais? precisa criar vetor?
        Para facilitar a lógica do algoritmo SOR é melhor criar os vetores
    */
    aux = -1/((in.LX/in.NX)*(in.LX/in.NX));
    for(i=0; i<tam-1; i++)
        da.b[i] = aux;
    for(i=in.NX-1; i<tam-1; i+=in.NX)
        da.b[i] = 0.0;

    aux = -1/((in.LY/in.NY)*(in.LY/in.NY));
    for(i=0; i<tam-1; i++)
        da.c[i] = aux;

    //inicializando todos os elementos da diagonal principal iguais a G/DT
    aux = in.G/in.DT;
    for(i=0; i<tam/2+1;i++)
        da.a[i] = aux;



    //primeiro elemento da diagonal principal da matriz.
    da.a[0] += fabs(da.b[0]) + fabs(da.c[0]);
    da.a[tam-1] = da.a[0];


    //da segunda até a (NX-1)-ésima e da (N-NX+2)-ésima até (N-1)-ésima linha
    for(i=1; i<in.NX; i++){
        da.a[i] += fabs(da.b[i-1])+ fabs(da.b[i]) + fabs(da.c[i]);
        da.a[tam-i-1] = da.a[i];
    }

    //restante dos elementos da diagonal principal
    for(i=in.NX; i<tam/2+1; i++){
        da.a[i] += fabs(da.b[i-1])+ fabs(da.b[i]) + 2*fabs(da.c[i]);
        da.a[tam-i-1] = da.a[i];
    }

    //inicializando o vetor d = f
    for(i=0; i<tam; i++)
        da.d[i] = in.f;
    //o elemento do meio ((NXNY-1)/2)+1 deve ser igual f-m
    da.d[((tam-2)/2)+1] = in.f - in.m;

    return da;
}

double* SORModif(DataArrays da, DataIn in){
   /* int i, tam;

    tam = in.n*in.n;

    for(i=0; i<tam; i++)
        da.x[i] = (in.w/da.a[i]);

    //primeira linha do vetor x têm a mesma lógica.
    da.x[0] *= (da.d[0]-(da.b[0]*da.x[1] + da.c[0]*da.x[in.NY]));
    /*
    for(i=1; i<in.NX; i++){
        da.x[i] *= (da.d[i]-(da.b[i-1]*da.x[i-1] + da.c[0]*da.x[in.NY]));
    }

*/
    return da.x;
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

    fprintf(fp, "A%s",separator);
    for(i=0; i<n*n; i++)
        fprintf(fp, "%10d%s",i,separator);

    fprintf(fp, "%s%s%s%s%s%s%s\n","*",separator,"x",separator,"=",separator,"d");

    for(i=0; i<n*n; i++){
        fprintf(fp, "%d%s",i,separator);
        for(j=0; j<n*n; j++){
            if(i==j)
                fprintf(fp, "%10.*lf%s", DC, data.a[i], separator);
            else if(j==i+1)
                fprintf(fp, "%10.*lf%s",DC, data.b[i],separator);
            else if(i==j+1)
                fprintf(fp, "%10.*lf%s",DC, data.b[j],separator);
            else if(i==j+n)
                fprintf(fp, "%10.*lf%s",DC, data.c[j],separator);
            else if(j==i+n)
                fprintf(fp, "%10.*lf%s",DC, data.c[i],separator);
            else
                fprintf(fp, "%s%s"," ",separator);
        }
        fprintf(fp, "%s%s%10.*lf%s%s%s%10.*lf","*",separator,DC,data.x[i],separator,"=",separator,DC,data.d[i]);

        fprintf(fp, "\n");
    }

    fclose(fp);
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
                printf("%.*lf", DC, data.a[i]);
            else if(j==i+1)
                printf("%.*lf%s",DC, data.b[i]);
            else if(i==j+1)
                printf("%.*lf%s",DC, data.b[j]);
            else if(i==j+n)
                printf("%.*lf%s",DC, data.c[j]);
            else if(j==i+n)
                printf("%.*lf%s",DC, data.c[i]);
            else
                printf("%s%s"," ");
        }
        printf("\n\n");
    }

}

int main(void){
    setlocale(LC_ALL, "Portuguese");
    DataIn entrada;
    DataArrays valores_matriz;

    entrada = getDataIn();
    valores_matriz = initializeABCD(entrada);
    //valores_matriz.x = SORModif(valores_matriz,entrada);
    //criando um arquivo CSV na raiz do executável para visualizar a matriz
    generateCSVMatriz(entrada.NX, valores_matriz, "teste2.csv",";");
    //printmatriz(entrada.NX, valores_matriz);

    return 0;

}
