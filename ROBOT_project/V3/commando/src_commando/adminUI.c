
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

#define errExit(msg) \
  do                 \
  {                  \
    perror(msg);     \
  } while (0)

#define MQ_max (5)

//A supprimer
typedef enum Direction_t Direction;
typedef enum VelocityVector_t VelocityVector;
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
  A_AUI_NOP = 0,
  A_AUI_STOP_MAIN,
  A_AUI_STOP_LOG,
  A_AUI_TES,
  A_AUI_ASK_EVENTS_COUNT,
  A_AUI_CLEAR_LOG,
  A_AUI_SET_EVENTS_COUNT,
  A_AUI_SET_EVENTS,
  A_AUI_BACK_MAIN,
  A_AUI_GO_SLOG,
  NB_ACTION
} Action;

//Definition des évènements possible
typedef enum
{
  E_AUI_STOP = 0,
  E_AUI_QUIT,
  E_AUI_TES,
  E_AUI_CLEAR_LOG,
  E_AUI_BACK_MS,
  E_AUI_GO_SLOG,
  E_AUI_TIME_OUT,
  E_AUI_SET_EVENTS_COUNT,
  E_AUI_SET_EVENTS,
  NB_EVENT
} Event;

//Definition des transitions
static Transition transition[NB_STATE][NB_EVENT] =
    {
        [S_MAIN_SCREEN][E_AUI_STOP] = {S_DEATH, A_AUI_STOP_MAIN},
        [S_MAIN_SCREEN][E_AUI_GO_SLOG] = {S_WAIT_NUMBER_EVENTS, A_AUI_ASK_EVENTS_COUNT},
        [S_MAIN_SCREEN][E_AUI_TES] = {S_MAIN_SCREEN, A_AUI_TES},
        [S_LOG_SREEN][E_AUI_CLEAR_LOG] = {S_LOG_SREEN, A_AUI_CLEAR_LOG},
        [S_LOG_SREEN][E_AUI_TES] = {S_LOG_SREEN, A_AUI_TES},
        [S_LOG_SREEN][E_AUI_STOP] = {S_DEATH, A_AUI_STOP_LOG},
        [S_LOG_SREEN][E_AUI_TIME_OUT] = {S_WAIT_NUMBER_EVENTS, A_AUI_BACK_MAIN},
        [S_LOG_SREEN][E_AUI_BACK_MS] = {S_MAIN_SCREEN, A_AUI_BACK_MAIN},
        [S_WAIT_EVENTS][E_AUI_SET_EVENTS_COUNT] = {S_WAIT_EVENTS, A_AUI_SET_EVENTS_COUNT},
        [S_WAIT_EVENTS][E_AUI_SET_EVENTS] = {S_LOG_SREEN, A_AUI_SET_EVENTS},
};

//Definition des écrans possibles
enum ScreenId
{
  MAIN_SCREEN = 0,
  LOG_SCREEN
};

//Definition des évènements
typedef enum
{
  E_FORGET = 0,
  NB_EVENT
} Event;

// Attributs de AdminUi
int currentEventNumber;
int previousEventNumber;
static State myState = S_MAIN_SCREEN;
static pthread_t myThread;

/**
 * BAL
 */
static const char BAL[] = "/BALaUI";
static mqd_t myMq;          //On déclare un descripteur de notre BAL qui permettra de l'ouvrir et de la fermer
static struct mq_attr attr; //On déclare un attribut pour la fonction mq_open qui est une structure spécifique à la file pour la configurer (cf l.64)

//Identifiant du timer
timer_t timerId;

//Structure pour l'état de destination et l'action à faire
typedef struct
{
  State destination;
  Action action;
  Event event;
} Transition;

//Structure pour les messages
typedef struct
{
  Event event;
} MqMessage;

