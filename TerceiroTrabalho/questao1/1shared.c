/*) Neste trabalho você deve modificar um programa que calcula a equação de uma reta
utilizando o método de mínimos quadrados. Antes disso, contudo, você deve escrever um
pequeno programa que gere um arquivo cujo primeiro dado é um valor n, o segundo o tipo de
dados utilizado, seguido de n pares de coordenadas x e y que vão servir como entrada de
dados. Você pode usar uma equação conhecida e atribuir um pequeno erro aleatoriamente
para cada y gerado. Utilize um formato de dados nativo e rotinas do MPI I/O com tipos de
dados derivados, ponteiros individuais e a rotina MPI_File_seek para criar esse arquivo. Para
modificar o programa observe as seguintes instruções:
 Uma versão MPI do programa do método dos mínimos quadrados pode ser obtida
no endereço:
http://www.dcc.ufrj.br/~gabriel/progpar/minquad.zip
 Escreva as seguintes versões do programa com rotinas do MPI I/O:
i. Utilizando ponteiros individuais com offset;
ii. Utilizando ponteiros compartilhados.
 Em todas as versões utilize vistas de arquivo, tipos de dados derivados e rotinas de
comunicação coletiva (redução) para a recepção das somas parciais;
 Compile, execute com n = 100.000 e 1.000.000 utilizando-se de 8 processos
 Compare as diferenças de desempenho.*/

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "mpi.h"
#include <sys/time.h>

#define GET_TIME(now) { \
   struct timeval t; \
   gettimeofday(&t, NULL); \
   now = t.tv_sec + t.tv_usec/1000000.0; \
}

