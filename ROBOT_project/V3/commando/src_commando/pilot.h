//Nathan Brient

#ifndef PILOT_H
#define PILOT_H

#include "robot.h"



// pour la machine à état
//etat 
typedef enum {
    NONE_S=0,//ne rien faire
    FINAL_S, //stoper la MAE 
   NORMALIDLE_S, //pas en mouvement
    NORMAL_FROM_RUNNING_S, //en mouvement
    NORMAL_BUMPED_S,
    EMERGENCY_S,
    NB_ETAT_S //pour avoir le nombre d'etats
} State; 

//evenement 
typedef enum {
    STOP_E=0, //
    CHECK_E, //on check
    SET_VELOCITY_E, //
    TOGGLEES_E,
    BUMPED_E,
    NO_BUMPED_E,
    COND_TRUE_E,
    SET_ROBOT_VEL_E,
    TI_OUT_E,
    NB_EVENT_E //
} Event; 

//Action a faire
typedef enum {
    NONE_A=0, //ne rien faire
    NORMAL_IDLE_TO_EMERG_A, //pour stopper la MAE
    NORMAL_IDLE_TO_NORUN_A, //pour envoyer le mouvement
    NORUN_TO_NORMAL_IDLE_A,
    NORUN_TO_NOBUMP_A,
    NO_RUN_TO_EMER_A,
    NO_BUMP_TO_EMERG_A,
    NO_BUMP_TO_NORUN_A,
    EMERGENCY_TO_NORIDLE_A,
    NORUN_NORUN_A,
    NB_ACTION,
    CHECK_A //pour checker les collisions, luminosité ...
}Action;

//structure tansition prenant l'etat de setsination et l'action pour y parvenir
typedef struct
{
State etatDestination;
Action action;
}Transition;


    
typedef struct
{
    Direction dir;
    int power;
} VelocityVector;

typedef struct
{
    int speed;
    int collision; //1 si collision 0 si pas collision
    float luminosity;
} PilotState;

//structure pilot
typedef struct
{
    PilotState pilotState;
    VelocityVector velocityVector;
    Robot robot;
    State etat;
}Pilot;


extern Pilot * pilot;




/**
 * START_E Pilot
 *
 */
extern void Pilot_start();
/*
extern void ContactCheck();
extern void LightCheck();

*/
/**
 * Stop Pilot
 *
 */
extern void Pilot_stop();

extern VelocityVector velocity1;

/**
 * initialize in memory the object Pilot
 */
extern void Pilot_new();


/**
 * destruct the object Pilot from memory 
 */
extern void Pilot_free();

/**
 * setVelocity
 * 
 * @brief description 
 * @param vel 
 */
extern void Pilot_setVelocity(VelocityVector vel);

/**
 * getState
 * 
 * @brief description 
 * @return PilotState
 */
extern PilotState Pilot_getState();


/**
 * check
 * 
 * @brief description 
 */
extern void check();



#endif /* PILOT_H */

