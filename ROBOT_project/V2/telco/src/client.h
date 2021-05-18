


#ifndef CLIENT_H
#define CLIENT_H

#include "../commun.h"

/**
 * Démarrer le client et la communication avec le serveur
 */
extern void client_start();

/**
 * Stopper le client
 */
extern void client_stop();

/**
 * client envoie un message de type EventSocket_e
 */
extern void client_sendMsg_socketEvent(EventSocket_e socketEvent);

/**
 * client envoie un message de type Direction_e
 */
extern void client_sendMsg_velocity(Direction_e direction);

/**
 * client reçoit un message de type PilotState_t
 */
extern PilotState_t client_readMsg_pilotState();

/**
 * remplir le nim de domaine (IP) de connexion
 */
extern void client_setIp(char * myIP);

#endif /* CLIENT_H */




