#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "PSOR_helder.h"

/*
    Fun��o para pegar os dados de entrada
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

DataIn getDataIn(char* filename){
    DataIn di;
    FILE* fin;

    if((fin=fopen(filename,"r"))==NULL){
        printf("Ero na abertura do arquivo %s",filename);
        exit(-1);
    }

    fscanf(fin,"%d\n", &di.n);
    fscanf(fin,"%lf\n", &di.w);
    fscanf(fin,"%lf\n", &di.LX);
    fscanf(fin,"%lf\n", &di.LY);
    fscanf(fin,"%lf\n", &di.DT);
    fscanf(fin,"%lf\n", &di.G);
    fscanf(fin,"%lf\n", &di.f);
    fscanf(fin,"%lf\n", &di.m);
    fscanf(fin,"%lf", &di.Err);

    fclose(fin);

    di.NX = di.n;
    di.NY = di.n;


    return di;
}


/*
    Fun��o para salvar os dados de sa�da em um arquivo
*/

void saveDataOut(char* filename, DataOut dout, int tam){
    FILE* fout;
    int i;

    if((fout=fopen(filename,"w"))==NULL){
        printf("Ero na cria��o do arquivo %s",filename);
        exit(-1);
    }

    fprintf(fout,"\nErro alcan�ado: %10.8lf\n", dout.absErr);
    fprintf(fout,"Intera��es: %10d\n", dout.interations);
    fprintf(fout,"\nVetor X:\n");
    for(i=0; i<tam; i++){
        fprintf(fout,"x(%2d)=%10.8lf\n",i,dout.xkm1[i]);
    }
    fprintf(fout,"------------------------------");
    fclose(fout);

}

