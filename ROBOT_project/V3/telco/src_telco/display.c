


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



static struct termios oldt, newt;





extern void displayScreen(RemoteUI_Screen_e screen){

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    switch (screen)
    {
    case CONNECT_SCREEN:
        printf("\033[2J\033[;H");
        printf("\n|_______________ECRAN DE CONNEXION_______________|\n");
        printf("1. Taper c : Choisir une adresse IP\n");
        printf("2. Taper q : Quitter\n");
        
        break;
    case ERROR_SCREEN:
        printf("\033[2J\033[;H");
        printf("\n|_______________ECRAN ERROR_______________|\n");
        printf("1. Taper a : Valider et revenir à écran de connexion\n");
        break;
    case CONNECTED_SCREEN:
        printf("\033[2J\033[;H");
        printf("\n|_______________ECRAN CONNECTE_______________|\n");
        printf("2. Taper q : Quitter\n");

        break;
    default:
        break;
    }
}







