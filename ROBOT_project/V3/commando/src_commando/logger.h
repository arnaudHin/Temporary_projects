/**
 * @file logger.h
 * @author Adrien LE ROUX, Arnaud HINCELIN et Briac BEUTTER
 * @brief 
 * @version 0.1
 * @date 2021-05-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <stdbool.h>
/**
 * @brief startPolling
 * 
 */
extern void startPolling();

/**
 * @brief stopPolling
 * 
 */
extern void stopPolling();

/**
 * @brief askEvents
 * 
 */

extern void askEvents();

/**
 * @brief clearEvents
 * 
 */

extern void clearEvents();

/**
 * @brief signalES
 * 
 */
extern void signalES(bool s);