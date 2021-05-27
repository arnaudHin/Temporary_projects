/**
 * Autor : Arnaud Hincelin
 * File : remoteUI.c
 */

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdbool.h>
#include <termios.h>

#include "remoteUI.h"
#include "../../utils.h"
#include "../../commun.h"
#include <display.h>

//---------- DEFINE ------------ 
#define MQ_MSG_COUNT 10

//---------- ENUM ------------ 

enum RemoteUI_State{
    S_IDLE=0,
    S_WAIT_CONNECTION,
    S_ERROR,
    S_WAIT_EVENT_COUNT,
    S_WAIT_EVENT,
    S_CONNECTED,
    S_DEATH,
    NB_STATE
};


enum RemoteUI_Action{
    A_SET_IP = 0,
    A_CONNECT,
    A_DISPLAYSCREEN_ERROR,
    A_DISPLAYSCREEN_IDLE,
    A_DISPLAYSCREEN_CONNECTED__SET_TIMER_TO1__SET_INDI,
    A_DISPLAYSCREEN_CONNECTED__SET_TIMER_TO1__SET_INDI__SET_EVENT,
    A_ASK_EVENT_COUNT,
    A_ASK_EVENT,
    A_TRANSLATE_VEL__SET_VEL,
    A_ESTOP,
    A_CANCEL_TIMER_TO1,
    A_QUIT,
    NB_ACTION
};



//---------- STRUCT ------------ 



typedef struct{
    RemoteUI_State_e nextState;
    RemoteUI_Action_e actionToDo;
}Transition_t;


typedef struct {
    indice myIndice;
    bool test;
    RemoteUI_Event_e myEvent;
    RemoteUI_State_e myState;
}RemoteUI_t;



//---------- VARIABLES STATIC ------------ 

static Transition_t tranSyst[NB_STATE-1][NB_EVENT] = {
    [S_IDLE][E_SET_IP] = {S_IDLE, A_SET_IP},
    [S_IDLE][E_QUIT] = {S_DEATH, A_QUIT},
    [S_IDLE][E_VALIDATE] = {S_WAIT_CONNECTION, A_CONNECT},
    [S_WAIT_CONNECTION][E_TEST_KO] = {S_ERROR, A_DISPLAYSCREEN_ERROR},
    [S_ERROR][E_VALIDATE] = {S_IDLE, A_DISPLAYSCREEN_IDLE},
    [S_WAIT_CONNECTION][E_TEST_OK] = {S_WAIT_EVENT_COUNT, A_ASK_EVENT_COUNT},
    [S_WAIT_EVENT_COUNT][E_INDI_ZERO] = {S_WAIT_EVENT_COUNT, A_DISPLAYSCREEN_CONNECTED__SET_TIMER_TO1__SET_INDI},
    [S_WAIT_EVENT_COUNT][E_INDI_NULL] = {S_ERROR, A_DISPLAYSCREEN_ERROR},
    [S_WAIT_EVENT_COUNT][E_INDI_OK] = {S_WAIT_EVENT, A_ASK_EVENT},
    [S_WAIT_EVENT][E_EVENT_NULL] = {S_ERROR, A_DISPLAYSCREEN_ERROR},
    [S_WAIT_EVENT][E_EVENT_OK] = {S_CONNECTED, A_DISPLAYSCREEN_CONNECTED__SET_TIMER_TO1__SET_INDI__SET_EVENT},
    [S_CONNECTED][E_SET_DIR] = {S_CONNECTED, A_TRANSLATE_VEL__SET_VEL},
    [S_CONNECTED][E_ESTOP] = {S_CONNECTED, A_ESTOP},
    [S_CONNECTED][E_QUIT] = {S_DEATH, A_CANCEL_TIMER_TO1},
    };


static RemoteUI_t * myRemoteUI;
static const char remoteUI_queueName[] = "/myBAL";
static mqd_t remoteUI_mq;
static Transition_t * myTransition;
static struct termios oldt, newt;

static pthread_t remoteUI_Thread;

//---------- PROTOTYPE STATIC ------------ 
static void setIp();
// static void setDir();
// static void validate();
static void ask4displayScreen(RemoteUI_Screen_e screen);
static bool RemoteUI_Connection();

static bool RemoteUI_mqReceive();
static bool RemoteUI_mqSend();

static void * RemoteUI_Run();
static void RemoteUI_PerformAction(RemoteUI_Action_e action);








extern void RemoteUI_New(){

    myRemoteUI = NULL;
    int8_t check = 0;
    errno = 0;
    myRemoteUI = calloc(1, sizeof(RemoteUI_t));
    
    STOP_ON_ERROR(myRemoteUI == NULL);

    myRemoteUI->test = false;
    myRemoteUI->myState = S_IDLE;

    struct mq_attr remoteUI_attrQueue = {
        .mq_maxmsg = MQ_MSG_COUNT,
        .mq_msgsize = sizeof(RemoteUI_MqMessage_u),
        .mq_flags = 0,
        .mq_curmsgs = 0
    };
    
    PRINT("Initialisation de remoteUI...\n");

    check = mq_unlink(remoteUI_queueName);
    STOP_ON_ERROR( (check == -1) && (errno != ENOENT) );

    remoteUI_mq = mq_open(remoteUI_queueName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &remoteUI_attrQueue);
    STOP_ON_ERROR(remoteUI_mq == -1);



}


extern void RemoteUI_Start(){

    int8_t check;

    check = pthread_create(&remoteUI_Thread, NULL, RemoteUI_Run, NULL);

    STOP_ON_ERROR(check != 0);

    TRACE("Démarrage");

}

