#include <stdio.h>
#include <string.h>
#define BUFLEN 20
#include <mpi.h>

int main(int argc, char *argv[]){
    int rank;
    int nb_proc;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
    printf("Processus %d sur %d : Hello MPI \n", rank, nb_proc); 
    MPI_Finalize();
    return 0;
}