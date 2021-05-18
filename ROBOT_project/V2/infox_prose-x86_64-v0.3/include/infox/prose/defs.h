/**
 * @file defs.h
 *
 * @brief Définition de types utilisés dans la bibliothèque, et disponibles aux programmes l'utilisant.
 *
 * @version 10.0
 * @date 01/2016
 * @author Xavier MORICI - ESEO
 * @author Matthias Brun (ajout utilisation de Intox/Infox)
 *
 * /copyright BSD 2-clauses
 */

#ifndef __Defs_h_
#define __Defs_h_

#include <stdint.h>

/**
 * \brief Erreur Prose.
 *
 * EProse_e représente une erreur de la librairie Prose.
 *
 * Remarque (MB) :
 * Valeurs des numéros d'erreur de errno.
 * Démarrage à 1 (errno n'est pas mis à zéro pour cause d'erreur cf. man errno).
 */
typedef enum {
	// commentaire (MB) NOT_OK=0,		/*!< Échec. */
	OK=1,			/*!< Succès. */
	E_INIT,			/*!< BrickPi non initialisé. */
	E_BRICKPI,		/*!< Erreur interne du BrickPi. */
	E_PORT,			/*!< Port invalide. */
	E_CMD,			/*!< Commande invalide. */
	E_MALLOC,		/*!< Malloc impossible. */
	E_TID,			/*!< Processus invalide. */
	E_NULL,			/*!< Instance NULL. */

	// Ajout (MB) pour Infox/Intox :

	E_NO_BRICK,			/**< erreur d'utilisation d'une fonction destinée à la brick-pi. */

	E_SOCKET,			/**< erreur de création de la socket de communication avec le simulateur */
	E_CONNECT,			/**< erreur de connexion avec le simulateur (erreur par défaut) */
	E_CONNECT_REFUSED,	/**< erreur de connexion avec le simulateur : connexion refusée */
	E_CONNECT_NET,		/**< erreur de connexion avec le simulateur : réseau inaccessible */
	E_CONNECT_HOST,		/**< erreur de connexion avec le simulateur : serveur inaccessible */
	E_CONNECT_TIMEOUT,	/**< erreur de connexion avec le simulateur : échéance de temps */

	E_INTOX, 			/**< erreur d'accès au simulateur Intox */
	E_INTOX_LOST, 		/**< perte d'accès au simulateur Intox */

	E_MOTOR_INV,		/**< erreur de tentative d'accès à un moteur invalide (port incorrecte) */
	E_MOTOR_NULL,		/**< erreur de tentative d'accès à un moteur null */
	E_MOTOR_CMD_VAL,	/**< valeur de puissance du moteur invalide (intervalle [-100%, +100%]) */
	E_MOTOR_COD_VAL,	/**< valeur de codeur du moteur invalide (intervalle [-100%, +100%]) */

	E_CONTACT_INV, 		/**< erreur de tentative d'accès à un capteur de contact invalide (port incorrecte) */
	E_CONTACT_NULL, 	/**< erreur de tentative d'accès à un capteur de contact null */

	E_LIGHT_INV, 		/**< erreur de tentative d'accès à un capteur de luminosité invalide (port incorrecte) */
	E_LIGHT_NULL 		/**< erreur de tentative d'accès à un capteur de luminosité null */

} EProse_e;

/**
 * \brief Port d'un moteur.
 *
 * LegoSensor représente un port du moteur.
 */
typedef enum {
	MA=0,	/*!< Le port A de la BrickPi. */
	MB=1,	/*!< Le port B de la BrickPi. */
	MC=2,	/*!< Le port C de la BrickPi. */
	MD=3	/*!< Le port D de la BrickPi. */
} LegoMotor;

/**
 * \brief Port d'un capteur.
 *
 * LegoSensor représente un port du capteur : .
 */
typedef enum {
	S1=0,	/*!< Le port 1 de la BrickPi. */
	S2=1,	/*!< Le port 2 de la BrickPi. */
	S3=2,	/*!< Le port 3 de la BrickPi. */
	S4=3	/*!< Le port 4 de la BrickPi. */
} LegoSensor;

/**
 * \brief État du capteur de contact.
 *
 * ContactStatus représente les différents états de l'objet ContactSensor.
 */
typedef enum
{
	ERROR=-1, 		/*!< Erreur. */
	RELEASED=0,   	/*!< Le bouton est relâché. */
	PRESSED=1 		/*!< Le bouton est pressé. */
}
ContactStatus;


/**
 * \brief Boolean.
 *
 * bool_e représente un booléen, il existe sous 2 états.
 */
typedef enum
{
   FALSE=0,		/*!< Valeur logique faux. */
   TRUE=1		/*!< Valeur logique vrai. */
} bool_e;


/**
 * \brief Commande moteur.
 *
 * Cmd représente le rapport cyclique, en pourcentage, entre 0 et 100 inclus. Peut être négatif pour indiquer le sens.
 */
typedef signed char Cmd;

/**
 * \brief Taux de luminosité.
 *
 * LightLevel représente le taux de luminosité reçu par un capteur de luminosité.
 */
typedef int LightLevel;


/**
 * \brief Valeur d'un codeur incremental
 *
 * IncrementalValue la valeur d'un codeur incremental de roue.
 */
typedef int32_t IncrementalValue;


#endif /* _DEFS_H */