/*
*   Calcula as diagonais
*/
DataArrays initDiagonals(DataIn in){
    DataArrays da;
    int i, j, tam;
    double aux;

    tam = in.NX*in.NY;

    //alocando os elementos do vetor das diagonais
    da.a = calloc(tam, sizeof(double));
    da.b = calloc(tam-1, sizeof(double));
    da.c = calloc(tam-in.NY, sizeof(double));

    //alocando mem�ria para o vetor d e iniciando com 0
    da.d = calloc(tam, sizeof(double));

    /*
        d�vida: os valores de b e c s�o sempre iguais? precisa criar vetor?
        Para facilitar a l�gica do algoritmo SOR � melhor criar os vetores
    */
    aux = -1/((in.LX/in.NX)*(in.LX/in.NX));
    for(i=0; i<tam-1; i++)
        da.b[i] = aux;
    for(i=in.NX-1; i<tam-1; i+=in.NX)
        da.b[i] = 0.0;

    aux = -1/((in.LY/in.NY)*(in.LY/in.NY));
    for(i=0; i<tam-in.NY; i++)
        da.c[i] = aux;

    //inicializando todos os elementos da diagonal principal iguais a G/DT
    aux = in.G/in.DT;
    for(i=0; i<tam/2+1;i++)
        da.a[i] = aux;



    //primeiro elemento da diagonal principal da matriz.
    da.a[0] += fabs(da.b[0]) + fabs(da.c[0]);
    da.a[tam-1] = da.a[0];


    //da segunda at� a (NX-1)-�sima e da (N-NX+2)-�sima at� (N-1)-�sima linha
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

/*
    Fun��o para calcular o erro absoluto
*/

double absoluteError(double* xk, double* xk1, int tam){
    int i;
    double maxErr, err;

    maxErr = fabs(xk1[0]-xk[0]);

    for(i=1; i<tam; i++){
        err = fabs(xk1[1]-xk[1]);

        if(err > maxErr)
            maxErr = err;
    }

    return maxErr;
}


/*
    Algoritmo SOR Modificado para o escopo do projeto
*/
DataOut SORModif(DataArrays da, DataIn in, int maxInterations){
    int i, tam;
    DataOut out;


    //alocando mem�ria para o vetor x(k) e iniciando com 0
    out.xk = calloc(tam, sizeof(double));

    //alocando mem�ria para o vetor x(k+1) e iniciando com 0
    out.xkm1 = calloc(tam, sizeof(double));

    tam = in.n*in.n;

    out.interations = 0;
    while(out.interations++ < maxInterations){

        // salvando os valores de x(k+1) em x(k) para c�lculo do erro da rodada
        for(i=0; i<tam; i++)
            out.xk[i] = out.xkm1[i];

        //c�lculo da primeira linha - x[0](k+1)
        out.xkm1[0] = (in.w/da.a[0])*(da.d[0]-da.b[0]*out.xkm1[1] - da.c[0]*out.xkm1[in.NY])-(in.w-1)*out.xkm1[0];

        //c�lculo da segunda at� (NX-1)-�sima linha - x[1](k+1) : x[NX-1](k+1)
        for(i=1; i<in.NX; i++){
            out.xkm1[i] = (in.w/da.a[i])*(da.d[i]-da.b[i-1]*out.xkm1[i-1] - da.b[i]*out.xkm1[i+1] - da.c[i]*out.xkm1[in.NY+1])-(in.w-1)*out.xkm1[i];
        }

        //c�lculo da (NX)-�sima at� (NX*NY-NX-1)-�sima linha - x[NX](k+1) : x[NX*NY-NX-1](k+1)
        for(; i<tam-in.n; i++)
            out.xkm1[i] = (in.w/da.a[i])*(da.d[i]-da.b[i-1]*out.xkm1[i-1] - da.b[i]*out.xkm1[i+1] - da.c[i-in.NY]*out.xkm1[i-in.NY] - da.c[i]*out.xkm1[in.NY+1])-(in.w-1)*out.xkm1[i];

        //c�lculo da (NX*NY-NX)-�sima at� (NX*NY-2)-�sima linha - x[NX*NY-NX](k+1) : x[NX*NY-2](k+1)
        for(; i<tam-1; i++)
            out.xkm1[i] = (in.w/da.a[i])*(da.d[i]-da.b[i-1]*out.xkm1[i-1] - da.b[i]*out.xkm1[i+1] - da.c[i-in.NY]*out.xkm1[i-in.NY])-(in.w-1)*out.xkm1[i];

        //c�lculo da �ltima linha - x[NX*NY-1](k+1)
        out.xkm1[i] = (in.w/da.a[i])*(da.d[i]-da.b[i-1]*out.xkm1[i-1] - da.c[i-in.NY]*out.xkm1[i-in.NY])-(in.w-1)*out.xkm1[i];

        //calculando o erro e absoluto e saindo do la�o caso tenha atingido
        out.absErr = absoluteError(out.xk, out.xkm1, tam);
        if(out.absErr <= in.Err) break;

    }

    out.interations--;

    return out;
}

/*
*   Para gerar um arquivo CSV com a matriz. Para fins de visualiza��o e teste
*/
void generateCSVMatriz(int n, DataArrays data, DataOut out, char * filename, char * separator){
    FILE *fp;
    int i,j;

    if((fp=fopen(filename,"w"))==NULL){
        printf("Ero na abertura do arquivo %s",filename);
        exit(-1);
    }

    fprintf(fp, "A%s",separator);
    for(i=0; i<n*n; i++)
        fprintf(fp, "%d%s",i,separator);

    fprintf(fp, "%s%s%s%s%s%s%s%s%s\n"," ",separator,"x",separator,"xk",separator," ",separator,"d");

    for(i=0; i<n*n; i++){
        fprintf(fp, "%d%s",i,separator);
        for(j=0; j<n*n; j++){
            if(i==j)
                fprintf(fp, "%.*lf%s", DC, data.a[i], separator);
            else if(j==i+1)
                fprintf(fp, "%.*lf%s",DC, data.b[i],separator);
            else if(i==j+1)
                fprintf(fp, "%.*lf%s",DC, data.b[j],separator);
            else if(i==j+n)
                fprintf(fp, "%.*lf%s",DC, data.c[j],separator);
            else if(j==i+n)
                fprintf(fp, "%.*lf%s",DC, data.c[i],separator);
            else
                fprintf(fp, "%s%s","0",separator);
        }
        fprintf(fp, "%s%s%.*lf%s%.*lf%s%s%s%.*lf"," ",separator,DC,out.xk[i],separator,DC,out.xkm1[i],separator," ",separator,DC,data.d[i]);


        fprintf(fp, "\n");
    }

    fprintf(fp, "\n");
    for(i=0; i<n*n+2; i++)
        fprintf(fp, "%s",separator);

    fprintf(fp, "%s%s%d","Intera��es",separator,out.interations);

    fprintf(fp, "\n");
    for(i=0; i<n*n+2; i++)
        fprintf(fp, "%s",separator);

    fprintf(fp, "%s%s%lf","Erro",separator,out.absErr);

    fclose(fp);
}
