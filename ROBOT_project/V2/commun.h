
#ifndef COMMUN_H
#define COMMUN_H

#include <sys/socket.h>
#include <netinet/in.h>
#include "commando/src/pilot.h"

#define SOCKET_ERROR -1
#define PORT_DU_SERVEUR_SOCKET (12349)
#define MAX_PENDING_CONNECTIONS (1)

#define CONNEXION_MARCHE 1
#define CONNEXION_ARRET 0

#define KNRM  "\x1B[0m"
#define KCYN  "\x1B[36m"

#define FRED  "\x1B[41m"
#define FBLU  "\x1B[44m"

typedef enum{
  E_SCK_SET_VEL=0,
  E_SCK_LOG,
  E_SCK_CLOSE,
  E_SCK_NOP,
  NB_SCK_EVENT
}EventSocket_e;

typedef struct{
    Direction_e myDirection;
    PilotState_t myPilotState;
}MyData_t;


typedef struct{
	  int mySocketListen;
	  int mySocketCom; 
    MyData_t myData;
  	struct sockaddr_in mon_adresse;
}MySocket_t;


#endif // COMMUN_H


