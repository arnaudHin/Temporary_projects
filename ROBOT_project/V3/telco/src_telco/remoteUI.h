/**
 * Autor : Arnaud Hincelin
 * File : RemoteUI.h
 */

#ifndef REMOTEUI_H_
#define REMOTEUI_H_

#include <stdint.h>

typedef enum RemoteUI_State RemoteUI_State_e;
typedef enum RemoteUI_Event RemoteUI_Event_e;
typedef enum RemoteUI_Action RemoteUI_Action_e;

typedef uint32_t ip;
typedef uint16_t indice;



extern void RemoteUI_New();
extern void RemoteUI_Stop();
extern void RemoteUI_Start();
extern void RemoteUI_Free();

extern void setEventsCount(indice * indice);
extern void setEvent(RemoteUI_Event_e * event);










#endif /* REMOTEUI_H_ */

