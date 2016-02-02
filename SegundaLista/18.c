#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#define TAM 4

int dotProduct(float *a, float *b, int size) {
    int sum = 0, i;
    for (i = 0; i < size; i++)
        sum += *(a + i) * *(b + i);
    return sum;
}

int main (int argc, char *argv[]) {
    float *vectorRecebe1, *vectorRecebe2, vectorEnvia1[TAM], vectorEnvia2[TAM];
    int sums, total;
    int rank, processors, i;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &processors);

    if (rank == 0) {
        srand(time(NULL));
        for (i = 0; i < TAM; i++) {
            vectorEnvia1[i] = rand() % 9 + 1;
            vectorEnvia2[i] = rand() % 9 + 1;
        }
    }

    vectorRecebe1 = (float *) malloc(sizeof(float) * TAM / processors);
    vectorRecebe2 = (float *) malloc(sizeof(float) * TAM / processors);

    MPI_Scatter(vectorEnvia1, TAM / processors, MPI_FLOAT, vectorRecebe1, TAM / processors, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Scatter(vectorEnvia2, TAM / processors, MPI_FLOAT, vectorRecebe2, TAM / processors, MPI_FLOAT, 0, MPI_COMM_WORLD);

    printf("Processo %d\n", rank);
    for (i = 0; i < TAM / processors; i++) {
        printf("[1]:%f [2]: %f, ", vectorRecebe1[i], vectorRecebe2[i]);
    }
    printf("\n");

    sums = dotProduct(vectorRecebe1, vectorRecebe2, TAM / processors);

    MPI_Reduce(&sums, &total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Resultado: %d\n", total);
        printf("Resultado2: %d\n", dotProduct(vectorEnvia1, vectorEnvia2, TAM));
    }

    MPI_Finalize();
}
