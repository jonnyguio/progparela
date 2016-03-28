#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

#define TAMBUF 10

int main (int argc, char** argv) 
{
	int meu_ranque, num_procs;
	int i, ret, *buf_escrita;	
	MPI_File arquivo;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	buf_escrita = (int*) malloc (TAMBUF*sizeof(int));
	for (i = 0; i < TAMBUF; i++)
		buf_escrita[i] = meu_ranque*TAMBUF + i;

	ret = MPI_File_open(	MPI_COMM_WORLD, "arquivo.dat", 
							MPI_MODE_CREATE | MPI_MODE_WRONLY, 
							MPI_INFO_NULL, &arquivo);
	if (ret == 0)
		printf("Arquivo aberto com sucesso no processo %d \n", meu_ranque);
	else 
	{
		printf("Arquivo aberto com erro no processo %d \n", meu_ranque);
		MPI_Abort(MPI_COMM_WORLD, 1);
	}

	MPI_File_set_view(	arquivo, meu_ranque*TAMBUF*sizeof(int),
						MPI_INT, MPI_INT, "native", MPI_INFO_NULL);

	MPI_File_write(		arquivo, buf_escrita, 
						TAMBUF, MPI_INT,
						MPI_STATUS_IGNORE);

	MPI_File_close(&arquivo);
	MPI_Finalize();
	return 0;
}