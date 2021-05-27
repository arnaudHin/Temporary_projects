#include "adminUI.h"
#include "robot.h"
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
#include <unistd.h>
#define MAX_LIST (200)
typedef struct
{
    SensorState sens;
    Speed speed;
} Eventa;

static pthread_t myThread;
static SensorState sensor;
static Speed spee;
static bool state1 = false;

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
/////////////////////////////////////////////////////////////////////////////////////////////
////                                                                                     ////
////                                   STATIC FUNCTIONS                                  ////
////                                                                                     ////
/////////////////////////////////////////////////////////////////////////////////////////////

static void *run(void *aParam)
{
    while (state1 != true)
    {
        usleep(250000);
        sensor = Robot_getSensorState();
        spee.speed = Robot_getRobotSpeed();
        appendEvent(sensor, spee);
    }

    return NULL;
}
static void appendEvent(SensorState ss, Speed sp)
{
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
}
extern void clearEvents()
{
}

extern void signalES(bool s)
{
}