/***********************************************************************
    Método SOR para cálculo de matriz simétrica e pentadiagonal

    Projeto desenvolvido por Hélder Almeida - hcalmeida@uesc.br
    Para fins de crédido no módulo de Técnicas Computacionais
    lecionado pelo prof. Grazione de Souza
    parte da disciplina de Fundamentos de Modelagem Computacional

    Período: 2021.2
    ARQUIVO PSOR_helder.c - biblioteca com as funções implementadas
***********************************************************************/

#ifndef PSOR_HELDER
#define PSOR_HELDER

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//casas decimais dos arquivos de saída
#define DC 8
//máximo de interações até o algoritmo interromper a execução
#define MAX_INTERATIONS 100

/**
    Estrutura DataIn para armazenar os dados de entrada
**/
typedef struct{
    double  w,      ///parâmetro de relaxação
            LX,     ///utilizado para calcular a diagonal b
            LY,     ///utilizado para calcular a diagonal c
            DT,     ///utilizado para calcular a diagonal a
            G,      ///utilizado para calcular a diagonal a
            f,      ///utilizado para calcular o vetor d
            m,      ///utilizado para calcular o vetor d
            Err;    ///erro absoluto aceitável (tolerância)
    int     n,      ///ordem da matriz
            NX,     ///igual a n, utilizado para calcular a diagonal b
            NY;     ///igual a n, utilizado para calcular a diagonal c
} DataIn;

/**
    Estrutura DataOut para armazenar os dados de saída
**/
typedef struct{
    int tam;        ///dimensão dos vetores x
    int n;          ///onde tam = n*n
    double absErr;  ///erro absoluto calculado
    int interations;///quantidade máxima de interações antes de abortar
    double* xk;     ///vetor x(k-1) guardando a última interação
    double* xkm;    ///vetor x(k) com interação em processamento
} DataOut;

/**
    Estrutura DataArrays para armazenar os vetores a, b, c e d
**/

typedef struct{
    double* a; ///vetor da diagonal principal
    double* b; ///vetor da diagonal b
    double* c; ///vetor da diagonal c
    double* d; ///vetor de resultados d
} DataArrays;

/**
    Parâmetro: string - nome do arquivo de entrada

    Retorno: DataIn - estrutura preenchida

    Objetivo:Função para pegar os dados de entrada
    de um arquivo, um por linha, na ordem:

        n           - inteiro
        w(omega)    - double
        LX          - double
        LY          - double
        DT          - double
        G           - double
        f           - double
        m           - double
        Err(erro)   - double

**/
DataIn getDataIn(char* filename){
    DataIn di;
    FILE *fin;

    if((fin=fopen(filename,"r"))==NULL){
        printf("Erro na abertura do arquivo %s",filename);
        exit(-1);
    }

    fscanf(fin,"%*s %d", &di.n);
    fscanf(fin,"%*s %lf", &di.w);
    fscanf(fin,"%*s %lf", &di.LX);
    fscanf(fin,"%*s %lf", &di.LY);
    fscanf(fin,"%*s %lf", &di.DT);
    fscanf(fin,"%*s %lf", &di.G);
    fscanf(fin,"%*s %lf", &di.f);
    fscanf(fin,"%*s %lf", &di.m);
    fscanf(fin,"%*s %lf", &di.Err);

    fclose(fin);

    di.NX = di.n;
    di.NY = di.n;

    if(validateDataIn(di)){

    }else{
        printf("\n\nEXECUÇÃO INTERROMPIDA!\nCORRIJA O ");
        printf("ARQUIVO '%s' E TENTE NOVAMENTE!\n", filename);
        getchar();
        exit(-1);
    }


    return di;
}


