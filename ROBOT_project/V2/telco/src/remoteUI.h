

#ifndef REMOTEUI_H
#define REMOTEUI_H

typedef enum remoteUIState remoteUIState_e;
typedef struct RemoteUI RemoteUI_t;

/**
 * Démarrer remoteUI
 */
extern void remoteUI_start(); 

/**
 * Stopper remoteUI
 */
extern void remoteUI_stop(); 

/**
 * Allouer mémoire pour remoteUI
 */
extern void remoteUI_new(); 

/**
 * Détruire mémoire de remoteUI
 */
extern void remoteUI_free(); 

/**
 * fonction principale du fonctionnement de remoteUI
 */
extern void run_Telecommande();

/**
 * fonction principale de fermeture de remoteUI et du client
 */
extern void close_Telecommande();


#endif /* REMOTEUI_H */



