
#include "../../commun.h"
#include "../../utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <mqueue.h>
#include <unistd.h> // close
#include "watchdog.h"
#include "pilot.h"
#define MQ_MAX_MESSAGES 10

/////////////////////////////////////////////////////////////////////////////////////////////
////                                                                                     ////
////                                  TYPEDEF & VARIABLES                                ////
////                                                                                     ////
/////////////////////////////////////////////////////////////////////////////////////////////
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
/**
 * @def MQ_MAX_MESSAGES
 *
 * The maximum number of message the queue can handle at one time
 *
 * By default, cannot be higher than 10, unless you change this
 * value into /proc/sys/fs/mqueue/msg_max
 */
bool bump = 0;
bool emergency = false;
PilotState myPilotState;
VelocityVector currentVel;
Watchdog *wat;
static pthread_t myThread;
static mqd_t myBal;
static const char queueName[] = "/Balou";
static Transition *myTrans;
static State myState = NORMALIDLE_S;
typedef void (*ActionPtr)();

Pilot *pilot;
//booléen pour savoir si collision ou non

//décalaration méthodes static
static bool hasBumped();

static void sendMvt(VelocityVector);
static void wdExpires(); // sera la fonction appelée par le constructeur du chien de garde
// static void executionAction(Action); //ou l'on va exécuter les différentes actions
// static void Pilot_idle(void);
// static void Pilot_running();
//static void run(Event event, VelocityVector vel);
static void *run(void *aParam);

static const char *const stateName[] = {
    "NONE_S",
    "FINAL_S",
    "NORMALIDLE_S",
    "NORMAL_FROM_RUNNING_S",
    "NORMAL_BUMPED_S ",
    "EMERGENCY_S",
    "NB_ETAT_S"};

static const char *stateGetName(int8_t i)
{
    return stateName[i];
}
static const char *const actionName[] = {
    "NONE_A",
    "NORMAL_IDLE_TO_EMERG_A",
    "NORMAL_IDLE_TO_NORUN_A",
    "NORUN_TO_NORMAL_IDLE_A",
    "NORUN_TO_NOBUMP_A ",
    "NO_RUN_TO_EMER_A",
    "NO_BUMP_TO_EMERG_A",
    "NO_BUMP_TO_NORUN_A",
    "EMERGENCY_TO_NORIDLE_A",
    "NORUN_NORUN_A",
    "CHECK_A",
    "NB_ACTION"};

static const char *actionGetName(int8_t i)
{
    return actionName[i];
}
static const char *const eventName[] = {
    "STOP_E",
    "CHECK_E",
    "SET_VELOCITY_E",
    "TOGGLEES_E",
    "BUMPED_E",
    "NO_BUMPED_E",
    "COND_TRUE_E",
    "SET_ROBOT_VEL_E",
    "TI_OUT_E",
    "NB_EVENT_E"};
static const char *eventGetName(int8_t i)
{
    return eventName[i];
}


/////////////////////////////////////////////////////////////////////////////////////////////
////                                                                                     ////
////                                  FUNCTIONS PROTOTYPES                               ////
////                                                                                     ////
/////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    Event event;
    // generic definition to copy events' parameters
} MqMsgData;
/**
 * @brief Enable to send and receive messages without state convertion concern
 * 
 */
typedef union
{
    MqMsgData data;
    char mqBuffer[sizeof(MqMsgData)];
} MqMsg;

/**
 * @brief To receive a mqMessage
 * 
 * @param aMsg is a State
 */
static void mqReceive(MqMsg *aMsg);


/**
 * @brief signalES()
 * 
 */
static void signalES(bool state);
/**
 * @brief setTimer()
 * 
 */
static void setTimer(Watchdog *timer);

/**
 * @brief cancel()
 * 
 */

static void cancel(Watchdog *timer);

/**
 * @brief setRobotVelocity(VelocityVector vel) function
 * 
 */
static void setRobotVelocity(VelocityVector vel);

