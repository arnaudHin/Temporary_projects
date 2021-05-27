#include "adminUI.h"
#include "robot.h"
//#include "../../commun.h"
#include "../../utils.h"
#include "adminUI.h"
#include "watchdog.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <mqueue.h>
#include <unistd.h>
#define MAX_LIST (200)

static pthread_t myThread;
static SensorState sensor;
static Speed spee;
static bool state1 = false;
static int compt = 0;
static Eventa myEvents[MAX_LIST];
/////////////////////////////////////////////////////////////////////////////////////////////
////                                                                                     ////
////                                  FUNCTIONS PROTOTYPES                               ////
////                                                                                     ////
/////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief appendEvent
 * 
 */
static void appendEvent(SensorState ss, Speed sp);
static void *run(void *aParam);
static void wdExpires();
/////////////////////////////////////////////////////////////////////////////////////////////
////                                                                                     ////
////                                   STATIC FUNCTIONS                                  ////
////                                                                                     ////
/////////////////////////////////////////////////////////////////////////////////////////////

static void *run(void *aParam)
{
    while (state1 != true)
    {
        Watchdog *wat;
        wat = Watchdog_construct(250, wdExpires);
        sensor = Robot_getSensorState();
        spee.speed = Robot_getRobotSpeed();
        appendEvent(sensor, spee);
    }

    return NULL;
}
static void appendEvent(SensorState ss, Speed sp)
{
    if(compt == 199){
        clearEvents();
    }
    myEvents[compt].sens = ss;
    myEvents[compt].speed = sp; 
    compt += compt;
}

/////////////////////////////////////////////////////////////////////////////////////////////
////                                                                                     ////
////                                   PUBLIC FUNCTIONS                                  ////
////                                                                                     ////
/////////////////////////////////////////////////////////////////////////////////////////////

extern void startPolling()
{
    int8_t check;
    check = pthread_create(&myThread, NULL, &run, NULL);
    STOP_ON_ERROR(check != 0);
    TRACE("Start Polling\n");
}

extern void stopPolling()
{
    pthread_join(myThread, NULL);
}

extern void askEvents()
{
    setEvents(myEvents);
}
extern void askEventsCount()
{
    if (sizeof(myEvents[0]) != 0)
    {
        setEventsCount(sizeof(myEvents) / sizeof(myEvents[0]));
    }
    else
    {
        setEventsCount(0);
    }
}
extern void clearEvents()
{
    Eventa newEvents[MAX_LIST];
    *myEvents = *newEvents;
}

extern void signalES(bool s)
{
    state1 = s;
}