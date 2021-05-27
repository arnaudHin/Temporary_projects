/**
 * Autor : Arnaud Hincelin
 * File : dispatcheurTelco.c
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

#include <../remoteUI.h>
#include "../../../utils.h"
#include "../../../commun.h"

#define MQ_MSG_COUNT 10







enum DispatcheurTelco_State{
    S_IDLE=0,
    S_CHECKMSG,
    NB_STATE
};


enum DispatcheurTelco_Action{
    A_CHECK_MSG= 0,
    NB_ACTION
};



//---------- VARIABLES STATIC ------------ 
static pthread_t dispatcheurTelco_Thread;
static const char dispatcheurTelco_queueName[] = "/myBALdispatchTelco";
static mqd_t dispatcheurTelco_mq;





//---------- PROTOTYPE STATIC ------------ 
static void DispatcheurTelco_Run();








extern void DispatcheurTelco_New(){

    int8_t check = 0;

    struct mq_attr remoteUI_attrQueue = {
        .mq_maxmsg = MQ_MSG_COUNT,
        .mq_msgsize = sizeof(RemoteUI_MqMessage_u),
        .mq_flags = 0,
        .mq_curmsgs = 0
    };
    

    check = mq_unlink(dispatcheurTelco_queueName);
    STOP_ON_ERROR( (check == -1) && (errno != ENOENT) );

    dispatcheurTelco_mq = mq_open(dispatcheurTelco_queueName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &remoteUI_attrQueue);
    STOP_ON_ERROR(dispatcheurTelco_mq == -1);

}


extern void DispatcheurTelco_Start(){

    int8_t check;

    check = pthread_create(&dispatcheurTelco_Thread, NULL, DispatcheurTelco_Run, NULL);

    STOP_ON_ERROR(check != 0);

}

extern void DispatcheurTelco_Stop(){
    pthread_join(dispatcheurTelco_Thread, NULL);

    PRINT("\033[2J\033[;H");
    PRINT(KNRM"ESEO SE 2020-2021 | Robot V2 |HINCELIN Arnaud\n");
    PRINT("Au revoir !\n");

}


extern void DispatcheurTelco_Free(){

    int check;
    check = mq_close( dispatcheurTelco_queueName);
    STOP_ON_ERROR(check == -1);

    check = mq_unlink(dispatcheurTelco_queueName);
    STOP_ON_ERROR(check == -1);
}

static void DispatcheurTelco_Run(){
    

}


