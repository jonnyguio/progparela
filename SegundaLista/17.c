#include <stdio.h>
#include <string.h>
#include "mpi.h"
#define MAX 32
#define LOCAL_MAX 32
void col_dif_anel(float x[], int tam_bloco, float y[], MPI_Comm com_anel);
void imprime_vetores(char* titulo, float y[], int tam_bloco, int num_procs);

int main(int argc, char** argv) {
	int meu_ranque, num_procs;
	float x[LOCAL_MAX];
	float y[MAX];
	int tam_bloco;
	int i;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    tam_bloco = LOCAL_MAX/num_procs;
    for (i = 0; i < LOCAL_MAX; i++)
        x[i] = (float) meu_ranque;
    if (meu_ranque == 0)
        imprime_vetores("Vetores iniciais processo 0", x, tam_bloco, num_procs);
    col_dif_anel(x, tam_bloco, y, MPI_COMM_WORLD);
    if (meu_ranque == 0)
        imprime_vetores("Vetores coletados processo 0", y, tam_bloco, num_procs);
    MPI_Finalize();
} /* Programa Principal */

void col_dif_anel(float x[], int tam_bloco, float y[], MPI_Comm com_anel ) {
    int i, num_procs, meu_ranque;
    int sucessor, predecessor;
    int offset_envia, offset_recebe;

    MPI_Status estado;
    MPI_Comm_rank(com_anel, &meu_ranque);
    MPI_Comm_size(com_anel, &num_procs);

    /* Copia x na posição correta em y */
    for (i = 0; i < tam_bloco; i++)
        y[i + meu_ranque*tam_bloco] = x[i];

    sucessor = (meu_ranque + 1) % num_procs;
    predecessor = (meu_ranque - 1 + num_procs) % num_procs;

    offset_envia = meu_ranque*tam_bloco;
    offset_recebe = ((meu_ranque - 1 + num_procs) % num_procs) * tam_bloco;
    for (i = 0 ; i < num_procs - 1; i++) {
        MPI_Sendrecv(y + offset_envia, tam_bloco, MPI_FLOAT, sucessor, 0, y + offset_recebe, tam_bloco, MPI_FLOAT, predecessor, 0, com_anel, &estado);
        offset_envia = ((meu_ranque - i - 1 + num_procs) % num_procs) * tam_bloco;
        offset_recebe = ((meu_ranque - i - 2 + num_procs) % num_procs) * tam_bloco;
    }
}

void imprime_vetores(char* titulo, float y[], int tam_bloco, int num_procs) {
    int i;
    printf("%s \n", titulo);
    for (i = 0; i < tam_bloco*num_procs; i++) {
        printf( " %3.1f ", y[i]);
    }
    printf ("\n");
}
