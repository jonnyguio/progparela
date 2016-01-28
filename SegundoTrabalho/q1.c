#include <stdio.h>
#include <math.h>
#include <mpi.h>

double minSquare(double *x, double *y, int n) {
    double slope, y_intercept, mySUMx = 0, mySUMy = 0, mySUMxy = 0, mySUMxx = 0;
    for (j = 0; j < n; j++) {
        mySUMx = mySUMx + x[j];
        mySUMy = mySUMy + y[j];
        mySUMxy = mySUMxy + x[j]*y[j];
        mySUMxx = mySUMxx + x[j]*x[j];
    }
    slope = (mySUMx * mySUMy - n * mySUMxy ) / ( mySUMx * mySUMx - n * mySUMxx );
    y_intercept = ( mySUMy - slope * mySUMx ) / n;
    return
}

int main(int argc, char const *argv[]) {

    int my_rank, p, n;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (!my_rank) {
        printf("Numero de coordenadas: ");
        scanf("%d", &n);
    }
    return 0;
}
