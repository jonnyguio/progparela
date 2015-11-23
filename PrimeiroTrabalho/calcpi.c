
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

int main (int argc, char **argv)
{
    int n, myid, numprocs, i, rc;
    double mypi, pi, h, x, sum = 0.0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    if (myid == 0)
    {
    /* printf ("Entre com o número de intervalos: ");
    scanf("%d", &n);*/
        if(argc != 2)
        {
          printf("Número incorreto de argumentos\n");
          exit(-1);
        }

        if (sscanf(argv[1], "%d", &n) != 1)
        {
            /* Erro  */
            exit(-1);
        }
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (n != 0)
    {
        h = 1.0 / (double) n;
        for (i=myid + 1; i <= n; i+=numprocs)
        {
            x = h * ((double)i - 0.5);
            sum += (4.0/(1.0 + x*x));
        }
        mypi = h * sum;

        MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM,0, MPI_COMM_WORLD);

        if (myid == 0)
            printf ("valor aproximado de pi: %.16f \n", pi);
    }
    MPI_Finalize( );
}
