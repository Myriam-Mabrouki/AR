#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define TAGINIT 0
#define NB_SITE 4

#define TAG 99

void calcul_min(int rang) {
   
   int voisins[3];
   int nb_voisins;
   int min_local;
   
   MPI_Status status;
   int min_recu = INT_MAX;
   int nb_recu = 0; 
   bool sent = false;
   bool recu[3] = {false, false, false}; /* Taille max du tableau : nombre de voisins maximum */

   /* Reçu du simulateur, processus 0  */
   MPI_Recv(&nb_voisins, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, NULL);
   MPI_Recv(&voisins, nb_voisins, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, NULL);
   MPI_Recv(&min_local, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, NULL);

   /* Début de l'algorithme à vagues */

   while(nb_recu != nb_voisins){ /* Condition de décision */
      if(nb_recu < nb_voisins-1){
         MPI_Recv(&min_recu, 1, MPI_INT, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);
         int received = status.MPI_SOURCE;
         int index = -1;
         for (int i = 0; i < nb_voisins; i++){
            if (voisins[i] == received){
               index = i;
               break;
            }
         }
         recu[index] = true;
         nb_recu++;
         if (min_recu < min_local){ 
            min_local = min_recu;
         }
      }
      else{
         if(!sent){
            int src = -1;
            for (int i = nb_voisins - 1; i >= 0; i--){
               if (recu[i] == false){
                  src = i;
                  break;
               }
            }
            MPI_Send(&min_local, 1, MPI_INT, src, TAG, MPI_COMM_WORLD);
         }
      }
   }

   printf("Je suis le décideur : %d. Le minimum est : %d\n", rang, min_local);

}

void simulateur(void) {
   int i;

   /* nb_voisins[i] est le nombre de voisins du site i */
   int nb_voisins[NB_SITE+1] = {-1, 2, 3, 2, 1, 1, 1};
   int min_local[NB_SITE+1] = {-1, 3, 11, 8, 14, 5, 17};

   /* liste des voisins */
   int voisins[NB_SITE+1][3] = {{-1, -1, -1},
         {2, 3, -1}, /* {1, 4, 5}, 
         {1, 6, -1}, */ {3, -1, -1},
         {2, -1, -1}, {3, -1, -1}};
                               
   for(i=1; i<=NB_SITE; i++){
      MPI_Send(&nb_voisins[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
      MPI_Send(voisins[i],nb_voisins[i], MPI_INT, i, TAGINIT, MPI_COMM_WORLD);
      MPI_Send(&min_local[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD); 
   }
}

/******************************************************************************/

int main (int argc, char* argv[]) {
   int nb_proc,rang;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);

   if (nb_proc != NB_SITE+1) {
      printf("Nombre de processus incorrect !\n");
      MPI_Finalize();
      exit(2);
   }
  
   MPI_Comm_rank(MPI_COMM_WORLD, &rang);
  
   if (rang == 0) {
      printf("Je suis le processus 0 qui initialise l'arbre\n");
      simulateur();
   } else {
      calcul_min(rang);
   }
  
   MPI_Finalize();
   return 0;
}