int main(int argc, char **argv) {

  double *xy;
  double mySUMx, mySUMy, mySUMxy, mySUMxx, SUMx, SUMy, SUMxy,
         SUMxx, SUMres, res, slope, y_intercept, y_estimate,
         begin, end;
  int i, j, n, myid, numprocs, naverage, nremain, mypoints,
    sizeFile, ret;
  /*int new_sleep (int seconds);*/
  MPI_Status istatus;
  MPI_Datatype MPI_POINT;
  MPI_File infile;
  MPI_Offset ishift;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myid);
  MPI_Comm_size (MPI_COMM_WORLD, &numprocs);

  MPI_Type_contiguous(2, MPI_DOUBLE, &MPI_POINT);
  MPI_Type_commit(&MPI_POINT);

  ret = MPI_File_open(MPI_COMM_WORLD, "1.in", MPI_MODE_RDONLY, MPI_INFO_NULL, &infile);
  if (ret == 0)
    printf("Arquivo aberto com sucesso no processo %d \n", myid);
  else {
    printf("Arquivo aberto com erro no processo %d \n", myid);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  /* ----------------------------------------------------------
   * Step 1: Process 0 reads data and sends the value of n
   * ---------------------------------------------------------- */


  if (myid == 0) {
    MPI_File_seek(infile, 0, MPI_SEEK_SET);
    MPI_File_read_shared(infile, &n, 1, MPI_INT, &istatus);
  }

  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

  naverage = n / numprocs;
  nremain = n % numprocs;

//  printf("%d - %d - %d - %d - %d\n", myid, n, naverage, nremain, 2 * (naverage + nremain));

  xy = (double *) malloc (2 * (naverage + nremain) * sizeof(double));

  /*if (myid == 0) {
    printf ("Number of processes used: %d\n", numprocs);
    printf ("-------------------------------------\n");
    printf ("The x coordinates on worker processes:\n");

    fscanf (infile, "%d", &n);
    x = (double *) malloc (n*sizeof(double));
    y = (double *) malloc (n*sizeof(double));
    for (i=0; i<n; i++)
      fscanf (infile, "%lf %lf", &x[i], &y[i]);
    for (i=1; i<numprocs; i++)
      MPI_Send (&n, 1, MPI_INT, i, 10, MPI_COMM_WORLD);
  }
  else {
    MPI_Recv (&n, 1, MPI_INT, 0, 10, MPI_COMM_WORLD, &istatus);
    x = (double *) malloc (n*sizeof(double));
    y = (double *) malloc (n*sizeof(double));
  }*/
  /* ---------------------------------------------------------- */

  /* ----------------------------------------------------------
   * Step 2: Process 0 sends subsets of x and y
   * ---------------------------------------------------------- */

  if (myid == 0)
    GET_TIME(begin);
  ishift = myid * naverage;
  mypoints = (myid < numprocs - 1) ? naverage : naverage + nremain;

  //MPI_File_set_view(infile, ishift, MPI_POINT, MPI_DOUBLE, "native", MPI_INFO_NULL);
  //MPI_File_seek(infile, ishift * sizeof(double) * 2, MPI_SEEK_CUR);
  MPI_File_read_shared(infile, &xy[0], mypoints, MPI_POINT, &istatus);

  /*for (i = 0; i < mypoints * 2; i += 2) {
      printf("(%d) %d: ", myid, i);
      printf("%.0lf ", xy[i]);
      printf("%.0lf\n", xy[i + 1]);
  }
  printf("\n");*/

  /*if (myid == 0) {
    for (i=1; i<numprocs; i++) {
      ishift = i * naverage;
      mypoints = (i < numprocs - 1) ? naverage : naverage + nremain;
      MPI_Send (&ishift, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
      MPI_Send (&mypoints, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
      MPI_Send (&x[ishift], mypoints, MPI_DOUBLE, i, 3, MPI_COMM_WORLD);
      MPI_Send (&y[ishift], mypoints, MPI_DOUBLE, i, 4, MPI_COMM_WORLD);
    }
  }
  else {
    MPI_Recv (&ishift, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &istatus);
    MPI_Recv (&mypoints, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &istatus);
    MPI_Recv (&x[ishift], mypoints, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD,
	      &istatus);
    MPI_Recv (&y[ishift], mypoints, MPI_DOUBLE, 0, 4, MPI_COMM_WORLD,
	      &istatus);
    printf ("id %d: ", myid);
    for (i=0; i<n; i++) printf("%4.2lf ", x[i]);
    printf ("\n");
  }*/

  /* ----------------------------------------------------------
   * Step 3: Each process calculates its partial sum
   * ---------------------------------------------------------- */
  mySUMx = 0; mySUMy = 0; mySUMxy = 0; mySUMxx = 0;
  if (myid == 0) {
    ishift = 0;
    mypoints = naverage;
  }
  for (j = 0; j < mypoints * 2; j += 2) {
    mySUMx += xy[j];
    mySUMy += xy[j + 1];
    mySUMxy += xy[j] * xy[j + 1];
    mySUMxx += xy[j] * xy[j];
  }

//  printf("%d:\t%lf - %lf - %lf - %lf\n", myid, mySUMx, mySUMy, mySUMxy, mySUMxx);

  /* ----------------------------------------------------------
   * Step 4: Process 0 receives partial sums from the others
   * ---------------------------------------------------------- */

   MPI_Reduce(&mySUMx, &SUMx, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
   MPI_Reduce(&mySUMy, &SUMy, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
   MPI_Reduce(&mySUMxy, &SUMxy, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
   MPI_Reduce(&mySUMxx, &SUMxx, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

   if (myid == 0)
     GET_TIME(end);

  /*if (myid != 0) {
    MPI_Send (&mySUMx, 1, MPI_DOUBLE, 0, 5, MPI_COMM_WORLD);
    MPI_Send (&mySUMy, 1, MPI_DOUBLE, 0, 6, MPI_COMM_WORLD);
    MPI_Send (&mySUMxy, 1, MPI_DOUBLE, 0, 7, MPI_COMM_WORLD);
    MPI_Send (&mySUMxx, 1, MPI_DOUBLE, 0, 8, MPI_COMM_WORLD);
	    }
  else {
    SUMx = mySUMx; SUMy = mySUMy;
    SUMxy = mySUMxy; SUMxx = mySUMxx;
    for (i=1; i<numprocs; i++) {
      MPI_Recv (&mySUMx, 1, MPI_DOUBLE, i, 5, MPI_COMM_WORLD, &istatus);
      MPI_Recv (&mySUMy, 1, MPI_DOUBLE, i, 6, MPI_COMM_WORLD, &istatus);
      MPI_Recv (&mySUMxy, 1, MPI_DOUBLE, i, 7, MPI_COMM_WORLD, &istatus);
      MPI_Recv (&mySUMxx, 1, MPI_DOUBLE, i, 8, MPI_COMM_WORLD, &istatus);
      SUMx = SUMx + mySUMx;
      SUMy = SUMy + mySUMy;
      SUMxy = SUMxy + mySUMxy;
      SUMxx = SUMxx + mySUMxx;
    }
  }*/

  /* ----------------------------------------------------------
   * Step 5: Process 0 does the final steps
   * ---------------------------------------------------------- */
  if (myid == 0) {
    slope = (SUMx * SUMy - n * SUMxy ) / (SUMx * SUMx - n * SUMxx);
    y_intercept = (SUMy - slope * SUMx) / n;

    printf ("\n");
    printf ("The linear equation that best fits the given data:\n");
    printf ("       y = %6.2lfx + %6.2lf\n", slope, y_intercept);
    printf ("--------------------------------------------------\n");
    printf ("   Original (x, y)     Estimated y     Residual\n");
    printf ("--------------------------------------------------\n");
    SUMres = 0;
  }
  MPI_Bcast(&slope, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(&y_intercept, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  for (j = 0; j < numprocs; j++) {
    MPI_Barrier(MPI_COMM_WORLD);
    if (j == myid) {
      SUMres = 0;
      for (i = 0; i < mypoints * 2; i += 2) {
        y_estimate = slope * xy[i] + y_intercept;
        res = xy[i + 1] - y_estimate;
        SUMres = SUMres + res * res;
        printf("   (%6.2lf %6.2lf)      %6.2lf       %6.2lf\n", xy[i], xy[i + 1], y_estimate, res);
      }
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  if (myid == 0) {
    printf("--------------------------------------------------\n");
    printf("Residual sum = %6.2lf\n", SUMres);
    printf("Time: %lf\n", end - begin);
  }

  MPI_File_close(&infile);

  MPI_Finalize();
}