/**
 * @brief 
 * 
 */
static void conditionTrue();
/**
 * @brief checkCurrentVel
 * 
 */
static void checkCurrentVel();
/**
 * @brief 
 * 
 */
static void notBumped();
/**
 * @brief bumped()
 * 
 */
static void bumped();
/**
 * @brief To send a mqMessage
 * 
 * @param aMsg is a State
 */
static void mqSend(MqMsg *aMsg);

/**
 * @brief Transition from *run(...) function. Allow people to test the transition
 * 
 * @param msg, transmit an Event
 */
static void transitionFct(MqMsg msg);

static void timeOutEmptyTime();
/**
 * @brief Table of actions to realize => as a function pointers
 * 
 */
static void actionNone();
/**
 * @brief Table of actions to realize => as a function pointers
 * 
 */
static void actionNorIdleToEmerg();
/**
 * @brief Table of actions to realize => as a function pointers
 * 
 */
static void actionNorIdleToNorRun();
/**
 * @brief Table of actions to realize => as a function pointers
 * 
 */
static void actionNoRunToNorIdle();
/**
 * @brief Table of actions to realize => as a function pointers
 * 
 */
static void actionNorRunToNoBump();
/**
 * @brief Table of actions to realize => as a function pointers
 * 
 */
static void actionNoRunToEmer();
/**
 * @brief Table of actions to realize => as a function pointers
 * 
 */
static void actionNoBumpToNoRun();
/**
 * @brief Table of actions to realize => as a function pointers
 * 
 */
static void actionEmerToNorIdle();
/**
 * @brief Table of actions to realize => as a function pointers
 * 
 */
static void actionNoRunToNoRun();
/**
 * @brief Table of actions to realize => as a function pointers
 * 
 */
static void actionCheck();
/**
 * @brief Table of actions to realize => as a function pointers
 * 
 */
static const ActionPtr actionsTab[NB_ACTION] = {&actionNone, &actionNorIdleToEmerg, &actionNorIdleToNorRun, &actionNoRunToNorIdle,
                                                &actionNorRunToNoBump, &actionNoRunToEmer, &actionNoBumpToNoRun, &actionEmerToNorIdle, &actionNoRunToNoRun, &actionCheck};






/////////////////////////////////////////////////////////////////////////////////////////////
////                                                                                     ////
////                                   STATIC FUNCTIONS                                  ////
////                                                                                     ////
/////////////////////////////////////////////////////////////////////////////////////////////
//methode maTransition qui renvoie une structure transition en fonction de l'etat actuel et de l'evenement choisi
static Transition maTransition[NB_ETAT_S][NB_EVENT_E] =
    {
        [NORMALIDLE_S][TOGGLEES_E] = {EMERGENCY_S, NORMAL_IDLE_TO_EMERG_A},
        [NORMALIDLE_S][SET_ROBOT_VEL_E] = {NORMAL_FROM_RUNNING_S, NORMAL_IDLE_TO_NORUN_A},

        [EMERGENCY_S][TOGGLEES_E] = {NORMALIDLE_S, EMERGENCY_TO_NORIDLE_A},

        [NORMAL_FROM_RUNNING_S][TOGGLEES_E] = {EMERGENCY_S, NORMAL_IDLE_TO_EMERG_A},
        [NORMAL_FROM_RUNNING_S][TI_OUT_E] = {NORMAL_BUMPED_S, NORUN_TO_NOBUMP_A},
        [NORMAL_FROM_RUNNING_S][SET_VELOCITY_E] = {NORMAL_FROM_RUNNING_S, NORUN_NORUN_A},
        [NORMAL_FROM_RUNNING_S][COND_TRUE_E] = {NORMALIDLE_S, NORUN_TO_NORMAL_IDLE_A},

        [NORMAL_BUMPED_S][NO_BUMPED_E] = {NORMAL_FROM_RUNNING_S, NO_BUMP_TO_NORUN_A},
        [NORMAL_BUMPED_S][TOGGLEES_E] = {EMERGENCY_S, NO_BUMP_TO_EMERG_A},
        [NORMAL_BUMPED_S][BUMPED_E] = {NORMALIDLE_S, NO_BUMP_TO_NORUN_A},

};
static void transitionFct(MqMsg msg)
{
    myTrans = &maTransition[myState][msg.data.event];

    TRACE("MAE, events management %s \n", eventGetName(msg.data.event));

    if (myTrans->etatDestination != NONE_S)
    {
        actionsTab[myTrans->action](); //execution of the action
        TRACE("MAE, actions management %s \n", actionGetName(myTrans->action));
        myState = myTrans->etatDestination;
        TRACE("MAE, going to state %s \n", stateGetName(myState));
    }
    else
    {
        TRACE("MAE, lost event %s  \n", eventGetName(msg.data.event));
    }
}

