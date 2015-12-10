#include <stdio.h>
#include <math.h>
#include "mpi.h"
#define TAM 4

int main(int argc, char** argv) {
    int meu_ranque;        /* Ranque do meu processo */
    int i, num_procs;      /* O número de processos */
    float a[TAM][TAM] =    /* Valor a ser enviado*/
        {1.0, 2.0, 3.0, 4.0,
        5.0, 6.0, 7.0, 8.0,
        9.0, 10.0, 11.0, 12.0,
        13.0, 14.0, 15.0, 16.0};
    float b [TAM];          /* Valor a ser recebido */
    int raiz = 0;          /* Processo raiz */

    MPI_Datatype linha;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    MPI_Type_contiguous(TAM, MPI_FLOAT,&linha);
    MPI_Type_commit(&linha);

    if (meu_ranque == 0) {
        MPI_Bcast(&a[3][0], 1, linha, raiz, MPI_COMM_WORLD);
    }
    else {
        MPI_Bcast(&b, 1, linha, raiz, MPI_COMM_WORLD);
    }
    /* Imprime o resultado*/
    if (meu_ranque != 0) {
        for (i = 0; i < TAM; i++)
            printf("Processo %d com b[%d] = %4.2f\n", meu_ranque, i,b[i]);
            printf("\n");
    }

    MPI_Finalize();
    return 0;
}
