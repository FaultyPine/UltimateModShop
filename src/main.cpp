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

#include "views/tabs/mod_page.h"

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

    // Have the application register an action on every activity that will quit when you press BUTTON_START
    brls::Application::setGlobalQuit(true);

    setup();

    // OpenGL is a single-threaded state machine, b/c of this, brls is absolutely not thread-safe at the moment and stuff like this makes a lotta undefined behavior.
    // In the future, brls will be, but for now I'll have to put this splash screen + load in background stuff on the backburner.
    // I'll definitely want to make some stuff async at some point - loading menus and such (although its pretty damn fast with multicalls so its not a huuuuuge deal).

    //SplashScreen* s = new SplashScreen();
    //brls::Application::pushActivity(s);
    //std::thread t([](){
        //MainActivity* m = new MainActivity();
        //brls::Application::pushActivity(m);
        //brls::Logger::debug("Loaded main activity!");
    //});
    //t.detach();

    json temp_json = R"(
{
    "_aMetadata": {
        "_nRecordCount": 1,
        "_nTotalRecordCount": 2970,
        "_nPageCount": 2970
    },
    "_aRecords": [
        {
            "_sName": "Hyper Sonic",
            "_sModelName": "Mod",
            "_sProfileUrl": "https:\/\/gamebanana.com\/mods\/292237",
            "_aSubmitter": {
                "_sName": "AGhostsPumpkinSoup",
                "_sAvatarUrl": "https:\/\/images.gamebanana.com\/img\/av\/5ea0f0c6d6b92.png",
                "_sUpicUrl": "https:\/\/images.gamebanana.com\/img\/upic\/5ea6f4253447b.png"
            },
            "_tsDateUpdated": 1622767536,
            "_tsDateAdded": 1622767536,
            "_aPreviewMedia": [
                {
                    "_sType": "image",
                    "_sBaseUrl": "https:\/\/images.gamebanana.com\/img\/ss\/mods",
                    "_sFile": "60b972cd0ae04.jpg",
                    "_sFile530": "530-90_60b972cd0ae04.jpg",
                    "_sFile100": "100-90_60b972cd0ae04.jpg",
                    "_sFile220": "220-90_60b972cd0ae04.jpg"
                },
                {
                    "_sType": "image",
                    "_sBaseUrl": "https:\/\/images.gamebanana.com\/img\/ss\/mods",
                    "_sFile": "60b972a952cf9.jpg",
                    "_sFile530": "530-90_60b972cd0ae04.jpg",
                    "_sFile100": "100-90_60b972a952cf9.jpg",
                    "_sFile220": "220-90_60b972cd0ae04.jpg"
                },
                {
                    "_sType": "image",
                    "_sBaseUrl": "https:\/\/images.gamebanana.com\/img\/ss\/mods",
                    "_sFile": "60b972aa01087.jpg",
                    "_sFile530": "530-90_60b972cd0ae04.jpg",
                    "_sFile100": "100-90_60b972aa01087.jpg",
                    "_sFile220": "220-90_60b972cd0ae04.jpg"
                },
                {
                    "_sType": "image",
                    "_sBaseUrl": "https:\/\/images.gamebanana.com\/img\/ss\/mods",
                    "_sFile": "60b972aa0b39a.jpg",
                    "_sFile530": "530-90_60b972cd0ae04.jpg",
                    "_sFile100": "100-90_60b972aa0b39a.jpg",
                    "_sFile220": "220-90_60b972cd0ae04.jpg"
                },
                {
                    "_sType": "image",
                    "_sBaseUrl": "https:\/\/images.gamebanana.com\/img\/ss\/mods",
                    "_sFile": "60b972ab1abd9.jpg",
                    "_sFile530": "530-90_60b972cd0ae04.jpg",
                    "_sFile100": "100-90_60b972ab1abd9.jpg",
                    "_sFile220": "220-90_60b972cd0ae04.jpg"
                }
            ],
            "_sText": "This mod changes Sonic's Final Smash to \"Hyper Sonic\" and makes him changes to a couple different colors.&nbsp;<br><br>This isn't an edit to any models itself so should work with any Super Sonic mod that uses the regular model names.&nbsp;",
            "_sDescription": "",
            "_aCategory": {
                "_idRow": 7581,
                "_sName": "Sonic",
                "_sIconUrl": "https:\/\/images.gamebanana.com\/img\/ico\/ModCategory\/5bff4a2f110cd.png"
            },
            "_aRootCategory": {
                "_idRow": 7581,
                "_sName": "Skins",
                "_sIconUrl": "https:\/\/images.gamebanana.com\/img\/ico\/ModCategory\/60b57708604b3.png"
            },
            "_aGame": {
                "_idRow": 6498
            },
            "_nViewCount": 0,
            "_nLikeCount": 2,
            "_nDownloadCount": 0,
            "_aFiles": [
                {
                    "_idRow": "584614",
                    "_sFile": "hyper_sonic_32f85.rar",
                    "_nFilesize": 878,
                    "_sDownloadUrl": "https:\/\/gamebanana.com\/dl\/584614",
                    "_sDescription": "",
                    "_tsDateAdded": 1622766308,
                    "_nDownloadCount": 0,
                    "_sAnalysisResult": "Archive passed",
                    "_bContainsExe": false
                }
            ],
            "_aModManagerIntegrations": {
                "584614": [
                    {
                        "_sInstallerName": "Quasar 2.1.0.1",
                        "_sInstallerUrl": "https:\/\/gamebanana.com\/tools\/6722",
                        "_sIconClasses": "QuasarModManagerIcon",
                        "_sDownloadUrl": "quasar:https:\/\/gamebanana.com\/mmdl\/584614,Mod,292237,rar"
                    }
                ]
            }
        }
    ]
}
    )"_json;

    gb::GbSubmission* tmp_sub = new gb::GbSubmission {.submission_data = temp_json};
    SubmissionNode* sub = new SubmissionNode(tmp_sub);
    brls::Application::pushActivity(new ModPage(sub));

    while (brls::Application::mainLoop()) {

    }
    delete installed_mods;

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
