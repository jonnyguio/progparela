#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[]) {

    int my_rank, p, n, i, j, cond;

    FILE *f;

    double
        *x, *y, slope, y_intercept,
        mySUMx = 0, mySUMy = 0, mySUMxy = 0, mySUMxx = 0,
        SUMx, SUMy, SUMxy, SUMxx;

    MPI_Datatype coordinates;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (!my_rank) {
        f = fopen("q1.in", "r");
        if (!f) {
            printf("Error. File not found.\n");
            exit(-1);
        }
        fscanf(f, "%d", &n);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Type_contiguous(n, MPI_DOUBLE, &coordinates);
    MPI_Type_commit(&coordinates);

    x = malloc(sizeof(double) * n);
    y = malloc(sizeof(double) * n);

    if (!my_rank) {
        for (i = 0; i < n; i++) {
            fscanf(f, "%lf", x + i);
            fscanf(f, "%lf", y + i);
        }
    }

    MPI_Bcast(x, 1, coordinates, 0, MPI_COMM_WORLD);
    MPI_Bcast(y, 1, coordinates, 0, MPI_COMM_WORLD);

    cond = n / p * (my_rank + 1);
    for (j = (n / p) * my_rank; j < cond; j++) {
        if (n % p && my_rank == p - 1 && cond < n - 1) {
            cond += n % p;
        }
        mySUMx = mySUMx + x[j];
        mySUMy = mySUMy + y[j];
        mySUMxy = mySUMxy + x[j] * y[j];
        mySUMxx = mySUMxx + x[j] * x[j];
    }

    MPI_Reduce(&mySUMx, &SUMx, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&mySUMy, &SUMy, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&mySUMxy, &SUMxy, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&mySUMxx, &SUMxx, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (!my_rank) {
        slope = (SUMx * SUMy - n * SUMxy ) / ( SUMx * SUMx - n * SUMxx );
        y_intercept = ( SUMy - slope * SUMx ) / n;
        printf("Slope: %lf\tY:%lf\n", slope, y_intercept);
    }

    MPI_Type_free(&coordinates);

    MPI_Finalize();

    return 0;
}
