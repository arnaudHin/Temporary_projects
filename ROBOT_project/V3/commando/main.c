#include <stdio.h>
#include <src_commando/comCommando/facteurCommando.h>

//#include "./commando/serveur.h"




int main()
{

//   #ifdef INTOX
//     // Initialisation pour l'utilisation du simulateur Intox.
//     if (ProSE_Intox_init("127.0.0.1", 12345) == -1)
//     {
//       PProseError("Problème d'initialisation du simulateur Intox");
//       //return EXIT_FAILURE;
//     }
//   #endif

  

//    //on appelle cette fonction pr instancier robot, pilot
// /*
//   while (keepGoing)
//   {
//     keepGoing = 0;
//   }*/ 
//   printf("je suis dans le main de commando\n");
//   Serveur_new();   
//   Serveur_start();
//   Serveur_stop();
//   Serveur_free();
    FacteurCommando_New();
    FacteurCommando_Start();
    FacteurCommando_Stop();



  return 0;
}
