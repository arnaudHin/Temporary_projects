
//Nathan Brient

#ifndef ROBOT_H
#define ROBOT_H
#include <stdlib.h>
#include <stdio.h>

//#include "../../infox_prose-x86_64-v0.3/include/infox/prose/motors.h" //va inclure motors.h, contact_sensors.h ...
#include "prose.h"
#include "../../commun.h"
//#include "commun.h"

//enumeration pour savoir si le robot detecte un obstacle


/** the captor's states of the robot (bumper and luminosity) */


typedef struct //structure qui va prendre les différents capteurs
{
    LightSensor *lightSensor;
    ContactSensor *contactSensor1;
    ContactSensor *contactSensor2;
} Sensors;

// défintion de la structure Robot qui prend les deux moteurs et les sensors
typedef struct
{
    SensorState sensorsState;
    Sensors sensors;
    Motor *mD;
    Motor *mG;
} Robot;

//Déclaration des fonction globales
/**
 * Start the Robot (initialize communication and open port)

 */
extern void Robot_start();

/**
 * Stop Robot (stop communication and close port)
 */
extern void Robot_stop();

/**
 * @briel initialize in memory the object Robot
 * 
 */
extern void Robot_new();

/**
 *  @brief destruct the object Robot from memory 
 */
extern void Robot_free();

/**
 * Robot_getRobotSpeed
 * 
 * @brief return the speed of the robot (positive average of the RIGHT_D's and left's current wheel power) 
 * @return speed of the robot (beetween 0 and 100)
 */
extern float Robot_getRobotSpeed();

/**
 * Robot_getSensorState
 * 
 * @brief return the captor's states of the bumper and the luminosity
 * @return SensorState
 */
extern SensorState Robot_getSensorState();

/**
 * Robot_setWheelsVelocity
 * 
 * @brief set the power on the wheels of the robot
 * @param int mr : RIGHT_D's wheel power, value between -10O and 100
 * @param int ml : left's wheel power, value between -100 and 100
 */
extern void Robot_setWheelsVelocity(int mr, int ml);

/**partie ajoutée */

/**
 * Destructeur de robot.
 *
 * \param robot le robot à détruire.
 */
void robot_D(Robot *robot);

/**partie ajoutée */

/**
 * Consignes sur les roues du robot.
 *
 * \param robot le robot auquel s'adressent les consignes.
 * \param roueD consigne pour la roue droite (moteur droit).
 * \param roueG consigne pour la roue gauche (moteur gauche).
 */
void robot_consignes(Robot *robot, int roueD, int roueG);

/**partie ajoutée */

/**
 * Affiche les informations sur les capteurs du robot.
 *
 * \param robot le robot concerné.
 */
void robot_affiche_informations(Robot *robot);

#endif /* ROBOT_H */
