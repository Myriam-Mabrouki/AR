/* MALENFER François 28706664 
   MABROUKI Myriam   28710344
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>


/* types de message */
#define REQUEST 1
#define ACK 2
#define FIN 3

/* etat */
#define REQUESTING 1
#define NOT_REQUESTING 2
#define CRITICAL_SECTION 3

#define MAX_CS 5

typdef struct _file {
    int p; /* Identifiant du processus */
    int h; /* Horloge */
} File;

MPI_Status status;
int nb_proc;
int nback = 0;

int rank;
int etat = NOT_REQUESTING;
int horloge = 0;
int date_requete = 0;
File file[nb_proc];

void Request_CS (void) {
    
    /* Initialisation */
    nback = 0;
    state = REQUESTING;
    horloge++;
    date_requete = horloge;

    /* Diffuser (REQ, horloge) */
    for (int i = 0; i < nb_proc; i++){
        if (i != rank ) MPI_Send(horloge,1,MPI_int,i,REQUEST,MPI_COMM_WORLD);
    }
}

void Release_CS (void) {
   horloge++;

    for (int i = 0; i < nb_proc; i++){
        if (file[i].p == -1) { /* Fin de la file si elle n'est pas complète */
            break;
        }
        else {
            MPI_Send(horloge,1,MPI_int,file[i].p,ACK,MPI_COMM_WORLD);
        }
    }

   etat = NOT_REQUESTING;

   /* On vide la file */
    for (int i = 0; i < nb_proc; i++){
        file[i].p = -1;
    }
}

/* Renvoie 1 si le processus courant est plus prioritaire et 0 sinon */
int Prioritaire (int h, int id) {
    if (horloge < h)
        return 1;
    if (h > horloge)
        return 0;
    if (rank < id)
        return 1;
    return 0;
} 

void Attendre_message (int * i) {
   
    int recu;
    MPI_Recv(&recu, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    
    horloge = max (horloge, recu) + 1;

    switch (status.MPI_TAG) {
        case REQUEST :
            int 
            if (etat == NOT_REQUESTING) {
                horloge++;
                MPI_Send(horloge,1,MPI_int,status.MPI_SOURCE,ACK,MPI_COMM_WORLD);
            }
            else {
                if (etat == REQUESTING && !Prioritaire(recu, status.MPI_SOURCE)) {
                    horloge++;
                    MPI_Send(horloge,1,MPI_int,status.MPI_SOURCE,ACK,MPI_COMM_WORLD);
                }
                else{
                    for (int i = 0; i < nb_proc; i++){
                        if (file[i].p == -1){
                            file[i].p = status.MPI_SOURCE;
                            file[i].h = recu;
                            break;
                        }
                    }
                }
            }
            break;

        case ACK:
            nback++;
            if (nback == nb_proc - 1) {
                etat = CRITICAL_SECTION;
            }
            break;

        case FIN :
            *i = *i + 1;
        
  }
}

int main (int argc, char* argv[]) {
   int i;
   int cont_CS=0;
  
    /* initialisation MPI */
    MPI_init(&argc,&argv);
	
   while (cont_CS < MAX_CS ) {

      Request_CS();
      
      nback = 0;
	
      while ( nback < nb_proc -1)
         Attendre_message (&i);

      /* CS */
      cont_CS++;
      printf ("ENTREE EN SECTION CRITIQUE cont:% d - proc:%d  \n", cont_CS, rang);
      sleep(1);
      printf ("SORTIE DE SECTION CRITIQUE cont:% d - proc:%d  \n", cont_CS, rang);
      Release_CS();
    }
        
    /* traiter la terminaison de l'algorithme */  
    for (i = 0; i < nb_proc; i++){
        if (i != rank ) MPI_Send(i,1,MPI_int,i,REQUEST,MPI_COMM_WORLD);
    }

    i = 0;
    while (i < nb_proc -1)
        Attendre_message (&i);

    MPI_Finalize();
    /* terminer MPI */
    return 0;
}
       


