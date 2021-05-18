


#include "robot.h"
#include "../../infox_prose-x86_64-v0.3/include/infox/prose/prose.h"


#include <math.h>
#include <stdio.h>




struct Sensors{
    LightSensor *pSensor_Light;
    ContactSensor *pSensor_Avant;
    ContactSensor *pSensor_Dessous;    
};

struct Robot{
    Motor *mR;
    Motor *mL;
    SensorState_t sensorState;
    Sensors_t sensors;
};



static Robot_t * myRobot;


extern void robot_new(){

    myRobot = calloc(1, sizeof(Robot_t));

    if (myRobot == NULL){
        perror("robot_new()");
        exit(EXIT_FAILURE);
    }

    //Pas besoin d'init valeurs -> calloc met tout à 0 
    //enumerations init avec 0
}

extern void robot_free(){
    free(myRobot);
}

extern void robot_start(){

#ifdef INTOX
    if (ProSE_Intox_init("127.0.0.1", 12345) == -1) {
        PProseError("Problème d'initialisation du simulateur Intox\n");
        printf("\n Lancer le simulateur Intox ! \n");
        exit(EXIT_FAILURE);
    }
#endif

   // connection and check motors
    myRobot->mR = (Motor*) Motor_open(MD);
    myRobot->mL = (Motor*) Motor_open(MA);

    // connection and check sensors
    myRobot->sensors.pSensor_Light = LightSensor_open(S1);
    myRobot->sensors.pSensor_Avant = ContactSensor_open(S2);
    myRobot->sensors.pSensor_Dessous = ContactSensor_open(S3);

    if (myRobot->mR == NULL) PProseError("Problème de connexion au moteur droit (port MD)");
    if (myRobot->mL == NULL) PProseError("Problème de connexion au moteur gauche (port MA)");
    if ((myRobot->sensors.pSensor_Light) == NULL) PProseError("S1 Problème de connexion au capteur de luminosité");
    if ((myRobot->sensors.pSensor_Avant) == NULL) PProseError("S2 Problème de connexion au capteur d'obstacle");
    if ((myRobot->sensors.pSensor_Dessous) == NULL) PProseError("S3 Problème de connexion au capteur de sol");

}

extern void robot_stop(){

    // stop communication and close ports of sensors
    if (LightSensor_close(myRobot->sensors.pSensor_Light) == -1)
        PProseError("S1 Problème de deconnexion au capteur de luminosité");
    if (ContactSensor_close(myRobot->sensors.pSensor_Avant) == -1)
        PProseError("S2 Problème de deconnexion au capteur d'obstacle");
    if (ContactSensor_close(myRobot->sensors.pSensor_Dessous) == -1)
        PProseError("S3 Problème de deconnexion au capteur de sol");

    // stop communication and close ports of motors
    if (Motor_close(myRobot->mR) == -1) PProseError("Problème de deconnexion au moteur droit (port MD)");
    if (Motor_close(myRobot->mL) == -1) PProseError("Problème de deconnexion au moteur gauche (port MA)");

    //stop communication with simulator
    ProSE_Intox_close();
}

extern int robot_getRobotSpeed() {
    // speed of the robot (positive average of the right's and left's current wheel power)
    // (between 0 and 100)
    return fabs(Motor_getCmd(myRobot->mL) + Motor_getCmd(myRobot->mR)) / 2;
}


extern SensorState_t robot_updateSensorState() {
    // Luminosity getter
    myRobot->sensorState.luminosity = (float) LightSensor_getStatus(myRobot->sensors.pSensor_Light); //lum = mV [0-2500] 

    // Bump test
    if (ContactSensor_getStatus(myRobot->sensors.pSensor_Avant) == RELEASED &&
        ContactSensor_getStatus(myRobot->sensors.pSensor_Dessous) == RELEASED)
        myRobot->sensorState.collision = NO_BUMP;
    else if (ContactSensor_getStatus(myRobot->sensors.pSensor_Avant) == PRESSED ||
             ContactSensor_getStatus(myRobot->sensors.pSensor_Dessous) == PRESSED)
        myRobot->sensorState.collision = BUMP;

    // return the captor's states of the bumper and the luminosity
    return myRobot->sensorState;
}

extern void robot_setWheelsVelocity(int mr, int ml) {
    if (Motor_setCmd(myRobot->mR, mr) == -1) {
        PProseError("Problème de commande du moteur droit");
    }
    if (Motor_setCmd(myRobot->mL, ml) == -1) {
        PProseError("Problème de commande du moteur gauche");
    }
}

extern Sensors_t robot_getSensors(){
    return myRobot->sensors;
}

extern Robot_t * robot_getRobot(){
    return myRobot;
}

