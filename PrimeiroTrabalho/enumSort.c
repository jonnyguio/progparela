#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <sys/time.h>
#include <unistd.h>

#define GET_TIME(now) { \
   struct timeval t; \
   gettimeofday(&t, NULL); \
   now = t.tv_sec + t.tv_usec/1000000.0; \
}

int main(int argc, char *argv[]) {
    int n, *aEnvia, *aRecebe, *bEnvia, *bRecebe, quant,
        i, j,
        numProcessors, myId, tamBuf,
        old_stdout;
    double begin, end;
    FILE *file, *results;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcessors);
    MPI_Comm_rank(MPI_COMM_WORLD, &myId);

    if (myId == 0)
    {
        printf("Entre com o numero de elementos:\n");
        scanf("%d", &n);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    tamBuf = (int) ceil((float) n / (float) numProcessors);
    bEnvia = (int *) malloc(sizeof(int) * tamBuf);
    aRecebe = (int *) malloc(sizeof(int) * tamBuf);

    for (i = 0; i < tamBuf; i++) {
        bEnvia[i] = 0;
    }

    if (myId == 0) {
        aEnvia = (int *) malloc(sizeof(int) * n);
        bRecebe = (int *) malloc(sizeof(int) * n);
        for (i = 0; i < n; i++) {
            aEnvia[i] = (i >= n / 2)? i % (n / 2) + 1 : i + n / 2 + 1;
        }
        GET_TIME(begin);
    }

    MPI_Scatter(aEnvia, tamBuf, MPI_INT, aRecebe, tamBuf, MPI_INT, 0, MPI_COMM_WORLD);

    for (i = 0; i < tamBuf; i++) {
        printf("\t%d\n", aRecebe[i]);
    }

    for (i = 0; i < tamBuf; i++) {
        quant = 0;
        for (j = 0; j < tamBuf; j++) {
            if (aRecebe[i] > aRecebe[j])
                quant++;
        }
        bEnvia[quant] = aRecebe[i];
    }

    MPI_Reduce(bEnvia, bRecebe, tamBuf, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
    //MPI_Gather (bEnvia, tamBuf, MPI_INT, bRecebe, tamBuf, MPI_INT, 0, MPI_COMM_WORLD);

    if (myId == 0) {
        GET_TIME(end);
        results = fopen("./results.txt", "w+");
        for (i = 0; i < n; i++) {
            printf("%d ", bRecebe[i]);
            fprintf(results, "%d ", bRecebe[i]);
        }
        printf("\n");
        fprintf(results, "\n");
        printf("Tempo paralelo:%.10f\n", end - begin);
    }

    MPI_Finalize();

    return 0;
}
