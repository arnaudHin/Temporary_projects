
/**
 * Autor : Arnaud Hincelin
 * File : facteurCommando.c
 */


#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "../../../commun.h"
#include "../../../utils.h"


typedef struct{
    MySocket_t mySocket_serveur;
}FacteurCommando_t;


static FacteurCommando_t * myFacteurCommando;


extern void FacteurCommando_New(){

    myFacteurCommando = NULL;
    int8_t check = 0;
    errno = 0;
    myFacteurCommando = calloc(1, sizeof(FacteurCommando_t));
    
    STOP_ON_ERROR(myFacteurCommando == NULL);

    myFacteurCommando->mySocket_serveur.mySocketListen = socket(AF_INET, SOCK_STREAM, 0); //creation du socket d'écoute du serveur

    if (myFacteurCommando->mySocket_serveur.mySocketListen == SOCKET_ERROR){
        perror("server create socket() ");
        exit(EXIT_FAILURE);         
    }
    
    myFacteurCommando->mySocket_serveur.mon_adresse.sin_addr.s_addr = htonl(INADDR_ANY); // serveur => accepte n'importe quelle adresse
    myFacteurCommando->mySocket_serveur.mon_adresse.sin_family = AF_INET; //adresse de famille internet
    myFacteurCommando->mySocket_serveur.mon_adresse.sin_port = htons(PORT_DU_SERVEUR_SOCKET); // port d'écoute du serveur

    errno = 0;

    //lier socket à interface de connexion
    int bind_erro = bind(myFacteurCommando->mySocket_serveur.mySocketListen, ( struct sockaddr*) &myFacteurCommando->mySocket_serveur.mon_adresse, sizeof(myFacteurCommando->mySocket_serveur.mon_adresse));

    if ( bind_erro == SOCKET_ERROR){
        perror("bind() ");
        exit(EXIT_FAILURE); 
    }

}


extern uint8_t FacteurCommando_Start(){

    int listen_erro = listen(myFacteurCommando->mySocket_serveur.mySocketListen, MAX_PENDING_CONNECTIONS);

    //ecouter les conexions entrantes
    if( listen_erro == SOCKET_ERROR){
        perror("listen() ");
        exit(EXIT_FAILURE);
    }

    PRINT("Attente de télécommande...\n");

    myFacteurCommando->mySocket_serveur.mySocketCom = accept(myFacteurCommando->mySocket_serveur.mySocketListen, NULL, 0);
    
    if ( myFacteurCommando->mySocket_serveur.mySocketCom == SOCKET_ERROR){ //vérfie si le socket de communication a bien été init
        perror("accept() ");
        exit(EXIT_FAILURE); 
    }

    printf("Télécommande correctement connecté\n");   

    listen_erro = close(myFacteurCommando->mySocket_serveur.mySocketListen);

     if( listen_erro == SOCKET_ERROR){
    perror("close socketListen() ");
    exit(EXIT_FAILURE);  
    }

}


extern void FacteurCommando_Stop(){

    while (1)
    {
        
    }
    


    errno = 0;

    int closeSocket_erro = close(myFacteurCommando->mySocket_serveur.mySocketCom);

    if ( closeSocket_erro == SOCKET_ERROR){
        perror("close socketCom() ");
        exit(EXIT_FAILURE);    
    }

}




