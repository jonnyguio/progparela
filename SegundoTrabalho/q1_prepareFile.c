#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {

    int n, i;
    double x, y;
    FILE *f;

    printf("Numero de coordenadas: ");
    scanf("%d", &n);

    f = fopen("q1.in", "w+");
    fprintf(f, "%d\n", n);
    srand(TIME(NULL));
    for (i = 0; i < n; i++) {
        x = rand() * 100;
        y = rand() * 50;
        fprintf(f, "%lf %lf\n", x, y);
    }
    fclose(f);
    return 0;
}