/**
    Parâmetro: DataIn - dados de entrada

    Retorno: verdadeiro - caso os dados estejam corretos
             falso - caso os dados de entrada estejam com problema

    Objetivo: Função para validar os dados de entrada segundo
              especificação do trabalho
**/
int validateDataIn(DataIn di){
    int erro = 0;

    printf("\nDados de entrada:\n");



    printf("NX= %d", di.NX);
    if( di.NX < 3 || di.NX % 2 != 1 ){
        printf(" -> Erro: NX deve ser inteiro, ímpar e >= 3\n");
        erro = 1;
    }else{
        printf(" -> Ok\n");
    }

    printf("NY= %d", di.NY);
    if( di.NY < 3 || di.NY % 2 != 1 ){
        printf(" -> Erro: NY deve ser inteiro, ímpar e >= 3\n");
        erro = 1;
    }else{
        printf(" -> Ok\n");
    }

    printf("w= %*lf", DC, di.w);
    if( di.w < 1 || di.w > 2 ){
        printf(" -> Erro: w deve estar entre 1 e 2\n");
        erro = 1;
    }else{
        printf(" -> Ok\n");
    }

    printf("LX= %*lf", DC, di.LX);
    if( di.LX <= 0 ){
        printf(" -> Erro: LX deve ser positivo\n");
        erro = 1;
    }else{
        printf(" -> Ok\n");
    }

    printf("LY= %*lf", DC, di.LY);
    if( di.LY <= 0 ){
        printf(" -> Erro: LY deve ser positivo\n");
        erro = 1;
    }else{
        printf(" -> Ok\n");
    }

    printf("DT= %*lf", DC, di.DT);
    if( di.DT <= 0 ){
        printf(" -> Erro: DT deve ser positivo\n");
        erro = 1;
    }else{
        printf(" -> Ok\n");
    }

    printf("G= %*lf", DC, di.G);
    if( di.G <= 0 ){
        printf(" -> Erro: G deve ser positivo\n");
        erro = 1;
    }else{
        printf(" -> Ok\n");
    }

    printf("f= %*lf", DC, di.f);
    if( di.f <= 0 ){
        printf(" -> Erro: f deve ser positivo\n");
        erro = 1;
    }else{
        printf(" -> Ok\n");
    }

    printf("m= %*lf", DC, di.m);
    if( di.m <= 0 ){
        printf(" -> Erro: m deve ser positivo\n");
        erro = 1;
    }else{
        printf(" -> Ok\n");
    }

    printf("Erro= %*lf", DC, di.Err);
    if( di.Err <= 0 ){
        printf(" -> Erro: erro deve ser positivo\n");
        erro = 1;
    }else{
        printf(" -> Ok\n");
    }


    return (!erro);

}

/**
    Parâmetro:  DataIn - dados de entrada

    Retorno: DataArrays - vetores com diagonais (a, b e c)
                          e o vetor respostas d

    Objetivo: Calcula as diagonais de acordo com a especificação
**/
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
        Inicializando o vetor b , segundo especificação
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

/**
    Parâmetro:  DataOut* - referência para osdados de saída

    Retorno: double - vetores com diagonais (a, b e c)
                      e o vetor respostas d

    Objetivo: Função para calcular o erro absoluto do vetor xk
**/

double absoluteError(DataOut *out){
    int i;
    double maxErr, err;

    maxErr = fabs(out->xkm[0]-out->xk[0]);

    for(i=1; i<out->tam; i++){
        err = fabs(out->xkm[i]-out->xk[i]);

        if(err > maxErr)
            maxErr = err;
    }

    out->absErr = maxErr;

    return maxErr;
}


/**
    Parâmetro:  DataArrays - vetores com diagonais e vetor d
                DataIn - dados de entrada

    Retorno: DataOut - vetor xk calculado pelo método SOR
                       e demais dados dos resultados de saída,
                       como número de interações e erro alcançado

    Objetivo: Algoritmo SOR Modificado para o escopo do projeto

**/
DataOut SORModif(DataArrays da, DataIn in){
    int i;
    DataOut out;
    double xk;

    out.n = in.n;
    out.tam = in.n*in.n;

    //alocando memória para o vetor x(k) e iniciando com 0
    out.xk = calloc(out.tam, sizeof(double));

    //alocando memória para o vetor x(k+1) e iniciando com 0
    out.xkm = calloc(out.tam, sizeof(double));


    out.interations = 0;
    while(out.interations < MAX_INTERATIONS){

        // salvando os valores de x(k+1) em x(k) para cálculo do erro da rodada
        for(i=0; i<out.tam; i++)
            out.xk[i] = out.xkm[i];

        //cálculo da primeira linha da matriz - x[0](k+1)
        out.xkm[0] = ((1-in.w)*out.xk[0])+(in.w/da.a[0])*(da.d[0] - (da.b[0]*out.xk[1] + da.c[0]*out.xk[in.NY]));

        //cálculo da segunda até (NX-1)-ésima linha - x[1](k+1) : x[NX-1](k+1)
        for(i=1; i<in.n; i++){
            out.xkm[i] = ((1-in.w)*out.xk[i])+(in.w/da.a[i])*(da.d[i] - da.b[i-1]*out.xkm[i-1] - (da.b[i]*out.xk[i+1] + da.c[i]*out.xk[in.NY+i]));
        }

        //cálculo da (NX)-ésima até (NX*NY-NX-1)-ésima linha - x[NX](k+1) : x[NX*NY-NX-1](k+1)
        for(; i<out.tam-in.n; i++)
            out.xkm[i] = ((1-in.w)*out.xk[i])+(in.w/da.a[i])*(da.d[i] - (da.c[i-in.NY]*out.xkm[i-in.NY]+da.b[i-1]*out.xkm[i-1]) - (da.b[i]*out.xk[i+1]+da.c[i]*out.xk[in.NY+i]));

        //cálculo da (NX*NY-NX)-ésima até (NX*NY-2)-ésima linha - x[NX*NY-NX](k+1) : x[NX*NY-2](k+1)
        for(; i<out.tam-1; i++)
            out.xkm[i] = ((1-in.w)*out.xk[i])+(in.w/da.a[i])*(da.d[i] - (da.c[i-in.NY]*out.xkm[i-in.NY]+da.b[i-1]*out.xkm[i-1]) - da.b[i]*out.xk[i+1] );

        //cálculo da última linha - x[NX*NY-1](k+1)
        out.xkm[i] = ((1-in.w)*out.xk[i])+(in.w/da.a[i])*(da.d[i] - (da.c[i-in.NY]*out.xkm[i-in.NY]+da.b[i-1]*out.xkm[i-1])) ;

        out.interations++;
        //calculando o erro e absoluto e saindo do laço caso tenha atingido
        absoluteError(&out);
        if(out.absErr <= in.Err) break;

    }

    return out;
}

