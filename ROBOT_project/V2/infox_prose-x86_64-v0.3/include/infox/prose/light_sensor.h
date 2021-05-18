/**
 * @file light_sensor.h
 *
 * @brief Header de l'objet Capteur de luminosité.
 * Cet objet permet de gérer un capteur de luminosité.
 *
 * @version 9.0
 * @date 01/2016
 * @author Xavier MORICI - ESEO
 * /copyright BSD 2-clauses
 */

#ifndef __LightSensor_h_
#define __LightSensor_h_

#include "defs.h"

/**
 * \brief Objet Capteur de luminosité.
 *
 * ContactSensor est la structure de données pour un capteur de luminosité du Robot Lego.
 */
typedef struct LightSensor_t LightSensor;

 /**
 * \brief Constructeur d'un objet Capteur de luminosité.
 *
 * Fonction de création d'une nouvelle instance d'un objet Capteur de luminosité, représentation d'un capteur de luminosité. Elle retourne l'objet ainsi créé.
 *
 * \param [in] port Le port sur lequel le capteur de luminosité est connecté. \see LegoSensor
 *
 * \return L'instance de l'objet Capteur de luminosité ou NULL en cas d'erreur auquel cas errno est affecté \see EProse_e
 */
extern LightSensor* LightSensor_open(LegoSensor port);

/**
 * \brief Destructeur d'un objet Capteur de luminosité.
 *
 * Fonction de destruction d'une instance d'un objet Capteur de luminosité, représentation d'un capteur de luminosité. Elle libère le port allouée au capteur.
 *
 * \param [in] lightSensor L'instance d'un objet Capteur de luminosité à détruire.
 *
 * \retval 0 Si Si la destruction de l'instance de l'objet Capteur de luminosité est effectué.
 * \retval -1 Lors d'une erreur pendant la fermeture auquel cas errno est affecté \see EProse_e
 */
extern int LightSensor_close(LightSensor* lightSensor);

 /**
 * \brief Mutateur de l'état de la lumière de commande d'un objet Moteur.
 *
 * Change l'état de la lumière (LED) du capteur de luminosité.
 *
 * \param [in] sensor L'objet du capteur de luminosité.
 * \param [in] status La valeur de la lumière. TRUE pour allumée
 *
 * \retval 0 Si la lumière à changer d'état correctement
 * \retval -1 Lors d'une erreur auquel cas errno est affecté \see EProse_e
 */
extern int LightSensor_setLight(LightSensor* lightSensor, bool_e status);

 /**
 * \brief Accesseur de la valeur d'un objet Capteur de luminosité.
 *
 * Retourne la valeur du capteur de luminosité qui correspond à une valeur de tension analogique entre 0 et 2,5V.
 *
 * \param [in] lightSensor L'instance d'un objet Capteur de luminosité que l'on souhaite lire
 *
 * \return La valeur est retourné en mV ou -1 en cas d'erreur auquel cas errno est affecté \see EProse_e
 */
extern LightLevel LightSensor_getStatus(LightSensor* lightSensor);

#endif
