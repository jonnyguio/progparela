/*
 * Paraleliza��o
 * Calpi
 * calpi.c
 * Ultima revis�o GPS 18/08/04
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <sys/time.h>

#define GET_TIME(now) { \
   struct timeval t; \
   gettimeofday(&t, NULL); \
   now = t.tv_sec + t.tv_usec/1000000.0; \
}

#define f(x) ((double)(4.0 / (1.0 + x * x)))
#define PI ((double)(4.0 * atan(1.0)))

int solicita (void);
void coleta (double sum);

int main(int argc, char *argv[])
{
    double sum, pi, w, begin, end;
    int i, N, numProcessors, myId;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcessors);
    MPI_Comm_rank(MPI_COMM_WORLD, &myId);

    if (myId == 0) {
        printf ("Entre o número de intervalos para a aproximação:(0 para terminar)\n");
        scanf("%d", &N);
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    while (N > 0) {
        GET_TIME(begin);
        w = 1.0 / (double) N;
        sum = 0.0;
        for (i = myId + 1; i <= N; i += numProcessors)
            sum = sum + f(((double)i - 0.5) * w);

        sum = sum * w;

        MPI_Reduce(&sum, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        if (myId == 0) {
            GET_TIME(end);
            printf("Tempo de execução da parte paralela: %.10f\n\n", end - begin);
            coleta (pi);
            N = solicita ();
        }
        MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();

    return (0);
}

int solicita (void)
{
    int N;
    printf ("Entre o número de intervalos para a aproximação:(0 para terminar)\n");
    scanf("%d",&N);
    return (N);
}

void coleta(double sum)
{
    double err;
    err = sum - PI;
    printf("soma, erro = %7.5f, %10e\n", sum, err);
}
