/* 
    MALENFER François 28706664
    MABROUKI Myriam   28710344
*/
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TAG_ELECTION 9
#define TAG_ANNONCE_FIN 10

#define INITIATEUR 2
#define BATTU 1
#define FIN 0

MPI_Status status;
int rank;
int etat;
int nb_proc;

void recevoir () {
    int recu;
    MPI_Recv(&recu, 1, MPI_INT, (rank-1+nb_proc)%nb_proc, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    switch (status.MPI_TAG) {
        case TAG_ELECTION:
            if (etat == INITIATEUR){
                if (rank < recu){
                    etat = BATTU;
                    printf("Je suis le processus %d, j'étais initiateur mais je suis battu par %d\n", rank, recu);
                    MPI_Send(&recu, 1, MPI_INT, (rank+1)%nb_proc, TAG_ELECTION, MPI_COMM_WORLD);
                }
                else {

                    if (rank == recu){ /* rank == recu, cas de terminaison car on est le gagnant */
                        etat = FIN;
                        printf("Je suis le processus %d et j'ai gagné !\n", rank);
                        for (int i = 0; i < nb_proc; i++){
                            if (i != rank) MPI_Send(&rank, 1, MPI_INT, i, TAG_ANNONCE_FIN, MPI_COMM_WORLD);
                        }
                    }
                    /* Else: initiateur, mais ne transmet pas le jeton car son identité n'est pas supérieure à la sienne. */
                    else {
                        printf("Je suis le processus %d et j'absorbe le token %d\n", rank, recu);
                    }
                }
            }
            else {
                etat = BATTU;
                printf("Je suis le processus %d, je n'étais pas initiateur et je transmets le message de %d\n", rank, recu);
                MPI_Send(&recu, 1, MPI_INT, (rank+1)%nb_proc, TAG_ELECTION, MPI_COMM_WORLD);
            } 
            break;
        case TAG_ANNONCE_FIN:
            etat = FIN;
            printf("Je suis le processus %d et mon leader est %d\n", rank, recu);
            MPI_Send(&recu, 1, MPI_INT, (rank+1)%nb_proc, TAG_ANNONCE_FIN, MPI_COMM_WORLD);
            break;
    }
}


int main(int argc, char **argv){

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);

    srand(getpid());
    etat = rand()%2 ? INITIATEUR : BATTU;

    if (etat == INITIATEUR){
        printf("init je suis %d il y a %d truc \n",rank,nb_proc);
        MPI_Send(&rank, 1, MPI_INT, (rank+1)%nb_proc, TAG_ELECTION, MPI_COMM_WORLD);
        
    }
    printf("je suis %d il y a %d truc \n",rank,nb_proc);
    while (etat != FIN){
        recevoir();
    }

    MPI_Finalize();

    return 0;
}