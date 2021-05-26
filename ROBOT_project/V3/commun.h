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

//structure DesDonnes qui va prendre la demande du client et la direction que doir prendre le robot
typedef struct
{
  Demande demande;
  Direction direction;
} DesDonnees;


#endif /* __COMMON_H */
