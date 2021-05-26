/**
 * Autor : Arnaud Hincelin
 * File : utils.h
 */

#ifndef UTIL_H_
#define UTIL_H_


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#define PRINT printf

#define TRACE(fmt, ...) do { \
    fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    fflush (stderr); \
    } while(0);

#define STOP_ON_ERROR(error_condition) do { \
	if(error_condition) { \
		fprintf (stderr, "*** Error (%s) at \
		%s:%d\nExiting\n", \
		#error_condition, __FILE__, __LINE__); \
		perror (""); \
		exit (EXIT_FAILURE); }                     \
    } while (0);


// #else
// #define TRACE(fmt, ...)
// #define STOP_ON_ERROR(error_condition)  #error "STOP_ON_ERROR must not be used in release builds"

#endif /* UTIL_H_ */
