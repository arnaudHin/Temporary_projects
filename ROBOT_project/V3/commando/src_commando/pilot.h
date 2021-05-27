//Nathan Brient

#ifndef PILOT_H
#define PILOT_H

#include "robot.h"
#include "./../../commun.h"








    
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
    //State etat;
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

extern void toggleES();

#endif /* PILOT_H */

