
#include "../../commun.h"
#include "client.h"
#include "remoteUI.h"
#include "../../commando/src/pilot.h"

#include <pthread.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

enum remoteUIState{ 
    S_RUN=0,
    S_STOP
};

struct RemoteUI{
    Direction_e actualDirection;
    char choiceEntry;
    remoteUIState_e remoteUIState;
    PilotState_t pilotState;
};

// STATIC prototypes functions
static void setIP();
static void captureChoice();
static void askMvt(Direction_e direction);
static void askClose();
static void ask4Log();
static void askClearLog();
static void run();
static void display();
static void choiceManager();
static void * remoteui_threadClient();
static void remoteUI_setActualDirection(Direction_e dir);
static void remoteUI_updateLog();

// Convert directions into string
const char *directionNames[] = {"Gauche", "Droite", "Devant", "Arrière", "Stop", ""};
static uint8_t showLog = 0;
static uint8_t LogNumber = 0;

static RemoteUI_t * myRemoteUI;

extern void remoteUI_new() {

    myRemoteUI = NULL;

    //dynamic allocution of RemoteUI
    myRemoteUI = calloc(1, sizeof(RemoteUI_t));

    if (myRemoteUI == NULL) {
        perror("remoteUI_new()");
        exit(EXIT_FAILURE);
    }

    //Pas besoin d'init valeurs -> calloc met tout à 0 
    //enumerations init avec 0
}

extern void remoteUI_start(){
    run();
}

extern void remoteUI_stop(){
    printf("\033[2J\033[;H");
    printf(KNRM"ESEO SE 2020-2021 | Robot V2 |HINCELIN Arnaud\n");
    printf("Au revoir !\n");
}

extern void remoteUI_free(){
    free(myRemoteUI);
}


static void * remoteui_threadClient(){

    while (myRemoteUI->remoteUIState != S_STOP){
                
        remoteUI_updateLog();
        sleep(0.1);
    }

    pthread_exit(NULL);
}




static void run(){

    // Avoid enter key for any request
    static struct termios oldt, newt;

    // solution trouvée :
    // https://stackoverflow.com/questions/1798511/how-to-avoid-pressing-enter-with-getchar-for-reading-a-single-character-only/1798833#1798833

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    newt.c_lflag &= ~(ICANON);

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);


    //------------------------------------
    pthread_t clientThread;

    pthread_create(&clientThread, NULL, remoteui_threadClient, NULL); //lancement thread de mise à jour des sensors

    display();

    while (myRemoteUI->remoteUIState == S_RUN){
        captureChoice();
        choiceManager();
    }
    
    pthread_cancel(clientThread);

    //------------------------------------

    /*restaurer ancienne config de terminal*/
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

static void display(){
    printf("\033[2J\033[;H");
    printf(KNRM FRED"--- Télécommande - Robot ---\n");
    printf(KNRM "q: Allez à gauche\n");
    printf("d: Allez à droite\n");
    printf("z: Avancer\n");
    printf("s: Stopper / Reculer\n");
    printf("p: Afficher Logs\n");
    printf("o: Effacer les Logs\n");
    printf("a: Quitter\n");
    
    if (myRemoteUI->pilotState.collision){
        remoteUI_setActualDirection(STOP);
    }

    printf("--- Direction actuelle : [ "KCYN"%s"KNRM" ] ---\n", directionNames[myRemoteUI->actualDirection]);    

    if (showLog)
    {
        printf("----- Log n° %d ----- \n", LogNumber);
        printf("Collision : %s  |  ", (myRemoteUI->pilotState.collision)?("Oui"):("Non") );
        printf("Vitesse : %d  |  ", myRemoteUI->pilotState.speed);
        printf("Luminosité : %f \n", myRemoteUI->pilotState.luminosity);
    }
}


static void captureChoice(){
    myRemoteUI->choiceEntry = getchar();
}


static void choiceManager(){

    switch (myRemoteUI->choiceEntry) {
        case 'a':
            askClose();
            break;
        case 'z':
            askMvt(FORWARD);
            remoteUI_setActualDirection(FORWARD);
            break;
        case 'q':
            askMvt(LEFT);
            remoteUI_setActualDirection(LEFT);
            break;
        case 's':
            if (myRemoteUI->actualDirection == STOP){
                askMvt(BACKWARD);
                remoteUI_setActualDirection(BACKWARD);
                break;             
            }
            askMvt(STOP);
            remoteUI_setActualDirection(STOP);
            break;
        case 'd':
            askMvt(RIGHT);
            remoteUI_setActualDirection(RIGHT);
            break;
        case 'p':
            ask4Log();
            break;
        case 'o':
            askClearLog();
            break;
        default:
            break;
    }

    display(myRemoteUI);    
}

static void askMvt(Direction_e direction){
    client_sendMsg_socketEvent(E_SCK_SET_VEL);
    client_sendMsg_velocity(direction);
}

static void askClose(){
    myRemoteUI->remoteUIState = S_STOP;
    client_sendMsg_socketEvent(E_SCK_CLOSE);
}

static void ask4Log(){
    showLog = 1;
    if(LogNumber < 254){
        LogNumber += 1;
    }
}


static void askClearLog(){
    showLog = 0;
}

static void setIP(){

    char ip[] = "127.0.0.1";

    client_setIp(ip);

}


static void remoteUI_updateLog(){
    client_sendMsg_socketEvent(E_SCK_LOG);

    myRemoteUI->pilotState = client_readMsg_pilotState();

}


static void remoteUI_setActualDirection(Direction_e dir){
    myRemoteUI->actualDirection = dir;
}

extern void run_Telecommande(){
    
    remoteUI_new();

    setIP();
    
    client_start();

    remoteUI_start();
}

extern void close_Telecommande(){
    
    remoteUI_stop();

    remoteUI_free();

    client_stop();
}