extern void RemoteUI_Stop(){
    pthread_join(remoteUI_Thread, NULL);

    PRINT("\033[2J\033[;H");
    PRINT(KNRM"ESEO SE 2020-2021 | Robot V2 |HINCELIN Arnaud\n");
    PRINT("Au revoir !\n");

}


extern void RemoteUI_Free(){

    int check;
    check = mq_close( remoteUI_mq);
    STOP_ON_ERROR(check == -1);

    check = mq_unlink(remoteUI_queueName);
    STOP_ON_ERROR(check == -1);

    free(myRemoteUI);

}

extern void setEventsCount(indice * indice){
    myRemoteUI->myIndice = *indice;
}

extern void setEvent(RemoteUI_Event_e * event){
    myRemoteUI->myEvent = *event;
}



static void RemoteUI_PerformAction(RemoteUI_Action_e action){
    
    switch (action)
    {
        RemoteUI_MqMessage_u Msg;
        case A_SET_IP:
            setIp();
            break;

        case A_CONNECT:
            PRINT("Connexion en cours...\n");
            //Appel du proxy
            RemoteUI_Connection();

            //Réception du message via le dispatcheur
            break;
        case A_DISPLAYSCREEN_ERROR:
            ask4displayScreen(ERROR_SCREEN);
            break;
        case A_DISPLAYSCREEN_IDLE:
            ask4displayScreen(CONNECT_SCREEN);
            break;
        case A_DISPLAYSCREEN_CONNECTED__SET_TIMER_TO1__SET_INDI:
            ask4displayScreen(CONNECTED_SCREEN);
            
            break;
        case A_DISPLAYSCREEN_CONNECTED__SET_TIMER_TO1__SET_INDI__SET_EVENT:
            ask4displayScreen(CONNECTED_SCREEN);
            
            break;
        case A_ASK_EVENT_COUNT:
            //Demande via ProxyLogger

            //Reception via Dispatcheur
            Msg.data.event = E_INDI_OK;
            RemoteUI_mqSend(&Msg);

            break;
        case A_ASK_EVENT:
            //Demande via ProxyLogger

            //Reception via Dispatcheur
            Msg.data.event = E_EVENT_OK;
            RemoteUI_mqSend(&Msg);   

            break;
        case A_TRANSLATE_VEL__SET_VEL:
            break;
        case A_ESTOP:
            
            break;
        case A_CANCEL_TIMER_TO1:
            

            break;
        case A_QUIT:
            PRINT("arret en cours...\n");
            break;


        default:
            break;
        }
    
}

static void * RemoteUI_Run(){

    RemoteUI_MqMessage_u myMsg;

    ask4displayScreen(CONNECT_SCREEN);

    while (myRemoteUI->myState != S_DEATH)
    {
        RemoteUI_mqReceive(&myMsg);
        //PRINT("event recu : %s", myMsg.buffer);
        myTransition = &tranSyst[myRemoteUI->myState][myMsg.data.event];
        
        RemoteUI_PerformAction(myTransition->actionToDo);
        myRemoteUI->myState = myTransition->nextState;        
        
    }
    return NULL;
}


static bool RemoteUI_mqReceive(RemoteUI_MqMessage_u * message){

    int8_t check = 0;
    check = mq_receive(remoteUI_mq, message->buffer, sizeof(RemoteUI_MqMessage_u), 0);
    STOP_ON_ERROR(check == -1);
    return check;
}

static bool RemoteUI_mqSend(RemoteUI_MqMessage_u * message){

    int8_t check = 0;
    check = mq_send(remoteUI_mq, message->buffer, sizeof(RemoteUI_MqMessage_u), 0);
    STOP_ON_ERROR(check == -1);
    return check;
}


static void ask4displayScreen(RemoteUI_Screen_e screen){

    char a;
    RemoteUI_MqMessage_u myMsg;

    switch (screen)
    {
        case CONNECT_SCREEN:
            displayScreen(CONNECT_SCREEN);
            a = getchar();

            printf("char chosir : %c", a);

            switch (a)
            {

                case 'c':
                    myMsg.data.event = E_SET_IP;
                    RemoteUI_mqSend(&myMsg); 
                    break;
                case 'q':
                    myMsg.data.event = E_QUIT;
                    RemoteUI_mqSend(&myMsg);
                    break;
                default:
                    break; 
            }
            
            break;
        case ERROR_SCREEN:

            /* code */

            break;
        case CONNECTED_SCREEN:
            displayScreen(CONNECTED_SCREEN);

            a = getchar();
            switch (a)
            {

                case 'q':
                    myMsg.data.event = E_QUIT;
                    RemoteUI_mqSend(&myMsg);
                    break;
                default:
                    break;
            }

            break;
        default:
            break;
    }


    
}


static void setIp(){

    PRINT("Entrer l'adresse IP sans mettre les . de séparation \n");
    PRINT("Adresse IP : ");

    char buf[20];
    scanf("%s",buf);
    
    PRINT("Enregistrement de l'adresse IP : %s \n", buf);

    RemoteUI_MqMessage_u Msg;
    
    Msg.data.event = E_VALIDATE;
    RemoteUI_mqSend(&Msg);
}

static bool RemoteUI_Connection(){

    //Envoie via Proxy
    //...
    //Réception du message via le dispatcheur

    RemoteUI_MqMessage_u Msg;
    
    Msg.data.event = E_TEST_OK;
    RemoteUI_mqSend(&Msg);

    return true;
}

