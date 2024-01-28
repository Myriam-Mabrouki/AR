#include "mpi_server.h"
static server the_server;

void callback(int tag, int source){
    
}

//ICI CODE THREAD SERVEUR A COMPLETER

void start_server(void (*callback)(int tag, int source)){
    the_server.callback=callback(tag,source);
}

void destroy_server(){

}

pthread_mutex_t* getMutex(){
    return &the_server.mutex;
}