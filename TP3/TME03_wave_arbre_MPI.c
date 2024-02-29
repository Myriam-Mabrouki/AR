#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define NB_SITE 6

/* Définition des TAG */
#define TAG_INIT 0
#define TAG_ARBRE 1
#define TAG_FIN 2


void calcul_min(int rang) {
   
   int voisins[3];
   int nb_voisins;
   int min_local;
   
   MPI_Status status;
   int min_recu = INT_MAX;
   int nb_recu = 0; 
   bool sent = false;
   int recu[3] = {0}; /* Taille max du tableau : nombre de voisins maximum */

   /* Reçu du simulateur, processus 0  */
   MPI_Recv(&nb_voisins, 1, MPI_INT, 0, TAG_INIT, MPI_COMM_WORLD, NULL);
   MPI_Recv(&voisins, nb_voisins, MPI_INT, 0, TAG_INIT, MPI_COMM_WORLD, NULL);
   MPI_Recv(&min_local, 1, MPI_INT, 0, TAG_INIT, MPI_COMM_WORLD, NULL);
   printf("Je suis %d et j'ai %d voisin(s) : ", rang, nb_voisins);
   for (int i = 0; i < nb_voisins - 1; i++){ 
      printf("%d, ", voisins[i]);
   }
   if (nb_voisins > 1) printf(" et %d; ", voisins[nb_voisins-1]); 
   printf("et mon minimum local est %d.\n", min_local);

   /* Début de l'algorithme à vagues */

   while(1){
      if(nb_recu < nb_voisins-1){
         MPI_Recv(&min_recu, 1, MPI_INT, MPI_ANY_SOURCE, TAG_ARBRE, MPI_COMM_WORLD, &status);
         int received = status.MPI_SOURCE;
         printf("Je suis %d, et j'ai reçu un message de %d.\n", rang, received);
        
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
         printf("Je suis %d dans if %d\n", rang, nb_recu);
      }

      else{
         if(!sent){
            printf("Je suis %d dans else\n", rang);
            int src = -1;
            if (nb_voisins > 1){
               for (int i = nb_voisins - 1; i >= 0; i--){
                  if (recu[i] == false){
                     src = i;
                     break;
                  }
               }
            }
            else {
               src = voisins[0];
            }
            
            printf("Je suis %d, et j'envoie à %d.\n", rang, src);
            MPI_Send(&min_local, 1, MPI_INT, src, TAG_ARBRE, MPI_COMM_WORLD);
            sent = true;
         }
      }

      /* Condition d'arrêt */

      /* Décideur si on a reçu un message de tous nos voisins */
      if (nb_recu == nb_voisins){
         printf("Je suis le décideur : %d. Le minimum est : %d\n", rang, min_local);
         for (int i = 1; i < NB_SITE+1; i++){
            MPI_Send(&min_local, 1, MPI_INT, i, TAG_FIN, MPI_COMM_WORLD);
         }
         break;
      }
      /* Une fois qu'on a envoyé notre message, on attend un message du décideur si on ne l'est pas */
      else if (sent){
         MPI_Recv(&min_recu, 1, MPI_INT, MPI_ANY_SOURCE, TAG_FIN, MPI_COMM_WORLD, &status);
         min_local = min_recu;
         printf("Je suis %d et je ne suis pas décideur. Le minimum est : %d\n", rang, min_local);
         break;
      }
   }

   

}

void simulateur(void) {
   int i;

   /* nb_voisins[i] est le nombre de voisins du site i */
   int nb_voisins[NB_SITE+1] = {-1, 2, 3, 2, 1, 1, 1};
   int min_local[NB_SITE+1] = {-1, 3, 11, 8, 14, 5, 17};

   /* liste des voisins */
   int voisins[NB_SITE+1][3] = {{-1, -1, -1},
         {2, 3, -1},  {1, 4, 5}, 
         {1, 6, -1}, {3, -1, -1},
         {2, -1, -1}, {3, -1, -1}};
                               
   for(i=1; i<=NB_SITE; i++){
      MPI_Send(&nb_voisins[i], 1, MPI_INT, i, TAG_INIT, MPI_COMM_WORLD);    
      MPI_Send(voisins[i],nb_voisins[i], MPI_INT, i, TAG_INIT, MPI_COMM_WORLD);
      MPI_Send(&min_local[i], 1, MPI_INT, i, TAG_INIT, MPI_COMM_WORLD); 
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
