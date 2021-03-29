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
#include "components_tab.h"
#include "submission_node.h"
#include "top_bar.h"
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

    brls::Application::setCommonFooter(std::string("Ver. ") + APP_VERSION);
    brls::Application::createWindow("UltimateModShop");

    // Have the application register an action on every activity that will quit when you press BUTTON_START
    brls::Application::setGlobalQuit(true);

    // Register custom views (including tabs, which are views)
    brls::Application::registerXMLView("SubmissionNode", SubmissionNode::create);
    brls::Application::registerXMLView("TopBar", TopBar::create);
    brls::Application::registerXMLView("ComponentsTab", ComponentsTab::create);

    brls::getLightTheme().addColor("captioned_image/caption", nvgRGB(2, 176, 183));
    brls::getDarkTheme().addColor("captioned_image/caption", nvgRGB(51, 186, 227));

    gb_test();

    // Create and push the main activity to the stack
    brls::Application::pushActivity(new MainActivity(), brls::TransitionAnimation::SLIDE_RIGHT);

    // Run the app
    while (brls::Application::mainLoop()) ;

    // Exit
    return EXIT_SUCCESS;
}