static void mqReceive(MqMsg *aMsg)
{
    int8_t check;
    check = mq_receive(myBal, aMsg->mqBuffer, sizeof(MqMsg), NULL);
    //check = mq_receive(myBal, (char *)aMsg, sizeof(*aMsg), NULL);
    STOP_ON_ERROR(check == -1);
}

static void mqSend(MqMsg *aMsg)
{
    int8_t check;
    check = mq_send(myBal, aMsg->mqBuffer, sizeof(MqMsg), 0);
    //check = mq_send(myBal, (const char*) aMsg->mqBuffer, sizeof(MqMsg), 0);
    STOP_ON_ERROR(check != 0);
}
static void *run(void *aParam)
{
    MqMsg msg;
    while (myState != NONE_S)
    {
        mqReceive(&msg);

        if (msg.data.event == STOP_E)
        {
            myState = NONE_S;
        }
        else
        {
            transitionFct(msg);
        }
    }
    return NULL;
}

static void actionNone()
{
    TRACE("[Pilote] Action NONE \n");
}

static void actionNorIdleToEmerg()
{
    toggleES();
}
static void actionNorIdleToNorRun()
{
    VelocityVector vela;
    vela.dir = LEFT_D;
    vela.power = 80;
    setRobotVelocity(vela);
}
static void actionNoRunToNorIdle()
{
    conditionTrue();
}
static void actionNorRunToNoBump()
{
}
static void actionNoRunToEmer()
{
    conditionTrue();
}
static void actionNoBumpToNoRun()
{
    notBumped();
}
static void actionEmerToNorIdle()
{
    toggleES();
}
static void actionNoRunToNoRun()
{
    VelocityVector vela;
    vela.dir = LEFT_D;
    vela.power = 80;
    Pilot_setVelocity(vela);
}
static void actionCheck()
{
}

static void setRobotVelocity(VelocityVector vel)
{
    MqMsg msg = {.data.event = SET_ROBOT_VEL_E};
    TRACE("TSET_ROBOT_VEL_E\n");
    currentVel = vel;
    sendMvt(currentVel);
    wat = Watchdog_construct(1, wdExpires);
    setTimer(wat);
    mqSend(&msg);
}
static void conditionTrue()
{
    MqMsg msg = {.data.event = COND_TRUE_E};
    TRACE("COND_TRUE_E\n");
    cancel(wat);
    mqSend(&msg);
}

static void notBumped()
{
    MqMsg msg = {.data.event = NO_BUMPED_E};
    TRACE("NO_BUMPED_E\n");
    bump = false;
    setTimer(wat);
    mqSend(&msg);
}
static void bumped()
{
    MqMsg msg = {.data.event = BUMPED_E};
    TRACE("BUMPED_E\n");
    bump = true;
    currentVel.dir = STOP_D;
    cancel(wat);
    mqSend(&msg);
}
static void wdExpires()
{ // sera la fonction appelée par le constructeur du chien de garde
}
static void setTimer(Watchdog *timer)
{
    //Watchdog_construct(delay, wdExpires);
    Watchdog_start(timer);
}
static void cancel(Watchdog *timer)
{
    Watchdog_cancel(timer);
    Watchdog_destroy(timer);
}
static void signalES(bool state)
{
    if (state == true)
    {
        emergency = true;
    }
    else
    {
        emergency = false;
    }
}

