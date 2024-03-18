#define N 3

mtype = {top, bottom};
mtype = {ack, agr, mes, term};

chan canaux [N]= [N] of {mtype, int, byte};
   /* message = <type, val_horloge, emetteur> */

bool termine = 0;
   /* positionne a vrai quand la terminaison est detectee */


/*************************************************/

inline desactivation() {
   etat = bottom
   if (unack==0){
      h++;
      last = i;
      hfin = h;
      nbagr = 0;
      //broadcast
   }
}

/*************************************************/

/* VOUS POUVEZ AJOUTER D'AUTRES INLINE */

/*************************************************/

inline envoyer () {
   unack++;
   canaux[i]!mes(id ,h);
}

inline recevoir () {
   if
      :: (etat == bottom) -> etat = top;
      :: else -> skip;
   fi
   canaux[i]!ack(id,h);   
}


inline recv_ack(){
   unack--;
   h = h > hrec ? h : hrec;
   if
      :: ((unack==0) && (etat==bottom)) -> 
            h++; dernier = id; hfin = h; nbarg = 0; //broadcast
      :: else -> skip; 
   fi
}

inline recv_term(){
   if
      :: ((etat == top) || (unack > 0)) -> 
            h = h > hrec ? h : hrec;
      :: else -> 
         :: ((hrec > hfin) || (hrec == fin && i > dernier)) ->
               h = hrec; hfin = hrec; dernier = i;
   fi

}

inline recv_agr(){
   if
      :: (hrec == hfin) -> nbarg++;
         :: (nbarg == N-1) -> //annoncer terminaison
         :: else -> skip;
      :: else -> skip;
   fi
}





proctype un_site (byte id) {

   byte i, dernier;
   byte sdr, dest = (id+1)%N;

   int h, hrec, hfin;

   short unack = 0; /* le nombre de messages non acquittes */
   short nbagr; /* nombre d'accords sur la terminaison recus */
   
   mtype mes_tp, etat = top;
   
   h = 0; 
   do
      :: ( empty(canaux[id]) && (etat == actif) ) ->   
            if    
               :: (1) ->   /* on peut arreter les actions locales */
                  desactivation()

               :: (1) ->   /* on peut envoyer un message. A COMPLETER */
                  unack++;
                  canaux[TODO]!mes(id,h)
            fi
			
      :: nempty(canaux[id]) ->   /* on recoit un message. A COMPLETER */
            if
               r
            fi
   od
}

/*************************************************/

init {

   byte i; 
   atomic {
      i=0;
      do
         :: i <N ->
               run un_site(i); 
               i++
         :: i == N -> break 
      od
   }
}
