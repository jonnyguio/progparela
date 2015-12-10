int main(int argc, char** argv){
    int num_procs, meu_ranque;
    int origem=0, destino, etiq=1, i;
    int blocklengths[2], displacements[2];
    float a[16] =
        {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0,
            9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0};
    float b[NELEMENTS];

    MPI_Status estado;
    MPI_Datatype indextype;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    blocklengths[0] = 4;
    blocklengths[1] = 2;
    displacements[0] = 5;