static void checkCurrentVel()
{
    if (currentVel.dir == STOP_D)
    {
        conditionTrue();
    }
}

// //méthode où l'on exécute notre action
// void executionAction(Action action)
// {
//     printf("je suis dans executionAction dans Pilot\n");

//     switch(action)
//     {
//             case NONE_A :
//                 if(hasBumped()==TRUE){
//                     Pilot_idle();

//                 }
//                 break;

//             case STOP_A : //stopper la MAE
//                 pilot->velocityVector.dir=STOP_D; //on met la direction du vecteurVelocité à stop
//                 sendMvt(pilot->velocityVector);
//                 Robot_stop();
//                 pilot->etat=FINAL_S;
//                 break;

//             case SET_VELOCITY_A :
//                 if(pilot->velocityVector.dir==STOP_D){
//                     Pilot_idle();
//                 }else{
//                     Pilot_running();
//                 }
//                 break;

//             case CHECK_A :
//                 bump=hasBumped();
//                 if(bump==TRUE){
//                     Pilot_idle();
//                 }else{
//                     pilot->etat=NORMAL_FROM_RUNNING_S;
//                 }
//                 break;

//             default :
//                 break;

//     }
// }

//méthode allouer mémoire a notre pilot et initialiser ses variables
extern void Pilot_new()
{
    int8_t check;
    struct mq_attr attr = {
        .mq_maxmsg = MQ_MAX_MESSAGES,
        .mq_msgsize = sizeof(MqMsg),
        .mq_flags = 0,
    };

    TRACE("Initialisation\n");
    check = mq_unlink(queueName);
    STOP_ON_ERROR((check == -1) && (errno != ENOENT));
    myBal = mq_open(queueName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &attr);
    STOP_ON_ERROR(myBal == -1);
    Robot_new();
    currentVel.dir = STOP_D;
    pilot = (Pilot *)calloc(1, sizeof(Pilot));
    //pilot->robot = *robotInit;

    pilot->pilotState.collision = (int)pilot->robot.sensorsState.collision;
    pilot->pilotState.luminosity = pilot->robot.sensorsState.luminosity;
    pilot->pilotState.speed = 0;

    pilot->velocityVector.power = 0;
    pilot->velocityVector.dir = STOP_D;
    //pilot->etat = NORMALIDLE_S;
}

//méthode pour start le pilot
void Pilot_start()
{
    printf("je suis dans pilot start");
    Robot_start();
    int8_t check;
    check = pthread_create(&myThread, NULL, &run, NULL);
    STOP_ON_ERROR(check != 0);
    TRACE("Launching\n");
}

// //méthode qui s'executera lorsquon démarrera le pilot
// void run(Event event, VelocityVector vel)
// {
//     printf("je suis dans run dans Pilot\n");

//     Action action;
//     State state;
//     action = maTransition[pilot->etat][event].action;
//     state = maTransition[pilot->etat][event].etatDestination;
//     if (state != FINAL_S)
//     {
//         pilot->velocityVector = vel;
//         executionAction(action);
//         //pilot->etat = state; //on met a jour l'état //NE PAS LE FAIRE ICI SINON PB QD IDLE
//         printf("on change d'état : %d\n", pilot->etat);
//     }
// }

// //méthode pour stopper le robot
// void Pilot_idle(){
//     printf("je suis dans Pilot_idle dans Pilot \n");
//     pilot->etat=NORMALIDLE_S;
//     pilot->velocityVector.dir=STOP_D;
//     sendMvt(pilot->velocityVector);
// }

// //méthode pour faire bouger le robot
// void Pilot_running(){
//     printf("je suis dans Pilot_running dans Pilot \n");

