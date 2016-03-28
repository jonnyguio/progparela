#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char *argv[]) {

    int n, i, num_procs, meu_ranque;
    double x[2];
    MPI_File f;
    MPI_Datatype MPI_POINT;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    MPI_Type_contiguous(2, MPI_DOUBLE, &MPI_POINT);
    MPI_Type_commit(&MPI_POINT);

    MPI_File_open(MPI_COMM_WORLD, "1.in", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &f);

    if (meu_ranque == 0) {
        printf("Numero de coordenadas:\n");
        scanf("%d", &n);
        MPI_File_write(f, &n, 1, MPI_INT, MPI_STATUS_IGNORE);
    }

    for (i = 0; i < n; i++) {
        x[0] = i;
        x[1] = i * 10 + 10;
        MPI_File_write(f, &x[0], 1, MPI_POINT, MPI_STATUS_IGNORE);
    }

    MPI_File_close(&f);
    MPI_Finalize();
    return 0;
}
