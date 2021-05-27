/**
 * @file commun.h
 * @author Adrien, Arnaud et Briac
 * @brief 
 * @version 0.1
 * @date 2021-05-26
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef COMMUN_H
#define COMMUN_H

#define PORT_DU_SERVEUR (1234)
#define MAX_PENDING_CONNECTIONS (5)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>


#define SOCKET_ERROR -1
#define PORT_DU_SERVEUR_SOCKET (12349)
#define MAX_PENDING_CONNECTIONS (1)

#define CONNEXION_MARCHE 1
#define CONNEXION_ARRET 0

#define KNRM  "\x1B[0m"
#define KCYN  "\x1B[36m"

#define FRED  "\x1B[41m"
#define FBLU  "\x1B[44m"
//les directions que peut prendre le robot
typedef enum {STOP_D=0,LEFT_D, RIGHT_D, FORWARD_D, BACKWARD_D} Direction;

//enumération demande qui va prendre la requete du client(telco)
typedef enum
{
  MOUVEMENT=0,
  QUITTER,
  LOG,
  EFFACER_LOG
} Demande;


typedef struct
{
    SensorState sens;
    Speed speed;
} Eventa;

//structure DesDonnes qui va prendre la demande du client et la direction que doir prendre le robot
typedef struct
{
  Demande demande;
  Direction direction;
} DesDonnees;
typedef struct
{
    Collision collision;
    float luminosity;
} SensorState;

typedef struct 
{
    float speed;
}Speed;
typedef struct{



}MyData_t;


typedef struct{
	  int mySocketListen;
	  int mySocketCom; 
    MyData_t myData;
  	struct sockaddr_in mon_adresse;
}MySocket_t;



#endif /* __COMMON_H */































