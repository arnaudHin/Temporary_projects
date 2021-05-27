/**
 * Autor : Arnaud Hincelin
 * File : display.c
 */


#include<stdio.h>
#include <stdbool.h>
#include <termios.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>


#include <display.h>
#include "remoteUI.h"
#include "../../utils.h"
#include "../../commun.h"


extern void displayScreen(RemoteUI_Screen_e screen){


    switch (screen)
    {
    case CONNECT_SCREEN:
        PRINT("\033[2J\033[;H");
        PRINT("\n|_______________ECRAN DE CONNEXION_______________|\n");
        PRINT("1. Taper c : Choisir une adresse IP\n");
        PRINT("2. Taper q : Quitter\n");
        
        break;
    case ERROR_SCREEN:
        PRINT("\033[2J\033[;H");
        PRINT("\n|_______________ECRAN ERROR_______________|\n");
        PRINT("1. Taper v : Valider et revenir à écran de connexion\n");
        break;
    case CONNECTED_SCREEN:
        PRINT("\033[2J\033[;H");
        PRINT("\n|_______________ECRAN CONNECTE_______________|\n");
        PRINT("1. Taper z : Avancer\n");
        PRINT("2. Taper s : Reculer\n");
        PRINT("3. Taper d : Tourner à droite\n");
        PRINT("4. Taper q : Tourner à gauche\n");
        PRINT("5. Taper s : Stopper\n");
        PRINT("6. Taper w : Quitter\n");

        break;
    default:
        break;
    }
}







