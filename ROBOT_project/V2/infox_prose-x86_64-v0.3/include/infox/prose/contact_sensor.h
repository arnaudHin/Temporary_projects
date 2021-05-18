/**
 * @file contact_sensor.h
 *
 * @brief Header de l'objet Capteur de contact.
 * Cet objet permet de gérer un capteur de contact.
 *
 * @version 9.0
 * @date 01/2016
 * @author Xavier MORICI - ESEO
 * /copyright BSD 2-clauses
 */

#ifndef __ContactSensor_h_
#define __ContactSensor_h_

#include "defs.h"

/**
 * \brief Objet Capteur de contact.
 *
 * ContactSensor est la structure de données pour un capteur de contact du Robot Lego.
 */
typedef struct ContactSensor_t ContactSensor;

/**
 * \brief Constructeur d'un objet Capteur de contact.
 *
 * Fonction de création d'une nouvelle instance d'un objet Capteur de contact, représentation d'un capteur de contact. Elle retourne l'objet ainsi créé.
 *
 * \param [in] port Le port sur lequel le capteur de contact est connecté. \see LegoSensor
 *
 * \return L'instance de l'objet Capteur de contact ou NULL en cas d'erreur auquel cas errno est affecté \see EProse_e
 */
extern ContactSensor* ContactSensor_open(LegoSensor port);

/**
 * \brief Destructeur d'un objet Capteur de contact.
 *
 * Fonction de destruction d'une instance d'un objet Capteur de contact, représentation d'un capteur de contact. Elle libère le port allouée au capteur.
 *
 * \param [in] contactSensor L'instance d'un objet Capteur de contact à détruire.
 *
 * \retval 0 Si la destruction de l'instance de l'objet Capteur de contact est effectué.
 * \retval -1 Lors d'une erreur pendant la fermeture auquel cas errno est affecté \see EProse_e
 */
extern int ContactSensor_close(ContactSensor* contactSensor);

/**
 * \brief Accesseur de l'état d'un objet Capteur de contact.
 *
 * Renvoie l'état d'une instance d'un objet Capteur de contact.
 *
 * \param [in] contactSensor L'instance d'un objet Capteur de contact que l'on souhaite lire
 *
 * \retval RELEASED Quand le capteur de contact est dans l'état relâché
 * \retval PRESSED Quand le capteur de contact est dans l'état appuyé
 * \retval ERROR Lors d'un erreur auquel cas errno est affecté \see EProse_e
 */
extern ContactStatus ContactSensor_getStatus(ContactSensor* contactSensor);

#endif

