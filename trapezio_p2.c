
/* Metodo do Trapézio Paralelo
*
* Entrada: Nenhuma.
* Saída: Estimativa da integral de a até b de f(x)
* usando o método do trapézio e n trapezóides.
*
* Algoritmo:
* 1. Cada processo calcula o seu intervalo
* de integração.
* 2. Cada processo estima a integral de f(x)
* sobre seu intervalo utilizando o método do trapézio
* 3a. Cada processo != 0 envia sua integral para 0.
* 3b. Processo 0 soma os valores recebidos dos
* processos individuais e imprime o resultado.
*
* Nota: f(x), a, b, e n são todos fixos.
*/
#include <stdio.h>
#include <math.h>
/* Estaremos utilizando funções e definições MPI. */
#include <mpi.h>

float f(float x) {

        float return_val;
        return_val = exp(x);
        return return_val;
}

float Trap(float local_a, float local_b, int local_n, float h) {
        float integral; /* Armazena o resultado em integral */
        float x;
        int i;
        integral = (f(local_a) + f(local_b))/2.0;
        x = local_a;
        for (i = 1; i != local_n-1; i++) {
            x+= h;
            integral += f(x);
        }
        integral *= h;
        return integral;
} /* Trap */

void main(int argc, char** argv) {
          int my_rank;            /* Rank do meu processo */
        int p;                  /* O número de processos */
          float a = 0.0;          /* Limite esquerdo */
          float b = 1.0;          /* Limite direito */
          int n = 1024;          /* Número de trapezóides */
          float h;                /* Comprimento da base do trapezoide */
          float local_a;          /* Limite esquedo do meu processo */
        float local_b;          /* Limite direito do meu processo  */
          int local_n;            /* Numero de trapezoides que devo calcular */
          float integral;        /* Integral no meu intervalo */
          float total;            /* Integral total */
          int source;            /* Processo enviando a integral */
          int dest = 0;          /* Todas as mensagens vão para  0 */
        int tag = 50;
          MPI_Status status;

          /* Permite ao sistema iniciar o  MPI */
          MPI_Init(&argc, &argv);

          /* Pega o rank do meu processo */
          MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

        /* Encontra quantos processos estão ativos */
          MPI_Comm_size(MPI_COMM_WORLD, &p);

          h = (b-a)/n;
          local_n = n/p; /* So is the number of trapezoids */

          /* Comprimento do intervalo de integração cada
          * processo = local_n*h. Então meu intervalo
          * inicia em: */

          local_a = a + my_rank * local_n * h;
          local_b = local_a + local_n * h;
          integral = Trap(local_a, local_b, local_n, h);

          /* Soma as integrais calculadas por cada processo */

          /*if (my_rank == 0) {
                  total = integral;
                  for (source = 1; source < p; source++) {
                          MPI_Recv(&integral, 1, MPI_FLOAT, source, tag,
                          MPI_COMM_WORLD, &status);
                        total += integral;
                  }
          } else {
                  MPI_Send(&integral, 1, MPI_FLOAT, dest,
                  tag, MPI_COMM_WORLD);
          }*/

          MPI_Allreduce(&integral, &total, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);

        /* Imprime o resultado*/
          if (my_rank == 0) {
                  printf("Com n = %d trapezoides, nossa estimativa \n", n);
                  printf("da integral de %f até %f = %f \n", a, b, total);
          }
        /* Derruba o MPI */
        MPI_Finalize();
} /* main */
