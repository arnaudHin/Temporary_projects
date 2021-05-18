

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

#include "client.h"
#include "../../commun.h"
#include "../../commando/src/robot.h"

//VARIABLES PRIVEES

static MySocket_t mySocket_client;
static char client_IpAdress[] = "000.1.0.0"; //10 car fin de chaine = retour chariot (\0)

//PROTOTYPES DE FONCTION STATIQUES
static void socket_client_start();


static void socket_client_start(){

    mySocket_client.mySocketCom = socket(AF_INET, SOCK_STREAM, 0); //créer socket du client

    if (mySocket_client.mySocketListen == SOCKET_ERROR){
        perror("client create socket() ");
        exit(EXIT_FAILURE);         
    }

    mySocket_client.mon_adresse.sin_family = AF_INET; //adresse de famille intetnet (vs = PF_INET, protocole)

    mySocket_client.mon_adresse.sin_port = htons(PORT_DU_SERVEUR_SOCKET); //convertir le nb de port du serveur au format du réseau

    mySocket_client.mon_adresse.sin_addr = *( ( struct in_addr*)gethostbyname(client_IpAdress)->h_addr_list[0]); //IP (nom de domaine) auquel on doit se connecter

    printf("connexion au serveur...\n");

    //demande de connexion auprès du serveur
    if (connect(mySocket_client.mySocketCom, (struct sockaddr*)&mySocket_client.mon_adresse, sizeof(mySocket_client.mon_adresse) ) == SOCKET_ERROR ){
        perror("connect() ");
        exit(EXIT_FAILURE); 
    }

    printf("Connexion au serveur réussie\n"); //succès de connexion au serveur
}



extern void client_sendMsg_socketEvent(EventSocket_e socketEvent){

    EventSocket_e socketEvent_temp = htonl(socketEvent);

    if ( write(mySocket_client.mySocketCom, &socketEvent_temp, sizeof(socketEvent_temp)) == SOCKET_ERROR ){  
        perror("write client() ");
        exit(EXIT_FAILURE);     
    }
}

extern void client_sendMsg_velocity(Direction_e direction){

    Direction_e direction_temp = htonl(direction);

    if ( write(mySocket_client.mySocketCom, &direction_temp, sizeof(direction_temp)) == SOCKET_ERROR ){  
        perror("write client() ");
        exit(EXIT_FAILURE);     
    }
}



extern PilotState_t client_readMsg_pilotState(){

    PilotState_t pilotState;

    if ( read(mySocket_client.mySocketCom, &pilotState, sizeof(pilotState)) == SOCKET_ERROR ){  
        perror("write client() ");
        exit(EXIT_FAILURE);     
    }
    return pilotState;
}



extern void client_start(){

    socket_client_start();
}

extern void client_stop(){

    if (close(mySocket_client.mySocketCom) == SOCKET_ERROR){
        perror("close socketListen() ");
        exit(EXIT_FAILURE);    
    }
}


extern void client_setIp(char * myIP){

    strcpy(client_IpAdress, myIP); //copie de chaine de caractère (string.h) -> pointeur vers premier élément de la chaine
}


