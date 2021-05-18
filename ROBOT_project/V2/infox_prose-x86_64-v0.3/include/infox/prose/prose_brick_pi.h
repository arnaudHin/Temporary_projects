/**
 * @file prose_brick_pi.h
 *
 * @brief Header de l'objet BrickPi pour ProSE.
 * Cet objet permet de gérer le fil d'exécution (Thread) servant le maintient de la BrickPi en fonctionnement correct dans sa librairie d'origine.
 * Il pourrait être remplacer par un Deamon dans une futur version.
 *
 * @version 9.0
 * @date 01/2016
 * @author Xavier MORICI - ESEO
 * /copyright BSD 2-clauses
 */

#ifndef __ProseBrickPi_h_
#define __ProseBrickPi_h_

#include "defs.h"

/**
 * \fn int BrickPi_init ()
 * \brief Constructeur du BrickPi.
 *
 * \return La valeur 0 si l'initialisation est réussie. En cas d'erreur, la valeur -1 est retournée.
 */
int BrickPi_init();

/**
 * \fn int BrickPi_destroy ()
 * \brief Destructeur du BrickPi.
 */
void BrickPi_destroy();

 /**
 * \brief Fonction d'affichage d'erreur de la librairie.
 *
 * Afficher un message d'erreur sur la console.
 *
 * \param [in] msg Le message d'erreur à afficher avant la description de l'erreur
 *
 * \return Retourne la chaine de caractère de la description de l'erreur si l'entrée msg est NULL sinon NULL.
 */
char const * PProseError(char *msg);

#endif
