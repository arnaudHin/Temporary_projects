/**
 * @file prose_infox.h
 *
 * @brief gestion d'un simulateur de robot Intox avec la lib Infox.
 *
 * @version 0.1
 *
 * @author Matthias Brun
 *
 * /copyright BSD 2-clauses
 */

#ifndef _PROSE_INFOX_H
#define _PROSE_INFOX_H

/**
 * Initialisation nécessaire pour l'utilisation du simulateur Intox
 * (simulateur du robot).
 *
 * \param adresse l'adresse IP du serveur de simulation du robot (sous forme d'une chaîne de caractères).
 * \param port le port du service du serveur de simulation du robot.
 *
 * \return état de l'utilisation du serveur Intox.
 * \retval 0 si l'utilisation est possible.
 * \retval -1 en cas d'erreur auquel cas errno est renseigné.
 */
int ProSE_Intox_init(const char * adresse, const int port);

/**
 * Fermeture de la communication avec le simulateur Intox
 * (simulateur du robot).
 */
void ProSE_Intox_close();

/**
 * \brief Fonction d'affichage d'erreur de la librairie.
 *
 * Afficher un message d'erreur sur la sortie standard.
 * Fonction également disponible sur la version brick-pi (cf. prose_brick_pi.h).
 *
 * \param [in] msg Le message d'erreur à afficher avant la description de l'erreur
 *
 * \return Retourne la chaîne de caractère de la description de l'erreur si l'entrée msg est NULL sinon NULL.
 */
char const * PProseError(char *msg);

#endif /* _PROSE_INFOX_H */
