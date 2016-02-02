#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

int primo (int n) {
    int i;
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n%2 == 0) return 0;
        for (i = 3; i < sqrt(n)+1; i += 2) {
            if (n%i == 0)
                return 0;
    }
    return 1;
}

void manager(int num_used, int total) {
    int totalPrimes = 0, turn, i = 0, sendTo, flag;
    MPI_Status status;

    while (i < total) {
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
        if (flag) {
            sendTo = status.MPI_SOURCE;
            MPI_Recv(&turn, 1, MPI_INT, sendTo, 1, MPI_COMM_WORLD, &status);
            totalPrimes += turn;
            //printf("isPrime(%d) = %d\n", i, turn);
            i++;
            MPI_Send(&i, 1, MPI_INT, sendTo, 2, MPI_COMM_WORLD);;
        }
    }

    for (i = 0; i < num_used; i++) {
        turn = -10;
        MPI_Send(&turn, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
    }

    printf("Total of primes: %d\n", totalPrimes);
}

void worker(MPI_Comm THE_COMM_WORLD, int managerid) {
    int prime;
    MPI_Status status;
    prime = 0;
    MPI_Send(&prime, 1, MPI_INT, managerid, 1, THE_COMM_WORLD);
    while (prime > -1) {
        MPI_Recv(&prime, 1, MPI_INT, managerid, 2, THE_COMM_WORLD, &status);
        if (prime > 0) {
            prime = primo(prime);
            MPI_Send(&prime, 1, MPI_INT, managerid, 1, THE_COMM_WORLD);
        }
    }
}

int main(int argc, char *argv[]) {

    int my_rank, p, total;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Comm_size(MPI_COMM_WORLD, &p);


    if (my_rank == 0) {
        printf("Total of numbers:\n");
        scanf("%d", &total);
        manager(p, total);
    }
    else {
        worker(MPI_COMM_WORLD, 0);
    }

    MPI_Finalize();

    return 0;
}
