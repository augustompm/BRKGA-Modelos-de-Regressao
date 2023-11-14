#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <vector>
#define N 150000000 //matriz N x N
typedef double numero; //funçao que preenche a matriz

using namespace std;

void encher(vector<numero>* a,vector<numero>* b) { 
    int i;
    //#pragma omp parallel for private(i)
    for (i=0;i<N;i++) { 
        (*a).push_back((numero)i+1);
        (*b).push_back((numero)N-i); 
    } 
}

numero calcular(vector<numero>* a,vector<numero>* b) { 
    int i;
    numero resul=0;
   #pragma omp parallel for private(i) shared (a,b) reduction(+:resul)
        //#pragma omp for 
        for (i=0;i<N;i++){
            resul+=(*a)[i]*(*b)[i];
        }
    return resul; 
} 
//funçao de imprimir uma matriz

void imprimir(vector<numero> c) { 
    int i;
    for (i=0;i<N;i++)
        printf("%10.f\t",c[i]);
    printf("\n");
} 

int main() { 
    //inicializando e alocando variaveis
    vector<numero> a;
    vector<numero> b;

    //numero* a;
    //numero* b;
    numero result;
    //a=(numero*)malloc(N*sizeof(numero));
    //b=(numero*)malloc(N*sizeof(numero)); 
    // fim 
    //chamando funçoes auxiliares
    omp_set_num_threads(6);
    //printf("Chegou\n");
    encher(&a,&b);
    //printf("Chegou\n");
    //imprimir(a);
    //imprimir(&b);
    result=calcular(&a,&b);
    printf("produto escalar: %23.f\n",result);
    // fim 
    // desalocando variaveis 
    //free(a);
    //free(b);
    //fim 
    return 0;
}