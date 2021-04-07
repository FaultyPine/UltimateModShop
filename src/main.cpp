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
#include "main_activity.h"
#include "splash_screen.h"
#include "views/top_bar/top_bar.h"
#include "views/main_window.h"

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

    brls::Application::createWindow(std::string("UltimateModShop \tVer. ") + APP_VERSION);

    // Have the application register an action on every activity that will quit when you press BUTTON_START
    brls::Application::setGlobalQuit(true);

    // Register custom views
    brls::Application::registerXMLView("TopBar", TopBar::create);
    brls::Application::registerXMLView("MainWindow", MainWindow::create);

    setup();

    // OpenGL is a single-threaded state machine, b/c of this, brls is absolutely not thread-safe at the moment and stuff like this makes a lotta undefined behavior.
    // In the future, brls will be, but for now I'll have to put this splash screen + load in background stuff on the backburner.
    // I'll definitely want to make some stuff async at some point - loading menus and such (although its pretty damn fast with multicalls so its not a huuuuuge deal).

    //SplashScreen* s = new SplashScreen();
    //brls::Application::pushActivity(s);
    //std::thread t([](){
        MainActivity* m = new MainActivity();
        brls::Application::pushActivity(m);
        //brls::Logger::debug("Loaded main activity!");
    //});
    //t.detach();

    while (brls::Application::mainLoop()) {

    }

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
