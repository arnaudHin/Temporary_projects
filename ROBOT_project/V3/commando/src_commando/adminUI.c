
//Rédigé par Briac Beutter
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <stdbool.h>
#include <mqueue.h>
#include "adminUI.h"
#include "pilot.h"
#include "logger.h"

#define MAX_LIST (200)

#define errExit(msg) \
  do                 \
  {                  \
    perror(msg);     \
  } while (0)

#define MQ_max (5)






//Definition des états possibles
typedef enum
{
  S_FORGET = 0,
  S_MAIN_SCREEN, 
  S_LOG_SREEN,
  S_DEATH,
  S_WAIT_NUMBER_EVENTS,
  S_WAIT_EVENTS,
  NB_STATE

} State;

//Definition des actions possibles
typedef enum
{
  A_AUI_NOP = 0,//Ne rien faire
  A_AUI_STOP_MAIN,//Arrêter le main
  A_AUI_STOP_LOG,//Arrêter les logs
  A_AUI_TES,//ToggleEmergencyStop
  A_AUI_ASK_EVENTS_COUNT,
  A_AUI_CLEAR_LOG,//Effacer logs
  A_AUI_SET_EVENTS_COUNT,
  A_AUI_SET_EVENTS,
  A_AUI_BACK_MAIN,//Revenir au main screen
  A_AUI_GO_SLOG,//Aller au log screen
  NB_ACTION
} Action;

//Definition des évènements possible
typedef enum
{
  E_AUI_STOP = 0,
  E_AUI_QUIT,//Quitter
  E_AUI_TES,//Faire toggleEmergencyStop
  E_AUI_CLEAR_LOG,//Effacer log
  E_AUI_BACK_MS,//Revenir au main screen
  E_AUI_GO_SLOG,//Aller au screen log
  E_AUI_TIME_OUT,//Time out du timer
  E_AUI_SET_EVENTS_COUNT,
  E_AUI_SET_EVENTS,
  NB_EVENT
} Event;

//Structure pour l'état de destination et l'action à faire
typedef struct
{
  State destination;
  Action action;
} Transition;

//Definition des transitions
static Transition transition[NB_STATE][NB_EVENT] =
    {
        [S_MAIN_SCREEN][E_AUI_STOP] = {S_DEATH, A_AUI_STOP_MAIN},
        [S_MAIN_SCREEN][E_AUI_GO_SLOG] = {S_WAIT_NUMBER_EVENTS, A_AUI_ASK_EVENTS_COUNT},
        [S_MAIN_SCREEN][E_AUI_TES] = {S_MAIN_SCREEN, A_AUI_TES},
        [S_LOG_SREEN][E_AUI_CLEAR_LOG] = {S_LOG_SREEN, A_AUI_CLEAR_LOG},
        [S_LOG_SREEN][E_AUI_TES] = {S_LOG_SREEN, A_AUI_TES},
        [S_LOG_SREEN][E_AUI_STOP] = {S_DEATH, A_AUI_STOP_LOG},
        [S_LOG_SREEN][E_AUI_TIME_OUT] = {S_LOG_SREEN, A_AUI_ASK_EVENTS_COUNT},
        [S_LOG_SREEN][E_AUI_BACK_MS] = {S_MAIN_SCREEN, A_AUI_BACK_MAIN},
        [S_WAIT_EVENTS][E_AUI_SET_EVENTS_COUNT] = {S_WAIT_EVENTS, A_AUI_SET_EVENTS_COUNT},
        [S_WAIT_EVENTS][E_AUI_SET_EVENTS] = {S_LOG_SREEN, A_AUI_SET_EVENTS},
};

//Definition des écrans possibles
typedef enum 
{
  MAIN_SCREEN = 0,
  LOG_SCREEN
}ScreenId;

// Attributs de AdminUi
int currentEventNumber;
int previousEventNumber;
static State myState = S_MAIN_SCREEN;
static pthread_t myThread;

/**
 * BAL
 */
static const char BAL[] = "/BALaUI";
static mqd_t myMq;          
static struct mq_attr attr; 

//Identifiant du timer
timer_t timerId;


Eventa myEvents[MAX_LIST];

//Structure pour les messages
typedef struct
{
  Event event;
} MqMessage;

char commande;
int quitter = 1;
//Prototypes static
static void *timerOut();
static void displayScreen(ScreenId idScreen);
static void updateEvents();
static void setTimer();
static void cancelTimer();
static void destroyTimer();
static void AdminUI_mqReceive(MqMessage *this);
static void *run();
static void performAction(Action action);
static void captureChoice();

//Méthodes externes

extern void AdminUI_new()
{

  struct sigevent sev;
  sev.sigev_notify = SIGEV_THREAD;
  sev.sigev_value.sival_ptr = &timerId;
  sev.sigev_notify_function = timerOut();
  int error_code;
  error_code = timer_create(CLOCK_REALTIME, &sev, &timerId);
  if (error_code == -1)
  {
    errExit("Timer_create error");
  }
}

extern void AdminUI_start()
{
  currentEventNumber = 0;
  previousEventNumber = 0;

  //Ouverture d'une BAL
  attr.mq_flags = 0;
  attr.mq_maxmsg = MQ_max;
  attr.mq_msgsize = sizeof(MqMessage);
  attr.mq_curmsgs = 0;
  mq_unlink(BAL);
  myMq = mq_open(BAL, O_RDWR | O_CREAT, 0777, &attr);
  if (myMq == -1)
  {
    perror("Erreur ouverture");
  }

  //Création d'un thread
  pthread_create(&myThread, NULL, &run, NULL);
  displayScreen(MAIN_SCREEN);
};

