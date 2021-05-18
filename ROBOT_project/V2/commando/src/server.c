

#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "server.h"
#include "pilot.h"
#include "../../commun.h"

//VARIABLES PRIVEES
static MySocket_t mySocket_server;
static uint8_t ConnexionServer = CONNEXION_ARRET;

//PROTOTYPES DE FONCTIONS
static void socket_server_start();
static EventSocket_e server_readMsg_event();
static void server_sendMsg_pilotState();
static Direction_e server_readMsg_velocity();

static void socket_server_run();
static void server_runCommunication();


static void socket_server_start(){

    printf(KNRM FRED"--- COMMANDO - Serveur ---");
    printf(KNRM "\n");

    errno = 0;

    mySocket_server.mySocketListen = socket(AF_INET, SOCK_STREAM, 0); //creation du socket d'écoute du serveur

    if (mySocket_server.mySocketListen == SOCKET_ERROR){
        perror("server create socket() ");
        exit(EXIT_FAILURE);         
    }
    
    mySocket_server.mon_adresse.sin_addr.s_addr = htonl(INADDR_ANY); // serveur => accepte n'importe quelle adresse

    mySocket_server.mon_adresse.sin_family = AF_INET; //adresse de famille internet

    mySocket_server.mon_adresse.sin_port = htons(PORT_DU_SERVEUR_SOCKET); // port d'écoute du serveur

    errno = 0;

    //lier socket à interface de connexion
    int bind_erro = bind(mySocket_server.mySocketListen, ( struct sockaddr*) &mySocket_server.mon_adresse, sizeof(mySocket_server.mon_adresse));

    if ( bind_erro == SOCKET_ERROR){
        perror("bind() ");
        exit(EXIT_FAILURE); 
    }

    errno = 0;

    int listen_erro = listen(mySocket_server.mySocketListen, MAX_PENDING_CONNECTIONS);

    //ecouter les conexions entrantes
    if( listen_erro == SOCKET_ERROR){
        perror("listen() ");
        exit(EXIT_FAILURE);
    }

    printf("Attente de télécommande...\n");

    errno = 0;

    //accept() renvoie un nouveau socket, qui servira à la communication client/server
    mySocket_server.mySocketCom = accept(mySocket_server.mySocketListen, NULL, 0);
    
    if ( mySocket_server.mySocketCom == SOCKET_ERROR){ //vérfie si le socket de communication a bien été init
        perror("accept() ");
        exit(EXIT_FAILURE); 
    }

    printf("Télécommande correctement connecté\n");

    errno = 0;

    //ferme le socket sur lequel on écoutait la demande de connexion
    if (close(mySocket_server.mySocketListen) == SOCKET_ERROR){
        perror("close socketListen() ");
        exit(EXIT_FAILURE);    
    }

    //connexion OK -> met à jour la variable connexion
    ConnexionServer = CONNEXION_MARCHE;
}




static void socket_server_run(){

    while (ConnexionServer) //tant que Connexion est en cours, on communique avec le client
    {
        server_runCommunication();
        sleep(0.1);
    }

}



static EventSocket_e server_readMsg_event(){

    EventSocket_e socketEvent;

    errno = 0;

    int read_erro = read(mySocket_server.mySocketCom, &socketEvent, sizeof(socketEvent));

    if ( read_erro == SOCKET_ERROR ){  
        perror("read server() ");
        exit(EXIT_FAILURE);     
    }
    return ntohl (socketEvent);
}


static Direction_e server_readMsg_velocity(){

    Direction_e direction;

    errno = 0;

    int readVel_erro = read(mySocket_server.mySocketCom, &direction, sizeof(direction));

    if ( readVel_erro == SOCKET_ERROR ){  
        perror("read server() ");
        exit(EXIT_FAILURE);     
    }
    return ntohl(direction);
}



static void server_sendMsg_pilotState(){
    
    PilotState_t pilotState = (mySocket_server.myData.myPilotState);

    errno = 0;

    int readPilt_erro = write(mySocket_server.mySocketCom, &pilotState, sizeof(pilotState));

    if ( readPilt_erro == SOCKET_ERROR ){  
        perror("write server() ");
        exit(EXIT_FAILURE);     
    }
}


static void server_runCommunication(){

    EventSocket_e eventSocket = server_readMsg_event();

    switch (eventSocket)
    {
    case E_SCK_SET_VEL:
        pilot_RunVelocity( server_readMsg_velocity() );
        break;

    case E_SCK_CLOSE:
        printf("\nFermeture de myCommando en cours...\n");
        ConnexionServer = CONNEXION_ARRET;
        pilot_run(E_CLOSE);
        break;

    case E_SCK_LOG:
        server_updatePilotState();
        server_sendMsg_pilotState();
        break;

    default:
        break;
    }
}




extern void server_stop(){

    errno = 0;

    int closeSocket_erro = close(mySocket_server.mySocketCom);

    if ( closeSocket_erro == SOCKET_ERROR){
        perror("close socketCom() ");
        exit(EXIT_FAILURE);    
    }
}





extern void server_start(){

    //lancer le serveur socker
    socket_server_start();

    socket_server_run();

}



extern void server_updatePilotState(){
    mySocket_server.myData.myPilotState = pilot_getPilotState();
}


