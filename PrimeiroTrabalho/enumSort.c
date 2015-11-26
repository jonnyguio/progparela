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
    int n, *aEnvia, *aRecebe, *bEnvia, *bRecebe, quant, *correctOrder,
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

    /* Todos os processos precisam das variáveis a seguir*/
    tamBuf = (int) ceil((float) n / (float) numProcessors);
    bEnvia = (int *) malloc(sizeof(int) * tamBuf);
    aRecebe = (int *) malloc(sizeof(int) * tamBuf);
    correctOrder = (int *) malloc(sizeof(int) * numProcessors);

    /* Zera a quantidade para todos os bs */
    for (i = 0; i < tamBuf; i++) {
        bEnvia[i] = 0;
    }

    /* Inicialização da entrada */
    if (myId == 0) {
        aEnvia = (int *) malloc(sizeof(int) * n);
        bRecebe = (int *) malloc(sizeof(int) * n);
        for (i = 0; i < n; i++) {
            aEnvia[i] = (i >= n / 2)? i % (n / 2) + 1 : i + n / 2 + 1;
        }
        GET_TIME(begin);
    }

    /* Envio para todos da entrada */
    MPI_Scatter(aEnvia, tamBuf, MPI_INT, aRecebe, tamBuf, MPI_INT, 0, MPI_COMM_WORLD);

    /*for (i = 0; i < tamBuf; i++) {
        printf("\t%d\n", aRecebe[i]);
    }*/

    /*
    Cada processo calcula então quantos números são menores dentro do vetor recebido
    Nota-se então que a ordenação é parcelada, cada processo ordena um vetor menor.
    */
    for (i = 0; i < tamBuf; i++) {
        quant = 0;
        for (j = 0; j < tamBuf; j++) {
            if (aRecebe[i] > aRecebe[j])
                quant++;
        }
        bEnvia[quant] = aRecebe[i];
    }

    /* Junta no processo raiz todos os vetores ordenados */
    MPI_Gather (bEnvia, tamBuf, MPI_INT, bRecebe, tamBuf, MPI_INT, 0, MPI_COMM_WORLD);

    if (myId == 0) {
        GET_TIME(end);
        int aux;
        /* Ordena então corretamente os vetores recebidos */
        for (i = 0; i < numProcessors; i++)
            correctOrder[i] = i;
        for (i = 0; i < numProcessors; i++) {
            for (j = i + 1; j < numProcessors; j++) {
                if (bRecebe[correctOrder[i]*tamBuf] > bRecebe[correctOrder[j] * tamBuf])
                {
                    aux = correctOrder[i];
                    correctOrder[i] = correctOrder[j];
                    correctOrder[j] = aux;
                }
            }
        }
        results = fopen("./results.txt", "w+");
        for (i = 0; i < numProcessors; i++) {
            for (j = 0; j < tamBuf; j++) {
            /* Na realidade, o vetor bRecebe continua desornenado. Tudo que ele tem agora é a ordem correta de impressão */
                fprintf(results, "%d ", bRecebe[correctOrder[i] * tamBuf + j]);
            }
        }
        fprintf(results, "\n");
        printf("Tempo paralelo:%.10f\n", end - begin);
    }

    MPI_Finalize();

    return 0;
}