extern void AdminUi_stop()
{
  mq_close(myMq);
  mq_unlink(BAL);
  pthread_join(myThread, NULL); //Attente que le thread se termine
  destroyTimer();
};

extern void AdminUI_quit()
{
  MqMessage eventMessage = {.event = E_AUI_QUIT};
  mq_send(myMq, (char *)&eventMessage, sizeof(eventMessage), 0);
};

extern void AdminUI_toggleEmergencyStop()
{
  MqMessage eventMessage = {.event = E_AUI_TES};
  mq_send(myMq, (char *)&eventMessage, sizeof(eventMessage), 0);
};

extern void AdminUI_clearLog()
{
  MqMessage eventMessage = {.event = E_AUI_CLEAR_LOG};
  mq_send(myMq, (char *)&eventMessage, sizeof(eventMessage), 0);
};

extern void AdminUI_goScreenLog()
{
  MqMessage eventMessage = {.event = E_AUI_GO_SLOG};
  mq_send(myMq, (char *)&eventMessage, sizeof(eventMessage), 0);
};

extern void AdminUI_backMainSreen()
{
  MqMessage eventMessage = {.event = E_AUI_BACK_MS};
  mq_send(myMq, (char *)&eventMessage, sizeof(eventMessage), 0);
};


//Méthodes static
static void *timerOut(void)
{
  MqMessage eventMessage = {.event = E_AUI_TIME_OUT};
  mq_send(myMq, (char *)&eventMessage, sizeof(eventMessage), 0);
  return NULL;
};

static void displayScreen(ScreenId idScreen)
{
  switch (idScreen)
  {
  case MAIN_SCREEN:
    printf("Main Screen : \n\r");
    printf("Voici la liste des commandes : \n \
        'R': Afficher Logs \n \
        'W': Arrêt urgence \n \
        'A': Quitter \n\r ");
    captureChoice();
    break;
  case LOG_SCREEN:
    printf("Log Screen :");
    printf("'E': Effacer Logs \n\
            'B': Retour Main screen \n\
            'A' : Quitter \n\r");
    captureChoice();
    break;
  default:
    break;
  }
};

extern void setEvents(Eventa *events){
  myEvents[currentEventNumber] = events[currentEventNumber];
}

extern void setEventsCount(int eventCount){
  currentEventNumber = eventCount;
}

static void updateEvents(){
  askEvents();
};

static void setTimer()
{
  struct itimerspec its;
  int error_code;
  its.it_value.tv_sec = 1;
  its.it_value.tv_nsec = 0;
  its.it_interval.tv_sec = 1;
  its.it_interval.tv_nsec = 0;
  error_code = timer_settime(timerId, 0, &its, NULL);
  if (error_code == -1)
  {
    errExit("Timer setTime error");
  }
}


//Annuler le timer
static void cancelTimer()
{
  struct itimerspec its;
  int error_code;
  its.it_value.tv_sec = 0;
  its.it_value.tv_nsec = 0;
  its.it_interval.tv_sec = 0;
  its.it_interval.tv_nsec = 0;
  error_code = timer_settime(timerId, 0, &its, NULL);
  if (error_code == -1)
  {
    errExit("Timer setTime error");
  }
}

//Détruitre le timer
static void destroyTimer()
{
  timer_delete(timerId);
}

static void AdminUI_mqReceive(MqMessage *this)
{
  mq_receive(myMq, (char *)this, sizeof(*this), NULL);
}


//Méthode run pour lire les messages de la BAL
static void *run()
{
  MqMessage mqMessage;
  Action action;
  while (myState != S_DEATH)
  {
    AdminUI_mqReceive(&mqMessage);
    if (transition[myState][mqMessage.event].destination == S_FORGET)
    {
      printf("Rien ne se passe");
    }
    else
    {
      action = transition[myState][mqMessage.event].action;
      myState = transition[myState][mqMessage.event].destination;
      performAction(action);
    }
  }
  return NULL;
}

//Appelle les méthodes en fonction de l'action en paramètre
static void performAction(Action action)
{
  switch (action)
  {
  case A_AUI_GO_SLOG:
    updateEvents();
    displayScreen(LOG_SCREEN);
    setTimer();
    break;
  case A_AUI_STOP_MAIN:
    AdminUi_stop();
    break;
  case A_AUI_STOP_LOG:
    cancelTimer();
    AdminUi_stop();
    break;
  case A_AUI_TES:
    toggleES();
    break;
  case A_AUI_CLEAR_LOG:
    clearEvents();
    previousEventNumber = 0;
    currentEventNumber = 0;
    break;
  case A_AUI_BACK_MAIN:
    displayScreen(MAIN_SCREEN);
    break;
  case A_AUI_ASK_EVENTS_COUNT:
    updateEvents();
    break;
  default:
    printf("Problème rencontré");
    break;
  }
}

/*
 * va prendre le choix de l'utilisateur
 *
 */
static void captureChoice()
{
  VelocityVector vel;
  while (quitter == 0)
  {
    printf("AdminUI (Main Screen)\n");
    system("stty -icanon min 1 time 0 -echo");
    commande = getchar();
    switch (commande)
    {
    case 'r':
      printf("ask4log \n");
      AdminUI_goScreenLog();
      break;
    //effacer Log
    case 'e':
      printf("askClearLog \n");
      AdminUI_clearLog();
      break;
    case 'a':
      printf("quit \n");
      printf("Déconnexion...\n\r");
      AdminUI_quit();
      quitter = 0;
      system("stty icanon echo ");
      break;
    case 'b':
      printf("Back main screen");
      AdminUI_backMainSreen();
      break;
    case 'w':
      AdminUI_toggleEmergencyStop();
      break;
    }
  }
}
