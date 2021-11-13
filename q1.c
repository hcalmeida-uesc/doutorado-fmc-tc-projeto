/*
EXAME DE QUALIFICAÇÃO – TÉCNICAS COMPUTACIONAIS (IPRJ/UERJ) - 25/06/2021
Questão 1
*/
#include <stdio.h>
#include <stdlib.h>

struct node{
    int info;
    struct node *prox;
};
typedef struct node ELEMENTO;

/*
 A resposta para a questão é apenas essa função somaimpares!
 O código restante é para demonstrar o funcionamento.
*/
int somaimpares(ELEMENTO * p, int x){
    int soma = p->info;
    ELEMENTO * aux;
    aux = p->prox;
    while(aux != p){
        if(aux->info%2 != 0)
            soma += aux->info;
        aux=aux->prox;
    }
    return (soma == x) ? 1 : 0;
}
// Fim da resposta da q1

ELEMENTO * initialize(int val){
    ELEMENTO *aux;

    aux = malloc(sizeof(ELEMENTO*));
    aux->info = val;
    aux->prox = aux;

    return aux;
}

void insert(ELEMENTO * lista, int val){
    ELEMENTO *aux;

    aux = initialize(val);
    aux->prox = lista->prox;
    lista->prox = aux;
}

int main(){
    ELEMENTO *last;

    last = initialize(5);

    insert(last,7);
    insert(last,3);
    insert(last,2);
    insert(last,1);

    printf("%d",somaimpares(last,16));

    return 0;
}