//     //check();
//     pilot->etat=NORMAL_FROM_RUNNING_S;
//     sendMvt(pilot->velocityVector);

// }

//envoyer ordre au robot du mouvement
void sendMvt(VelocityVector vector)
{
    printf("je suis dans sendMvt dans Pilot \n");

    if (vector.power != 0)
    {
        switch (vector.dir)
        {
        case LEFT_D:
            printf("je suis dans sendMvt dans le case LEFT dans Pilot \n");

            Robot_setWheelsVelocity((int)vector.power, -(int)vector.power); //
            break;

        case RIGHT_D:
            printf("je suis dans sendMvt dans le case RIGHT dans Pilot \n");

            Robot_setWheelsVelocity(-(int)vector.power, (int)vector.power);
            break;

        case FORWARD_D:
            printf("je suis dans sendMvt dans le case FORWARD dans Pilot \n");

            Robot_setWheelsVelocity((int)vector.power, (int)vector.power);
            break;

        case BACKWARD_D:
            printf("je suis dans sendMvt dans le case BACKWARD dans Pilot \n");

            Robot_setWheelsVelocity(-(int)vector.power, -(int)vector.power);
            break;

        case STOP_D:
            printf("je suis dans sendMvt dans le case STOP dans Pilot \n");

            Robot_setWheelsVelocity(0, 0);
            break;
        }
    }
}
extern void toggleES()
{
    MqMsg msg = {.data.event = TOGGLEES_E};
    TRACE("TOGGLEES_E\n");
    if (emergency == true)
    {
        signalES(false);
    }
    else
    {
        signalES(true);
        currentVel.dir= STOP_D;
        sendMvt(currentVel);
        cancel(wat);
    }
    mqSend(&msg);
}
//stopper le robot
void Pilot_stop()
{
    MqMsg msg = {.data.event = STOP_E};
    TRACE("Stop the tour\n");
    mqSend(&msg);
    //run(STOP_E, pilot->velocityVector);
}

//changer la velocité du robot (diration, vitesse)
void Pilot_setVelocity(VelocityVector vel)
{
    MqMsg msg = {.data.event = SET_VELOCITY_E};
    TRACE("SET_VELOCITY_E\n");
    printf("je suis dans Pilot_setVelocity dans Pilot\n");
    //run(SET_VELOCITY_E,vel);
    currentVel = vel;
    sendMvt(currentVel);
    checkCurrentVel();
    mqSend(&msg);
}

//libérer mémoire alloué au robot et pilot
void Pilot_free()
{
    Robot_free(pilot->robot);
    free(pilot);
    myState = NONE_S;
    int8_t check;
    check = mq_close(myBal);
    STOP_ON_ERROR(check == -1);
    check = mq_unlink(queueName);
    STOP_ON_ERROR(check == -1);
    TRACE("Destruction\n");
}

//retourne l'etat du robot
PilotState Pilot_getState()
{
    printf("je suis dans Pilot_getState dans Pilot\n");
    pilot->pilotState.luminosity = Robot_getSensorState(pilot->robot).luminosity;
    pilot->pilotState.collision = Robot_getSensorState(pilot->robot).collision;
    pilot->pilotState.speed = Robot_getRobotSpeed(pilot->robot);
    return pilot->pilotState;
}

//on met a jour les états du pilot
void check()
{
    if (bump == 1)
    {
        bumped();
    }
    else
    {
        notBumped();
    }
}
static void timeOut1()
{
    MqMsg msg = {.data.event = TI_OUT_E};
    TRACE("Time out of the timer\n");
    bump = hasBumped();
    check();
    mqSend(&msg);
}

//pour savoir si le robot est bump ou pas, collision ou pas
bool hasBumped()
{
    //check();
    pilot->pilotState = Pilot_getState();
    bool bumpBis;
    if (pilot->pilotState.collision == 0)
    {
        bumpBis = false;
    }
    if (pilot->pilotState.collision == 1)
    {
        bumpBis = true;
    }
    return bumpBis;
}