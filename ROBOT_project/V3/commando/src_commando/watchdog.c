/**
 * @file compute_pi.c
 *
 * @author team FORMATO, ESEO
 *
 * @section License
 *
 * The MIT License
 *
 * Copyright (c) 2016, Jonathan ILIAS-PILLET (ESEO)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "watchdog.h"
#include "../../utils.h"
#include <malloc.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN
#define errExit(msg)        \
	do                      \
	{                       \
		perror(msg);        \
		exit(EXIT_FAILURE); \
	} while (0)

static void print_siginfo(siginfo_t *si)
{
	timer_t *tip;										// On créé un timer
	int or ;											// stockera la variable renvoyer par la fonction timer_getoverrun
	tip = si->si_value.sival_ptr;						//Données transmises à la notification - en l'occurence la valeur du pointeur
	printf("sival_ptr = %p; ", si->si_value.sival_ptr); // On affiche la notification
	printf("*sival_ptr = 0x%lx\n", (long)*tip);			// On affiche l'adresse de stockage de la variable de notification
	or = timer_getoverrun(*tip);						// Cette fonction renvoie le nombre de dépassements d'une minuterie POSIX d'un processus
	if (or == -1)
		errExit("timer_getoverrun"); // Erreur
	else
		printf("overrun count = %d\n", or); // On renvoie la valeur de l'expiration, si 0 alors aucune expiration a eu lieu
}

struct Watchdog_t
{
	/* TODO: POSIX timer */ /**< POSIX Timer*/
	timer_t timerid1;
	uint32_t myDelay;			 /**< configured delay */
	WatchdogCallback myCallback; /**< function to be called at delay expiration */
};

/**
 * Calls the watchdog callback when the delay of the timer expires
 *
 * @param handlerParam must be the watchdog reference
 */
// static void mainHandler (Watchdog *this)
// {

// 	Watchdog *theWatchdog = this;
// 	//printf("signal appelé %d\n", sig);
// 	//print_siginfo(si);
// 	//signal(sig, SIG_IGN); // ignore le signal
// 	theWatchdog->myCallback (theWatchdog);
// }
static void mainHandler(union sigval signal)
{

	//	Watchdog *theWatchdog = this;
	Watchdog *theWatchdog = signal.sival_ptr;

	//printf("signal appelé %d\n", sig);
	//print_siginfo(si);
	//signal(sig, SIG_IGN); // ignore le signal
	theWatchdog->myCallback(theWatchdog);
	//print_siginfo(signal);
}

Watchdog *Watchdog_construct(uint32_t thisDelay, WatchdogCallback callback)
{

	//	struct sigaction sa;
	//	sa.sa_flags = SA_SIGINFO; // On spécifie le type de traitement du signal - SIGINFO spécifie un masque du signal qui devra être bloquant pendant l'éxecution du gestionnaire des signaux
	//	sa.sa_sigaction = mainHandler; // On spécifie l'action à être associée au signal
	//sigemptyset(&sa.sa_mask);
	//if(sigaction(SIG, &sa, NULL) == -1) // on change l'action de la reception du signal
	//		errExit("sigaction");
	// On bloque le timer de façon temporaire
	//	printf("Signal Bloquant %d\n", SIG);
	//	sigemptyset(&mask);
	//	sigaddset(&mask, SIG);
	//	if(sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
	//		errExit("sigprocmask");
	// allocates and initializes the watchdog's attributes

	// Begin
	Watchdog *result;
	struct sigevent sev;
	result = (Watchdog *)malloc(sizeof(Watchdog)); // On alloue de la mémoire à result
	STOP_ON_ERROR(result == NULL);
	result->myDelay = thisDelay;
	result->myCallback = callback;

	// TODO: creates the POSIX timer
	//On créé le timer

	memset(&sev, 0, sizeof(struct sigevent));
	sev.sigev_notify = SIGEV_THREAD; // On spécifie la méthode de notification
	sev.sigev_notify_function = &mainHandler;
	sev.sigev_value.sival_ptr = &result->timerid1;
	if (timer_create(CLOCK_REALTIME, &sev, &(result->timerid1)) == -1) // on créé le timer avec les paramètres passés aux variables précèdemment
		//if(timer_create(CLOCK_REALTIME, &sev, result) == -1) // on créé le timer avec les paramètres passés aux variables précèdemment
		errExit("timer_create");

	//result->timerid1 = timerid;

	printf("L'id du timer est 0x%ld\n", (long)result->timerid1); // On affiche l'id sous forme hexadécimal
	return result;
}

void Watchdog_start(Watchdog *this)
{
	struct itimerspec its;
	memset(&its, 0, sizeof(struct itimerspec));
	//long long freq_nanosecs;
	// TODO: starts the POSIX timer
	//freq_nanosecs = 1000000000;
	//atoll(); //permet de transformer une chaîne de caratère contenant la représentation textuel d'un entier en un entier très long
	if (this->myDelay == 250)
	{
		its.it_value.tv_nsec = this->myDelay * 1000000;
	}
	else
	{
		its.it_value.tv_sec = this->myDelay; // indique le temps restant pour la prochaine expiration
	}
	//its.it_value.tv_nsec = this->myDelay*1000000000;
	//its.it_interval.tv_sec = its.it_value.tv_sec; // spécifie la période entre chaque expiration de timer
	//its.it_interval.tv_nsec = its.it_value.tv_nsec;
	printf("myDelay %d", this->myDelay);
	printf("L'id du timer est 0x%lx\n", (long)this->timerid1); // On affiche l'id sous forme hexadécimal

	if (timer_settime(this->timerid1, 0, &its, NULL) == -1)
		;				// arme la minuterie indiquée par timerid1 et donne la valeur initial et et le nouvel intervalle / si valeur null, alors déasarmé
	printf("bnojours"); //errExit("timer_settime");
	printf("Le delay du timer est de %ld \n", (long)this->timerid1);

	//printf("Dormir pendant %d secondes \n", atoi()); // permet de transformer une chaine de caractère représentant une valeur entière en valeur numérique  de type int
}

void Watchdog_cancel(Watchdog *this)
{
	//int error_code;
	struct itimerspec its;
	its.it_value.tv_sec = 0;								   // indique le temps restant pour la prochaine expiration
	printf("L'id du timer est 0x%lx\n", (long)this->timerid1); // On affiche l'id sous forme hexadécimal

	timer_settime(this->timerid1, 0, &its, NULL); //On force l'arrêt du timer, pour cela le temps d'expiration doit être égale à 0.

	// TODO: disarms the POSIX timer
}

void Watchdog_destroy(Watchdog *this)
{
	//int error_code;

	// TODO: disarms and deletes the POSIX timer
	timer_delete(this->timerid1); // On détruit le timer
	// then we can free memory
	free(this); // On libère la mémoire
}
