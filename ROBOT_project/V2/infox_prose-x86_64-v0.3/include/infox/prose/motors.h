/**
 * @file motor.h
 *
 * @brief Header de l'objet Moteur.
 * Cet objet permet de gérer un moteur et le codeur associé.
 *
 * @version 9.0
 * @date 01/2016
 * @author Xavier MORICI - ESEO
 * /copyright BSD 2-clauses
 */

#ifndef __Motor_h_
#define __Motor_h_

#include "defs.h"

/// Valeur de retour lors d'une erreur pour la fonction \ref Motor_getCmd
#define E_GCMD -101
/// Valeur de retour lors d'une erreur pour la fonction \ref Motor_getIncrementalCoderValue
#define E_GCODER INT32_MAX

/**
 * \brief Objet Moteur.
 *
 * Motor est la structure de données pour un moteur du robot LEGO.
 */
typedef struct Motor_t Motor;

/**
 * \brief Constructeur d'un objet Moteur.
 *
 * Fonction de création d'une nouvelle instance d'un objet Moteur, représentation d'un moteur. Elle retourne l'objet ainsi créé.
 *
 * \param [in] port	Le port sur lequel le moteur est connecté. \see LegoMotor
 *
 * \return L'instance de l'objet Moteur ou NULL en cas d'erreur auquel cas errno est affecté \see EProse_e
 */
extern Motor* Motor_open(LegoMotor port);

/**
 * \brief Destructeur d'un objet Moteur.
 *
 * Fonction de destruction d'une instance d'un objet Moteur, représentation d'un moteur. Elle libère le port allouée au moteur.
 *
 * \param [in] motor L'instance d'un objet Moteur à détruire.
 *
 * \retval 0 Si la destruction de l'instance de l'objet Moteur est effectué.
 * \retval -1 lors d'une erreur pendant la fermeture auquel cas errno est affecté \see EProse_e
 */
extern int Motor_close(Motor* motor);

/**
 * \brief Mutateur de la valeur de commande d'un objet Moteur.
 *
 * Commande correspond au rapport cyclique (en %) de la PWM commandant le pont en H
 * permettant de délivrer la tension au moteur à courant continu correspondant.
 * Le signe de cmd indique le sens de rotation.
 *
 * \param [in] motor L'instance d'un objet Moteur que l'on souhaite commander
 * \param [in] cmd Le rapport cyclique (en %), compris entre -100 et 100
 *
 * \retval 0 lorsque la commande est bien passée au moteur
 * \retval -1 lors d'une erreur auquel cas errno est affecté \see EProse_e
 */
extern int Motor_setCmd(Motor* motor, Cmd cmd);

/**
 * \brief Accesseur de la valeur de commande d'un objet Moteur.
 *
 * Renvoie la vitesse de rotation actuellement configurée sur le moteur sélectionné.
 * Le signe indique le sens de rotation.
 *
 * \param [in] motor L'instance d'un objet Moteur que l'on souhaite lire
 *
 * \return La valeur de commande type Cmd comprise entre -100 et 100 ou E_GCMD en cas d'erreur auquel cas errno est affecté \see EProse_e
 */
extern Cmd Motor_getCmd(Motor* motor);

/**
 * \brief Mutateur de la valeur du compteur incrémental d'un objet Moteur.
 *
 * Configure la valeur du codeur du moteur, cette fonctionne ne fait pas bouger le moteur
 * mais ajoute un offset sur la valeur.
 *
 * \param [in] motor L'instance d'un objet Moteur où la valeur du compteur incrémental sera modifiée.
 * \param [in] position Position à envoyer au compteur incrémental du moteur.
 *
 * \retval 0 lorsque la valeur de la position est bien passée
 * \retval -1 lors d'un erreur auquel cas errno est affecté \see EProse_e
 */
extern int Motor_setIncrementalCoderValue(Motor* motor, IncrementalValue position);

/**
 * \brief Accesseur de la valeur du compteur incrémental d'un objet Moteur.
 *
 * Renvoie la valeur de rotation actuellement configurée sur le moteur sélectionné.
 * Le signe indique le sens de rotation.
 *
 * \brief Renvoie la position actuelle indiquée par le codeur associé au moteur sélectionné.
 * \param [in] motor L'instance d'un objet Moteur où la valeur du compteur incrémental sera lue.
 *
 * \return La valeur du compteur incrémental ou E_GCODER en cas d'erreur auquel cas errno est affecté \see EProse_e
 */
extern IncrementalValue Motor_getIncrementalCoderValue(Motor* motor);

/**
 * \brief Fourni la valeur correspondant à un tour de moteur.
 *
 * \return Le nombre d'impulsion pour un tour de roue.
 */
extern int Motor_getIncrementalCoderModulo();

#endif
