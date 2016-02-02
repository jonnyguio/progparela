#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <math.h>

#define ALL 1000000
#define ETIQ1 1234
#define ETIQ2 2345

int turn;

int primo (int n) {
    int i;
    if (n % 2 == 0)
        return 0;
    for (i = 3; i < sqrt(n) + 1; i += 2) {
        if (n % i == 0)
            return 0;
    }
    return 1;
}

void manager(int workers) {
    int iGot = 0, iSent = 0, gotFrom, sendTo, i, x, y;
    int inputs[ALL - 2], outputs[ALL - 2], *last;
    MPI_Status status;

    for(i = 0; i < ALL - 2; i++) {
        inputs[i] = i + 3;
    }

    last = (int *) malloc(sizeof(int) * workers);
    while (iGot < ALL - 2) {
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        gotFrom = status.MPI_SOURCE;
        sendTo = gotFrom;
        MPI_Recv(&x, 1, MPI_INT, gotFrom, ETIQ1, MPI_COMM_WORLD, &status);
        if (x != -1) {
            outputs[last[gotFrom - 1] - 3] = x;
            iGot++;
        }

        if (iSent < ALL - 2) {
            y = inputs[iSent];
            iSent++;
            last[sendTo - 1] = y;
            MPI_Send(&y, 1, MPI_INT, sendTo, ETIQ2, MPI_COMM_WORLD);
        }
    }
    for (i = 1; i <= workers; i++){
        x = -2;
        MPI_Send(&x, 1, MPI_INT, i, ETIQ2, MPI_COMM_WORLD);
    }
    for (i = 0; i < ALL - 2; i++) {
        if (outputs[i])
        printf("%d ", i + 3);
    }
    printf("\n");
    free(last);
}

void worker(int manager) {
    int x;
    MPI_Status status;
    x = -1;
    sleep(1);
    while (x > -2) {
        /* send message says I am ready for data */
        MPI_Send(&x, 1, MPI_INT, manager, ETIQ1, MPI_COMM_WORLD);
        /* get a message from the manager */
        MPI_Recv(&x, 1, MPI_INT, manager, ETIQ2, MPI_COMM_WORLD, &status);
        /* process data */
        if (x != -2) {
            turn = x;
            x = primo(x);
        }
    }
}

int main(int argc, char *argv[]) {
    int processors, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processors);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (!rank) {
        printf("Criando manager (%d)\n", processors - 1);
        manager(processors - 1);
    }
    else {
        printf("Criando trabalhador\n");
        worker(0);
    }

    MPI_Finalize();
}
