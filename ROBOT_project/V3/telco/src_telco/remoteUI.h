/**
 * Autor : Arnaud Hincelin
 * File : RemoteUI.h
 */

#ifndef REMOTEUI_H_
#define REMOTEUI_H_

#include <stdint.h>

typedef enum RemoteUI_State RemoteUI_State_e;
typedef enum RemoteUI_Action RemoteUI_Action_e;

typedef enum{
    CONNECT_SCREEN=0,
    ERROR_SCREEN,
    CONNECTED_SCREEN,
    NB_SCREEN
}RemoteUI_Screen_e;

typedef enum {
    E_NOP=0,
    E_SET_IP,
    E_VALIDATE,
    E_TEST_OK,
    E_TEST_KO,
    E_INDI_ZERO,
    E_INDI_NULL,
    E_INDI_OK,
    E_EVENT_NULL,
    E_EVENT_OK,
    E_SET_DIR,
    E_ESTOP,
    E_QUIT,
    NB_EVENT
}RemoteUI_Event_e;


typedef struct{

    RemoteUI_Event_e event;

}RemoteUI_DataMq_t;

typedef union{

    RemoteUI_DataMq_t data;
    char buffer[sizeof(RemoteUI_DataMq_t)];

}RemoteUI_MqMessage_u;




typedef uint32_t ip;
typedef uint16_t indice;



extern void RemoteUI_New();
extern void RemoteUI_Stop();
extern void RemoteUI_Start();
extern void RemoteUI_Free();

extern void setEventsCount(indice * indice);
extern void setEvent(RemoteUI_Event_e * event);




#endif /* REMOTEUI_H_ */

