#include "console.h"
#include "utils.hpp"



const std::string console_status = "\n" RED "X" RESET " to launch smash" MAGENTA "\t\t\t\tUltimateModShop Ver. " + std::string(APP_VERSION) + WHITE "\t\t\t\t\t" RED "+" RESET " to exit" RESET;

int main(int argc, char** argv) {

    console_init();
    console_set_status(console_status.c_str());
    socketInitializeDefault();
    curl_global_init(CURL_GLOBAL_DEFAULT);

    
    while (appletMainLoop()) {
        consoleClear();
        hidScanInput();

        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        
        consoleUpdate(NULL);
    }
    console_exit();
    curl_global_cleanup();
    socketExit();
    return 0;
}