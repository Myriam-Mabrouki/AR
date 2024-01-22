#include <stdio.h>
#include <string.h>
#include <mpi.h>
#define SIZE 128

int main(int argc, char **argv){

    int my_rank;
    int nb_proc;
    int next_rank;
    int prec_rank;
    int tag =0;
    char message[SIZE];
    char recu[SIZE];
    MPI_Status status;

    MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
        MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

        next_rank = (my_rank+1)%nb_proc;
        prec_rank = ((my_rank-1)+nb_proc)%nb_proc;

        sprintf(message, "je suis %d et j'envoie a %d \n",my_rank,next_rank);
        if(my_rank%2){
            MPI_Ssend(message, strlen(message)+1, MPI_CHAR, next_rank ,tag,MPI_COMM_WORLD);
            MPI_Recv(recu, SIZE, MPI_CHAR, prec_rank , tag, MPI_COMM_WORLD, &status);
            printf("%s",recu);
        }else{
            MPI_Recv(recu, SIZE, MPI_CHAR, prec_rank , tag, MPI_COMM_WORLD, &status);
            MPI_Ssend(message, strlen(message)+1, MPI_CHAR, next_rank ,tag,MPI_COMM_WORLD);        
            printf("%s",recu);
        }
        
    MPI_Finalize();
    return 0;
}