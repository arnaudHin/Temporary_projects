


#include <pthread.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "../../commun.h"
#include "robot.h"
#include "pilot.h"
#include "server.h"


//ENUMERATIONS PRIVEES

enum State{
    S_IDLE=0,
    S_RUNNING,
    NB_STATE
};

enum Action{
    A_NOP=0,
    A_STOP,
    A_SET_VELOCITY,
    A_CLOSE
};

//STRUCTURES PRIVEES

struct VelocityVector{
    Direction_e dir;
    int power;
};

struct Pilot{
    PilotState_t pilotState;
    VelocityVector_t velocityVector;
    Event_e event;
    State_e statePilotMAE;
};

struct Transition
{
    State_e destinationState;
    Action_e action;
};

//VARIABLES PRIVEES

static Transition_t mySystem[NB_STATE][NB_EVENT] = {
    
    [S_IDLE][E_STOP] = {S_IDLE, A_STOP},
    [S_IDLE][E_ASK_VELOCITY] = {S_RUNNING, A_SET_VELOCITY},
    [S_IDLE][E_CLOSE] = {S_IDLE, A_CLOSE},

    [S_RUNNING][E_STOP] = {S_IDLE, A_STOP},
    [S_RUNNING][E_ASK_VELOCITY] = {S_RUNNING, A_SET_VELOCITY},
    [S_RUNNING][E_CLOSE] = {S_IDLE, A_CLOSE},
};

static Pilot_t * myPilot;

//PROTOTYPES DE FONCTION

static void pilot_performAction(Action_e action);
static void sendMvt();
static int hasBumped();
static void pilot_updatePilotState();
static void * thread_sensors();



extern void pilot_new() {

    //allocation dynamique de myPilot
    myPilot = calloc(1, sizeof(Pilot_t));

    if (myPilot == NULL) {
        perror("pilot_new()");
        exit(EXIT_FAILURE);
    }

    //Pas besoin d'init valeurs -> calloc met tout à 0 
    //enumerations init avec 0
}




static void * thread_sensors(){

    while (myPilot->event != E_CLOSE) //tache vraie tant que pas de demande de fermeture
    {

        pilot_updatePilotState();

        if ( hasBumped() ){ //Si collision, demande sde stopper le robot
            pilot_setEvent(E_STOP);
            pilot_run(myPilot->event);
        }
        
        sleep(0.2); //temps de pause dans la tache pour ne pas surcharger les demandes de de sensors
    }

    pthread_exit(NULL);
}

extern void pilot_start(){
    pthread_t mySensorthread;

    pthread_create(&mySensorthread, NULL, thread_sensors, NULL);//lancer le thread appelant la fonction thread_sensors()

    if (myPilot->event == E_CLOSE)
    {
        pthread_cancel(mySensorthread);
    }
    
}

extern void pilot_free(){
    free(myPilot); //vider espace mémoire alloué pour myPilot
}

extern void pilot_run(Event_e event){
    Action_e aAction;
    State_e aState;

    //detertminer l'action à effectuer selon l'état et l'event de pilot

    aAction = mySystem[myPilot->statePilotMAE][event].action;
    aState = mySystem[myPilot->statePilotMAE][event].destinationState;

    pilot_performAction(aAction);
    myPilot->statePilotMAE = aState;
}


static void pilot_performAction(Action_e action){

    switch (action)
    {
    case A_SET_VELOCITY: //écrire une direction à ordonner au robot
        sendMvt();
        break;
    case A_STOP: //stoppet le robot
        myPilot->velocityVector.dir = STOP;
        myPilot->velocityVector.power = 0;
        sendMvt();
        break;
    case A_CLOSE: // demande d'arret de programme
        pilot_setDirection(STOP);
        sendMvt();
        break;

    default:
        break;
    }
}


static void sendMvt(){
    
    switch (myPilot->velocityVector.dir) {
        case FORWARD:
            myPilot->velocityVector.power = 80;
            robot_setWheelsVelocity(myPilot->velocityVector.power, myPilot->velocityVector.power);
            break;
        case LEFT:
            myPilot->velocityVector.power = 80;
            robot_setWheelsVelocity(myPilot->velocityVector.power, -myPilot->velocityVector.power);
            break;
        case RIGHT:
            myPilot->velocityVector.power = 80;
            robot_setWheelsVelocity(-myPilot->velocityVector.power, myPilot->velocityVector.power);
            break;
        case STOP:
            myPilot->velocityVector.power = 0;
            robot_setWheelsVelocity(myPilot->velocityVector.power, myPilot->velocityVector.power);
            break;
        case BACKWARD:
            myPilot->velocityVector.power = 60;
            robot_setWheelsVelocity(-myPilot->velocityVector.power, -myPilot->velocityVector.power);
            break;
        case INIT:
            break;
    }
}


static int hasBumped(){
    return myPilot->pilotState.collision;
}


static void pilot_updatePilotState(){
    
    //met à jour les sensors de myRobot
    SensorState_t sensorState_temp = robot_updateSensorState();

    //met à jour les sensors de myPilot
    myPilot->pilotState.collision = (sensorState_temp.collision == BUMP)?(1):(0);
    myPilot->pilotState.luminosity = sensorState_temp.luminosity;

    myPilot->pilotState.speed = robot_getRobotSpeed();
}


extern void pilot_RunVelocity(Direction_e direction){
    pilot_setEvent(E_ASK_VELOCITY);
    pilot_setDirection(direction);
    pilot_run(myPilot->event);
}

extern void pilot_RunClose(){
    pilot_setEvent(E_CLOSE);    
    pilot_run(E_CLOSE);
}


extern void run_MyCommando(){

    //init myRobot
    robot_new();
    //start myRobot
    robot_start();

    //init myPilot
    pilot_new();

    pilot_start();

    server_start();
}

extern void stop_MyCommando(){

    //init myPilot
    pilot_free();

    //stop myRobot
    robot_stop();
    //free myRobot
    robot_free();

    server_stop();

    printf("C'était un plaisir, à très bientôt ! \n");
}

extern void pilot_setDirection(Direction_e direction){
    myPilot->velocityVector.dir = direction;
}

extern void pilot_setEvent(Event_e event){
    myPilot->event = event;
}

extern PilotState_t pilot_getPilotState(){
    return myPilot->pilotState;
}
