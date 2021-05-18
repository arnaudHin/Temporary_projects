

#ifndef SERVER_H
#define SERVER_H

#include "../../commun.h"
#include "pilot.h"

/**
 * démarrer le serveur et ouvri la communication
 */
extern void server_start();

/**
 * stopper le serveur
 */
extern void server_stop();

/**
 * 
 */
extern void server_setSocketEvent(EventSocket_e socketEvent);

/**
 * Mettre à jour la structure PilotState_t de myPilot
 */
extern void server_updatePilotState();

#endif /* SERVER_H */













