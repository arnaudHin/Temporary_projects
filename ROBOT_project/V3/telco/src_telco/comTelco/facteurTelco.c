/**
 * Autor : Arnaud Hincelin
 * File : facteurTelco.c
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
    MySocket_t mySocket_client;
}FacteurTelco_t;


static FacteurTelco_t * myFacteurTelco;
static char FacteurTelco_IpAdress[] = "000.0.0.0"; //10 car fin de chaine = retour chariot (\0)

//PROTOTYPES DE FONCTION STATIQUES




extern void FacteurTelco_New(){

    myFacteurTelco = NULL;
    int8_t check = 0;
    errno = 0;
    myFacteurTelco = calloc(1, sizeof(FacteurTelco_t));
    
    STOP_ON_ERROR(myFacteurTelco == NULL);


    myFacteurTelco->mySocket_client.mySocketCom = socket(AF_INET, SOCK_STREAM, 0); //créer socket du client

    STOP_ON_ERROR(myFacteurTelco->mySocket_client.mySocketListen == SOCKET_ERROR);

    myFacteurTelco->mySocket_client.mon_adresse.sin_family = AF_INET; //adresse de famille intetnet (vs = PF_INET, protocole)
    myFacteurTelco->mySocket_client.mon_adresse.sin_port = htons(PORT_DU_SERVEUR_SOCKET); //convertir le nb de port du serveur au format du réseau

}


extern uint8_t FacteurTelco_Start(){

    int8_t check = 0;

    printf("connexion au serveur %s en cours...\n", FacteurTelco_IpAdress);

    PRINT("IP : %s", FacteurTelco_IpAdress);

    myFacteurTelco->mySocket_client.mon_adresse.sin_addr = *( ( struct in_addr*)gethostbyname(FacteurTelco_IpAdress)->h_addr_list[0]); //IP (nom de domaine) auquel on doit se connecter

    check = connect(myFacteurTelco->mySocket_client.mySocketCom, (struct sockaddr*)&myFacteurTelco->mySocket_client.mon_adresse, sizeof(myFacteurTelco->mySocket_client.mon_adresse) ); 
    //demande de connexion auprès du serveur
    if( check == SOCKET_ERROR){
        return 1;
    }
    return 0;
}


extern void FacteurTelco_Stop(){

    int8_t check = 0;

    check = close(myFacteurTelco->mySocket_client.mySocketCom);
    STOP_ON_ERROR(check == SOCKET_ERROR);
}

extern void FacteurTelco_SetIP(char * myIP){

    strcpy(FacteurTelco_IpAdress, myIP);
}





