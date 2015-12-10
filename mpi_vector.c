

#include <stdio.h>
#include <math.h>
#include "mpi.h"
#define TAM 4

int main(int argc, char** argv) {
    int meu_ranque;        /* Ranque do meu processo */
    int i, num_procs, etiq = 1, origem = 0;      /* O número de processos */
    float a[TAM][TAM] =    /* Valor a ser enviado*/
        {1.0, 2.0, 3.0, 4.0,
        5.0, 6.0, 7.0, 8.0,
        9.0, 10.0, 11.0, 12.0,
        13.0, 14.0, 15.0, 16.0};
    float b [TAM];          /* Valor a ser recebido */
    int raiz = 0;          /* Processo raiz */

    MPI_Status estado;
    MPI_Datatype coluna;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    MPI_Type_vector(TAM, 1, TAM, MPI_FLOAT, &coluna);
    MPI_Type_commit(&coluna);

    if (num_procs == TAM) {
        if (meu_ranque == 0) {
            for (i = 0; i < num_procs; i++)
                MPI_Send(&a[0][i], 1, coluna, i, etiq, MPI_COMM_WORLD);
        }
        MPI_Recv(b, TAM, MPI_FLOAT, origem, etiq, MPI_COMM_WORLD, &estado);
        printf("Ranque= %d b= %3.1f %3.1f %3.1f %3.1f\n", meu_ranque, b[0], b[1], b[2], b[3]);
    }
    else
        printf("Deve-se especificar %d processadores. Abortando.\n", TAM);

    MPI_Type_free(&coluna);
    MPI_Finalize();
    return 0;
}
