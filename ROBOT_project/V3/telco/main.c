
//#include "mrRobot/adminui.h"

#include "src_telco/remoteUI.h"
#include <stdio.h>




/**
 * Main Function
 */
int main() {

    RemoteUI_New();
    RemoteUI_Start();
    RemoteUI_Stop();
    RemoteUI_Free();

    return 0;
}

