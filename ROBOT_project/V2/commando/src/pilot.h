

#ifndef PILOT_H
#define PILOT_H

//DEFINITIONS DES ENUMERATIONS PRIVEES
typedef enum State State_e;
typedef enum Action Action_e;

//DEFINITIONS DES ENUMERATIONS P
typedef enum{
    INIT=0,
    RIGHT,
    FORWARD,
    BACKWARD,
    STOP,
    LEFT
}Direction_e;

typedef enum {
    E_STOP=0,
    E_ASK_VELOCITY,
    E_CLOSE,
    NB_EVENT    
}Event_e;

typedef struct{
    int speed;
    int collision;
    float luminosity;
}PilotState_t;

typedef struct VelocityVector VelocityVector_t;
typedef struct Pilot Pilot_t;
typedef struct Transition Transition_t;

/**
 * Allouer mémoire de pilot
 */
extern void pilot_new();

/**
 * Lancer le pilot
 */
extern void pilot_start();

/**
 * effacer espace mémoire de instance pilot
 */
extern void pilot_free();

/**
 * déterminer action à effectuer selon event et état du pilot
 */
extern void pilot_run(Event_e event);

/**
 * Setter de direction
 */
extern void pilot_setDirection(Direction_e direction);

/**
 * Setter de event
 */
extern void pilot_setEvent(Event_e event);

/**
 * demander un changement de direction
 */
extern void pilot_RunVelocity(Direction_e direction);

/**
 * demander un arret de fonctionnement de pilot et du serveur
 */
extern void pilot_RunClose();

/**
 * fonction principale pour le fonctionnement de commando
 */
extern void run_MyCommando();

/**
 * onction principale pour fermer commando
 */
extern void stop_MyCommando();

/**
 * getter de pilot
 */
extern PilotState_t pilot_getPilotState();


#endif