/**
    Parâmetro:  string - nome do arquivo
                DataOut - dados para serem salvos no arquivo
    Retorno: sem retorno

    Objetivo: Função para salvar os dados de saída em um arquivo
**/
void saveDataOut(char* filename, DataOut dout){
    FILE* fout;
    int i,j;

    if((fout=fopen(filename,"w"))==NULL){
        printf("Erro na criação do arquivo %s",filename);
        exit(-1);
    }

    fprintf(fout,"\nErro alcançado: %10.*lf\n",DC, dout.absErr);
    fprintf(fout,"Interações: %10d\n", dout.interations);
    fprintf(fout,"\nVetor X:\n");
    for(i=0; i<dout.tam; i++){
        fprintf(fout,"x[%03d] = %15.*lf\n",i,DC,dout.xkm[i]);
    }
    fprintf(fout,"------------------------------");

    fprintf(fout,"\nVetor X em matriz:\n");

    for(i=0; i<dout.n; i++){
        for(j=0; j<dout.n; j++)
            fprintf(fout,"%15.*lf",DC,dout.xkm[i*dout.n+j]);
        fprintf(fout,"\n");
    }
    fclose(fout);

}


/**
    Parâmetro:  DataArrays - vetores com diagonais (a, b e c) e vetor d
                DataOut - dados de saída calculados pelo SOR
                int - ordem da matriz a ser impressa
                string - nome do arquivo csv
                string - separados de colunas do csv

    Retorno: sem retorno

    Objetivo: Gerar um arquivo CSV com a matriz.
              Para fins de visualização e testes.
**/
void generateCSVMatriz(DataArrays data, DataOut out, char * filename, char * separator){
    FILE *fp;
    int i,j;

    if((fp=fopen(filename,"w"))==NULL){
        printf("Ero na abertura do arquivo %s",filename);
        exit(-1);
    }

    fprintf(fp, "A%s",separator);
    for(i=0; i<out.tam; i++)
        fprintf(fp, "%d%s",i,separator);

    fprintf(fp, "%s%s%s%s%s%s%s%s%s\n"," ",separator,"x",separator,"xk",separator," ",separator,"d");

    for(i=0; i<out.tam; i++){
        fprintf(fp, "%d%s",i,separator);
        for(j=0; j<out.tam; j++){
            if(i==j)
                fprintf(fp, "%.*lf%s", DC, data.a[i], separator);
            else if(j==i+1)
                fprintf(fp, "%.*lf%s",DC, data.b[i],separator);
            else if(i==j+1)
                fprintf(fp, "%.*lf%s",DC, data.b[j],separator);
            else if(i==j+out.n)
                fprintf(fp, "%.*lf%s",DC, data.c[j],separator);
            else if(j==i+out.n)
                fprintf(fp, "%.*lf%s",DC, data.c[i],separator);
            else
                fprintf(fp, "%s%s","0",separator);
        }
        fprintf(fp, "%s%s%.*lf%s%.*lf%s%s%s%.*lf"," ",separator,DC,out.xk[i],separator,DC,out.xkm[i],separator," ",separator,DC,data.d[i]);


        fprintf(fp, "\n");
    }

    fprintf(fp, "\n");
    for(i=0; i<out.tam+2; i++)
        fprintf(fp, "%s",separator);

    fprintf(fp, "%s%s%d","Interações",separator,out.interations);

    fprintf(fp, "\n");
    for(i=0; i<out.tam+2; i++)
        fprintf(fp, "%s",separator);

    fprintf(fp, "%s%s%lf","Erro",separator,out.absErr);

    fclose(fp);
}
#endif // PSOR_HELDER
