/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#include "robot.h"
//#include "pilot.h"

//définitions macros
#define LEFT_MOTOR MD
#define RIGHT_D_MOTOR MA
#define LIGHT_SENSOR S1
#define FRONT_BUMPER S3
#define FLOOR_SENSOR S2
#define ROBOT_CMD_STOP (0)

//VelocityVector velocity1;

Robot * robotInit;


/**
 * allocation de la mémoire du robot
 *
 * \see robot.h
 */

void Robot_new() //initialisation du robot
{
	robotInit = (Robot *)calloc(1,sizeof(Robot));
	robotInit->sensorsState.collision=NO_BUMP;
	robotInit->sensorsState.luminosity=0;

}

// On démarre le robot et on établit la connexions avec les moteurs ...
void Robot_start()
{

	// Initialisation des moteurs et du robot
	printf("je suis dans pilot start");

	robotInit->mD = Motor_open(MD); //moteur droit
	robotInit->mG = Motor_open(MA); //moteur gauche

	if (robotInit->mD == NULL)
		PProseError("Problème d'ouverture du moteur droit (port MD)");
	if (robotInit->mG == NULL)
		PProseError("Problème d'ouverture du moteur gauche (port MA)");

	//initialisation des sensors collision et luminosity
	robotInit->sensors.lightSensor=LightSensor_open(S1);
	if ((robotInit->sensors.lightSensor) == NULL) PProseError("Problème d'ouverture du port S1:lightSensor");
    robotInit->sensors.contactSensor1 = ContactSensor_open(S2);
    if ((robotInit->sensors.contactSensor1) == NULL) PProseError("Problème d'ouverture du port S2:contactSensor1");
    robotInit->sensors.contactSensor2 = ContactSensor_open(S3);
    if ((robotInit->sensors.contactSensor2) == NULL) PProseError("Problème d'ouverture du port S3:contactSensor2");

}

/** Stop Robot (stop communication and close port)*/
void Robot_stop()
{

	// on stop les deux moteurs pour que le robot se stop
	if (Motor_setCmd(robotInit->mD, ROBOT_CMD_STOP) == -1)
	{
		PProseError("Problème de commande du moteur droit");
	}
	if (Motor_setCmd(robotInit->mG, ROBOT_CMD_STOP) == -1)
	{
		PProseError("Problème de commande du moteur gauche");
	}
	//fermer les deux moteur avec motor_close /
	if (Motor_close(robotInit->mD) == -1)
	{
		PProseError("Problème fermeture moteur gauche");
	}
	if (Motor_close(robotInit->mG) == -1)
	{
		PProseError("Problème fermeture moteur gauche");
	}

	//fermer les capteurs avec LightSensor_close et ContactSensor_close /
	if (LightSensor_close(robotInit->sensors.lightSensor) == -1) {
		PProseError("Problème de fermeture du port S1:lightSensor");
	}
    if (ContactSensor_close(robotInit->sensors.contactSensor1) == -1){
		 PProseError("Problème de fermeture du port S2:contactSensor1");
	}
    if (ContactSensor_close(robotInit->sensors.contactSensor2) == -1){
		PProseError("Problème de fermeture du port S3:contactSensor2");
	}
}

/**destruct the object Robot from memory  */
void Robot_free()
{
	Robot_stop(robotInit);
	free(robotInit);
}

/**
 * @brief return the captor's states of the bumper and the luminosity
 * @return SensorState
 */
SensorState Robot_getSensorState()
{
	// on va aller chercher la luminosité
	if (LightSensor_getStatus(robotInit->sensors.lightSensor)==-1)
	{
		PProseError("Problème de status sur capteur de luminosité");
	}else{
		    robotInit->sensorsState.luminosity = (float) LightSensor_getStatus(robotInit->sensors.lightSensor); //lum = mV [0-2500] 
	}
	
    //on va aller chercher si les contact sont bump ou pas et mettre a un si au moins un est égal à 1
	if (ContactSensor_getStatus(robotInit->sensors.contactSensor1) == ERROR || ContactSensor_getStatus(robotInit->sensors.contactSensor2) == ERROR)
	{
		PProseError("Problème de status sur un des contact Sensor");
	}
    else if(ContactSensor_getStatus(robotInit->sensors.contactSensor1) == RELEASED && ContactSensor_getStatus(robotInit->sensors.contactSensor2) == RELEASED){
		robotInit->sensorsState.collision = NO_BUMP;
		//printf("je suis dans Robot_getSensorState et la collision est %d\n",robotInit->sensorsState.collision);

	}
    else if(ContactSensor_getStatus(robotInit->sensors.contactSensor1) == PRESSED || ContactSensor_getStatus(robotInit->sensors.contactSensor2) == PRESSED){
		robotInit->sensorsState.collision = BUMPED;
		//printf("je suis dans Robot_getSensorState et la collision est %d\n",robotInit->sensorsState.collision);

	}	
	return robotInit->sensorsState;

}


/**
 * @brief return the speed of the robot (positive average of the RIGHT_D's and left's current wheel power) 
 * @return speed of the robot (beetween 0 and 100)
 */
float Robot_getRobotSpeed()
{
	float s = 0;
	//if (pilot->velocityVector.dir != STOP_D)
	//{
	s = (Motor_getCmd(robotInit->mD) + Motor_getCmd(robotInit->mG)) / 2; 

	if (s < 0){ //valeur absolue j'ai pas trouvé comment faire l'absolue avec une fonction
		s = -s;
	}
	//}
	return s;
}

/**
 * @brief set the power on the wheels of the robot
 * @param int mr : RIGHT_D's wheel power, value between -10O and 100
 * @param int ml : left's wheel power, value between -100 and 100
 */
void Robot_setWheelsVelocity(int mr, int ml)
{
	printf("je suis dans Robot_setWheelsVelocity dans Robot \n");


	if (Motor_setCmd(robotInit->mD, mr) == -1)
	{
		PProseError("Erreur lors de la fonction Robot_setWheelsVelocity ");
	}
	if (Motor_setCmd(robotInit->mG, ml) == -1)
	{
		PProseError("Erreur lors de la fonction Robot_setWheelsVelocity ");
	}
}
