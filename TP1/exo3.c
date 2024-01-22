#include <stdio.h>
#include <string.h>
#include <mpi.h>
#define MASTER 0
#define SIZE 128

int main(int argc, char **argv){

    int my_rank;
    int nb_proc;
    int source;
    int dest;
    int tag =0;
    char message[SIZE];
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

    if(my_rank !=MASTER){
        sprintf(message, "Hello Master from %d", my_rank);
        dest = MASTER;
        MPI_Send(message, strlen(message)+1, MPI_CHAR,dest,tag,MPI_COMM_WORLD);
    }
    
    else{
        for(source=0;source < nb_proc;source++){
            if(source !n programme tel que chaque processus envoie une chaˆıne de caract`eres `a son successeur (le processus
(rang + 1) mod nb proc), puis rec¸oit un message de son pr´ed´ecess my_rank){
                MPI_Recv(message, SIZE, MPI_CHAR, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
                printf("%s\n", message);
            }
        }
    }
    MPI_Finalize();
    return 0;
}