#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <math.h>

//casas decimais
#define DC 20

/*
* Estrutura para armazenar os dados de entrada
*/
typedef struct{
    double w, LX, LY, DT, G, f, m, Err;
    int n, NX, NY;
} DataIn;

typedef struct{
    double absErr;
    int interations;
    double* xk; //vetor x(k) com chute inicial
    double* xkm1; //vetor x(k+1) com cada aproximação
} DataOut;

/*
* Estrutura para armazenar os valores de a, b e c
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
    Função para salvar os dados de saída em um arquivo
*/

void saveDataOut(char* filename, DataOut dout, int tam){
    FILE* fout;
    int i;

    if((fout=fopen(filename,"w"))==NULL){
        printf("Ero na criação do arquivo %s",filename);
        exit(-1);
    }

    fprintf(fout,"\nErro alcançado: %10.8lf\n", dout.absErr);
    fprintf(fout,"Interações: %10d\n", dout.interations);
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

    //alocando memória para o vetor d e iniciando com 0
    da.d = calloc(tam, sizeof(double));

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
    for(i=0; i<tam-in.NY; i++)
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

/*
    Função para calcular o erro absoluto
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


    //alocando memória para o vetor x(k) e iniciando com 0
    out.xk = calloc(tam, sizeof(double));

    //alocando memória para o vetor x(k+1) e iniciando com 0
    out.xkm1 = calloc(tam, sizeof(double));

    tam = in.n*in.n;

    out.interations = 0;
    while(out.interations++ < maxInterations){

        // salvando os valores de x(k+1) em x(k) para cálculo do erro da rodada
        for(i=0; i<tam; i++)
            out.xk[i] = out.xkm1[i];

        //cálculo da primeira linha - x[0](k+1)
        out.xkm1[0] = (in.w/da.a[0])*(da.d[0]-da.b[0]*out.xkm1[1] - da.c[0]*out.xkm1[in.NY])-(in.w-1)*out.xkm1[0];

        //cálculo da segunda até (NX-1)-ésima linha - x[1](k+1) : x[NX-1](k+1)
        for(i=1; i<in.NX; i++){
            out.xkm1[i] = (in.w/da.a[i])*(da.d[i]-da.b[i-1]*out.xkm1[i-1] - da.b[i]*out.xkm1[i+1] - da.c[i]*out.xkm1[in.NY+1])-(in.w-1)*out.xkm1[i];
        }

        //cálculo da (NX)-ésima até (NX*NY-NX-1)-ésima linha - x[NX](k+1) : x[NX*NY-NX-1](k+1)
        for(; i<tam-in.n; i++)
            out.xkm1[i] = (in.w/da.a[i])*(da.d[i]-da.b[i-1]*out.xkm1[i-1] - da.b[i]*out.xkm1[i+1] - da.c[i-in.NY]*out.xkm1[i-in.NY] - da.c[i]*out.xkm1[in.NY+1])-(in.w-1)*out.xkm1[i];

        //cálculo da (NX*NY-NX)-ésima até (NX*NY-2)-ésima linha - x[NX*NY-NX](k+1) : x[NX*NY-2](k+1)
        for(; i<tam-1; i++)
            out.xkm1[i] = (in.w/da.a[i])*(da.d[i]-da.b[i-1]*out.xkm1[i-1] - da.b[i]*out.xkm1[i+1] - da.c[i-in.NY]*out.xkm1[i-in.NY])-(in.w-1)*out.xkm1[i];

        //cálculo da última linha - x[NX*NY-1](k+1)
        out.xkm1[i] = (in.w/da.a[i])*(da.d[i]-da.b[i-1]*out.xkm1[i-1] - da.c[i-in.NY]*out.xkm1[i-in.NY])-(in.w-1)*out.xkm1[i];

        //calculando o erro e absoluto e saindo do laço caso tenha atingido
        out.absErr = absoluteError(out.xk, out.xkm1, tam);
        if(out.absErr <= in.Err) break;

    }

    out.interations--;

    return out;
}

/*
*   Para gerar um arquivo CSV com a matriz. Para fins de visualização e teste
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

    fprintf(fp, "%s%s%d","Interações",separator,out.interations);

    fprintf(fp, "\n");
    for(i=0; i<n*n+2; i++)
        fprintf(fp, "%s",separator);

    fprintf(fp, "%s%s%lf","Erro",separator,out.absErr);

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
