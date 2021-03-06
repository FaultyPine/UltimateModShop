/*
    Copyright 2020-2021 natinusala
    Copyright 2019 p-sam

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ums_utils.h"
#include "splash_screen.h"

#include "zip/ZipUtil.hpp"

#include "main_activity.h"

int main(int argc, char* argv[])
{
// init for pc/switch
#ifdef __SWITCH__
    // Switch
    socketInitializeDefault();
#else 
    // PC
    brls::Logger::setLogLevel(brls::LogLevel::DEBUG);
#endif

    if (!brls::Application::init())
    {
        brls::Logger::error("Unable to init Borealis application");
        return EXIT_FAILURE;
    }

    brls::Application::createWindow(std::string("UltimateModShop \t\tVer. ") + APP_VERSION);

    brls::Application::setGlobalQuit(true);
    
    UnZip::PhysFSInit();

    setup();

    SplashScreen* s = new SplashScreen();
    brls::Application::pushActivity(s);

    BgTask::pushCallbackToQueue([]() {
        brls::Application::popActivity();
        brls::Application::pushActivity(new MainActivity());
    });


    while (brls::Application::mainLoop()) {

    }
    delete installed_mods;
    UnZip::PhysFSDeinit();

// exit for pc/switch
#ifdef __SWITCH__
    // Switch
    socketExit();
#else 
    // PC
    
#endif

    // Exit
    return EXIT_SUCCESS;
}
