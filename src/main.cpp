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

#include "utils.h"
#include "main_activity.h"
#include "views/submission_node.h"
#include "views/top_bar/top_bar.h"
#include "gb.h"

int main(int argc, char* argv[])
{
    // Set log level
#ifndef __SWITCH__
    brls::Logger::setLogLevel(brls::LogLevel::DEBUG);
#endif

    // Init the app and i18n
    if (!brls::Application::init())
    {
        brls::Logger::error("Unable to init Borealis application");
        return EXIT_FAILURE;
    }

    brls::Application::createWindow(std::string("UltimateModShop \tVer. ") + APP_VERSION);

    // Have the application register an action on every activity that will quit when you press BUTTON_START
    brls::Application::setGlobalQuit(true);

    // Register custom views (including tabs, which are views)
    //brls::Application::registerXMLView("SubmissionNode", SubmissionNode::create);
    brls::Application::registerXMLView("TopBar", TopBar::create);

    //gb_test();

    // Create and push the main activity to the stack
    brls::Application::pushActivity(new MainActivity(), brls::TransitionAnimation::SLIDE_RIGHT);

    // Run the app
    while (brls::Application::mainLoop()) ;

    // Exit
    return EXIT_SUCCESS;
}
