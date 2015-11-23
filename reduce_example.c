#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define TAM 5
int main(int argc, char ** argv) {
    int meu_ranque; /* O ranque do meu processo      */
    int i, num_procs;   /* O número de processos        */
    float a [TAM];     /* Valor a ser enviado          */
    float b [TAM]; /* Valor a ser recebido         */
    int origem; /* Processo enviando a integral  */
    int destino = 0; /* Todas as mensagens vão para 0 */
    int etiq = 50;
    MPI_Status info;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    /* Preenche o vetor com valores que dependem do ranque */
    for  (i = 0; i < TAM; i++)
        a[i] = (float) (meu_ranque*TAM+i);
    /* Faz a redução, encontrando o valor máximo do vetor */
    MPI_Reduce(&a, &b, TAM, MPI_FLOAT, MPI_MAX, destino,MPI_COMM_WORLD);
    /* Imprime o resultado*/
    if (meu_ranque == destino) {
        for (i = 0; i < TAM; i++)
            printf("Com b[%d] = %f ", i, b[i]);
        printf("\n\n");
    }
    MPI_Finalize();
}