char commande;
int quitter;
//Prototypes static
static void timerOut();
static void displayScreen(ScreenId idScreen);
static void updateEvents(){};
static void setTimer();
static void cancelTimer();
static void destroyTimer();
static void AdminUI_mqReceive(MqMessage *this);
static void *run();
static void performAction(MqMessage MqMessage, Action action);
static void captureChoice();
static askMvt(Direction direction);

//Méthodes externes

extern void AdminUI_new()
{
  struct sigevent sev;
  sev.sigev_notify = SIGEV_THREAD;
  sev.sigev_value.sival_ptr = &timerId;
  sev.sigev_notify_function = timerOut();
  int error_code;
  error_code = timer_create(CLOCK_REALTIME, &sev, &timerId);
  if (error_code = -1)
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
  int error_open;
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

static void timerOut(void)
{
  MqMessage eventMessage = {.event = E_AUI_TIME_OUT};
  mq_send(myMq, (char *)&eventMessage, sizeof(eventMessage), 0);
};

static void displayScreen(ScreenId idScreen)
{
  switch (idScreen)
  {
  case MAIN_SCREEN:
    printf("Main Screen : \n\r");
    printf("Voici la liste des commandes : \n \
        'Z': Avancer \n \
        'S': Reculer \n \
        'Q': Droite \n \
        'D': Gauche \n  \
        'W': Stop \n \
        'R': Afficher Logs \n \
        'E': Effacer Logs \n \
        'F': Afficher l'état du robot \n \
        'A': Quitter \n\r ");
    break;
  case LOG_SCREEN:
    printf("Log Screen :");
    break;
  default:
    break;
  }
};

static void updateEvents(){};

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

static void destroyTimer()
{
  timer_delete(timerId);
}

static void AdminUI_mqReceive(MqMessage *this)
{
  mq_receive(myMq, (char *)this, sizeof(*this), NULL);
}

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
      performAction(mqMessage, action);
    }
  }
}

static void performAction(MqMessage MqMessage, Action action)
{
  MqMessage mqMessage = mqMessage;
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
    Pilot_toggleES();
    break;
  case A_AUI_CLEAR_LOG:
    Logger_clearEvents();
    previousEventNumber = 0;
    currentEventNumber = 0;
    break;
  case A_AUI_BACK_MAIN:
    displayScreen(MAIN_SCREEN);
    break;
  case A_AUI_ASK_EVENTS_COUNT:
    Logger_askEventsCount();
    break;
  default:
    Printf("Problème rencontré");
    break;
  }
}

/*
 * va prendre le choix de l'utilisateur (afficher les logs, faire avancer le robot ...)
 *
 */
static void captureChoice()
{
  VelocityVector vel;
  while (quitter == 0)
  {
    printf("je suis dans capture choice dans Admin\n");
    system("stty -icanon min 1 time 0 -echo");
    commande = getchar();
    switch (commande)
    { //mouvement
    case 'z':
      printf("CaptureChoice cas forward \n");
      askMvt(FORWARD);
      break;
    case 'q':
      printf("CaptureChoice cas left \n");
      askMvt(LEFT);
      break;
    case 'd':
      printf("CaptureChoice cas right \n");
      askMvt(RIGHT);
      break;
    case 's':
      printf("CaptureChoice cas backward \n");
      askMvt(BACKWARD);
      break;
    case 'w':
      printf("CaptureChoice cas Stop \n");
      askMvt(STOP);
      break;
      //log
    case 'r':
      printf("CaptureChoice cas ask4log \n");
      AdminUI_goScreenLog();
      break;
    //effacer Log
    case 'e':
      printf("CaptureChoice cas askClearLog \n");
      AdminUI_clearLog();
      break;
    case 'a':
      printf("CaptureChoice cas quit \n");
      printf("Déconnexion...\n\r");
      //quit();
      quitter = 0;
      system("stty icanon echo ");
      break;
    }
  }
}

static askMvt(Direction direction)
{
  VelocityVector vel;
  vel.power = 50;
  vel.dir = direction;
  Pilot_setVelocity(vel);
}