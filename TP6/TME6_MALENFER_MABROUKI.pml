#define N      3    /* Number of processes */ 
#define L     10    /* Buffer size */
#define NIL (N+1)   /* for an undefined value */

#define dmd_SC (node[1]@ask_SC)
#define en_SC (node[1]@in_SC)

mtype = {req, tk};

/* le processus i recoit ses messages dans canal[i] */ 
chan canal[N] = [L] of {mtype, byte};    /* pour un message de type tk, 
                                            on mettra la valeur associee a 0 */

byte SharedVar = 0;                      /* la variable partagee */ 

inline Initialisation ( ) {
   /* initialise les variables locales, sauf reqId, val et typ_mes */
      
   requesting = false;    
   next = NIL;    
   father = 0;
   token = (father == id);
   if
      :: (father == id) -> father = NIL;
      :: else -> skip;
   fi;        
          
}

inline Request_CS ( ) {
  /* operations effectuees lors d'une demande de SC */
   mtype t;
   requesting = true;
   if
      :: (father!=NIL) -> canal[father]!req(id); father = NIL;
      :: else -> skip;
   fi;
   
   canal[id]?t(tk);
}

inline Release_CS ( ) {
   /* operations effectuees en sortie de SC */
   requesting = false;
   if
      :: (next!=NIL) -> canal[next]! tk(0); token = false; next = NIL;
      :: else -> skip;
   fi;
}

inline Receive_Request_CS ( ) {
  /* traitement de la reception d'une requete */
   if
      :: (father == NIL)-> 
         if
            :: (requesting == true) -> next = reqId;
            :: else -> token = false; canal[reqId]!tk(0);
         fi;
      :: else -> canal[father]!req(reqId); 
   fi;      
   father = reqId;
}

inline Receive_Token () {
  /* traitement de la reception du jeton */
  token = true;       
}


proctype node(byte id; byte Initial_Token_Holder){
    
   bit requesting;    /* indique si le processus a demande la SC ou pas  */
   bit token;         /* indique si le processus possede le jeton ou non */

   byte father;       /* probable owner */
   byte next;         /* next node to whom send the token */
   byte val;          /* la valeur contenue dans le message */
   mtype typ_mes;     /* le type du message recu */
   byte reqId;        /* l'Id du demandeur, pour une requete */

   chan canal_rec = canal[id];     /* un alias pour mon canal de reception */
   xr canal_rec;                   /* je dois etre le seul a le lire */

   /* Chaque processus execute une boucle infinie */

   Initialisation ( );
   do
       :: ((token == true) && empty(canal_rec) && (requesting == true)) ->
	      /* on oblige le detenteur du jeton a consulter les messages recus */
	      in_SC :
	            /* acces a la ressource critique (actionszafq sur SharedVar), 
                  puis sortie de SC */
               SharedVar = SharedVar + 1;
               Release_CS ();
               assert(SharedVar==1);
               SharedVar = SharedVar - 1; 
                

       :: canal_rec ? typ_mes(val) ->
	            /* traitement du message recu */ 
               reqId = val;
               Receive_Request_CS ();

       :: (requesting == false) -> /* demander la SC */
            ask_SC:
               Request_CS ( );
   od ;
}



/* Cree un ensemble de N processus */ 

init {
   byte proc; 
   atomic {
      proc=0;
      do
         :: proc <N ->
            run node(proc, 0); 
            proc++
         :: proc == N -> break 
      od
   }
   ltl abs_famine {(always (dmd_SC implies (eventually en_SC)))}
}
