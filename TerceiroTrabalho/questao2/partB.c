#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

#define TAMBUF 10
#define TAMTUPLA 2

int meu_ranque, num_procs, ret;
int *buf_leitura;
MPI_File arquivo, arquivofinal;
MPI_Datatype tupla;

void leitura();
void leitura_final();
void escrita();

int main (int argc, char** argv) 
{	
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	leitura();
	escrita();

	MPI_File_close(&arquivo);
	MPI_Finalize();

	leitura_final();

	return 0;
}

void leitura()
{
	int i, ret;
	buf_leitura = (int*) malloc (TAMBUF*sizeof(int));

	ret = MPI_File_open(	MPI_COMM_WORLD, "arquivo.dat", 
							MPI_MODE_RDONLY, 
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

	MPI_File_read(		arquivo, buf_leitura, 
						TAMBUF, MPI_INT,
						MPI_STATUS_IGNORE);

	for (i = 0; i < TAMBUF; ++i)
		printf("%d\n", buf_leitura[i]);
}

void escrita()
{	
	int i;
	MPI_Type_contiguous(TAMTUPLA, MPI_INT,&tupla);    
    MPI_Type_commit(&tupla);

	ret = MPI_File_open(	MPI_COMM_WORLD, "arquivofinal.dat", 
							MPI_MODE_WRONLY | MPI_MODE_CREATE, 
							MPI_INFO_NULL, &arquivofinal);
	if (ret == 0)
		printf("Arquivo final aberto com sucesso no processo %d \n", meu_ranque);
	else 
	{
		printf("Arquivo final aberto com erro no processo %d \n", meu_ranque);
		MPI_Abort(MPI_COMM_WORLD, 1);
	}
	    
	MPI_File_set_view(	arquivofinal, 0,
						MPI_INT, MPI_INT, 
						"native", MPI_INFO_NULL);

	for (i = 0; i < TAMBUF; i+=TAMTUPLA)
		MPI_File_write_ordered(	arquivofinal, buf_leitura + i, 1, tupla, MPI_STATUS_IGNORE);
	
	MPI_File_close(&arquivofinal);
}

void leitura_final()
{
	printf("-----------------------------------------------\n");
	printf("Resultado final:\n");
	FILE* file = fopen("arquivofinal.dat", "rb");
	if(!(buf_leitura = (int*) malloc (TAMBUF*num_procs*sizeof(int))))
	{
		printf("erro malloc\n");
		exit(1);
	}
	if(!fread (buf_leitura, sizeof(int), num_procs*TAMBUF, file))
	{
		printf("erro fread\n");
		exit(1);
	}

	int i;
	for (i = 0; i < num_procs*TAMBUF; ++i)
		printf("%d\n", buf_leitura[i]);
}