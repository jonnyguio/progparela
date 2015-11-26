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

#define PI ((double)(4.0 * atan(1.0)))

double f(double x) {
    return (double) (4.0 / (1.0 + x * x));
}

int getN (void)
{
    int N;
    printf ("Entre o número de intervalos para a aproximação:(0 para terminar)\n");
    scanf("%d",&N);
    return (N);
}

void print(double sumIntegral)
{
    double err;
    err = sumIntegral - PI;
    printf("soma, erro = %7.5f, %10e\n", sumIntegral, err);
}

int main(int argc, char *argv[])
{
    double sumIntegral, pi, var, begin, end;
    int i, N, numProcessors, myId;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcessors);
    MPI_Comm_rank(MPI_COMM_WORLD, &myId);

    if (myId == 0) {
        printf ("Entre o número de intervalos para a aproximação:(0 para terminar)\n");
        scanf("%d", &N);
        GET_TIME(begin);
    }

    /* Manda para todos qual o tamanho do N */
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    while (N > 0) {
        /* Assim, cada processo calcula um pedaço das somas */
        var= 1.0 / (double) N;
        sumIntegral = 0.0;
        for (i = myId + 1; i <= N; i += numProcessors)
            sumIntegral = sumIntegral + f(((double)i - 0.5) * w);

        sumIntegral = sumIntegral * var;

        MPI_Reduce(&sum, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        if (myId == 0) {
            GET_TIME(end);
            printf("Tempo de execução da parte paralela: %.10f\n\n", end - begin);
            print (pi);
            N = getN ();
        }
        MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();

    return (0);
}
