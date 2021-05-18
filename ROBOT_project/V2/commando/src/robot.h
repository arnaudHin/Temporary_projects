

#ifndef ROBOT_H
#define ROBOT_H

#include <stdlib.h>
#include <stdio.h>

typedef enum {
    NO_BUMP=0,
    BUMP
}Collision_e;

typedef struct {
    Collision_e collision;
    float luminosity;
}SensorState_t;

typedef struct Sensors Sensors_t;
typedef struct Robot Robot_t;

extern void robot_new();
extern void robot_free();
extern void robot_start();
extern void robot_stop();
extern void robot_setWheelsVelocity(int mr, int ml);
extern int robot_getRobotSpeed();
extern SensorState_t robot_updateSensorState();
extern Sensors_t robot_getSensors();
extern Robot_t * robot_getRobot();

#endif /* ROBOT_H */

