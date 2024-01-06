#include "MainServer.h"

#include "IniParser/ParserInfoFile.h"
#include "IniParser/PostParsingStruct.h"

#include "Defines.h"

using namespace std;

using namespace IniParser;

MainServer::MainServer()
{
    s_Server = new Server(this);
}

MainServer::~MainServer()
{
    delete s_Server;
    delete s_ServerList;
    map<string, Game*>::iterator iter1, iter2;
    for(iter1 = s_ListGameClass.begin(), iter2 = s_ListGameClass.end(); iter1 != iter2; iter1++) if(iter1->second != 0) delete iter1->second;
}

bool MainServer::load()
{
    int id_srv = 0;
    ParserInfoFile prs;
    s_ServerList = prs.getParsedFromFile("ServerData/ServerList.ini");
    if(!s_ServerList) return false;
    map<string, map<string, string > >::iterator iter;
    for(iter = s_ServerList->getMapVariables().begin(); iter != s_ServerList->getMapVariables().end(); iter++)
    {
        if(iter->first.find("Server") == 0)
        {
            id_srv++;
            cout << endl << "Server (ID = " << id_srv << "):" <<endl;
            Game* gm = new Game;
            if(s_ListGameClass.find(s_ServerList->getValue(iter->first, "id")) == s_ListGameClass.end()) s_ListGameClass[s_ServerList->getValue(iter->first, "id")] = gm;
            else
            {
                cout << "Error! Server with id = '" << s_ServerList->getValue(iter->first, "id") << "' already exists." << endl;
                return false;
            }
            if(!gm)
            {
                cout<<"Error creating GameClass!"<<endl;
                return 0;
            }
            NetClient* nc = new NetClient(gm);
            gm->s_NetClient = nc;
            string name = "Player", pass = "", host = "127.0.0.1";
            int port = 11237;

            nc->s_NetInfoStruct->s_Name = name;
            nc->s_NetInfoStruct->s_Pass = pass;
            nc->s_NetInfoStruct->s_Host = host;
            nc->s_NetInfoStruct->s_Port = port;

            nc->s_NetInfoStruct->s_goGame = false;
            nc->s_NetInfoStruct->s_Mode = "singleplayer";

            ParserInfoFile prsfl;

            gm->readConfig();

            gm->s_GameInfo->readKeys(gm->s_IniFile);
            gm->s_DisplayStruct->s_GameResolutionX = atoi( (gm->s_IniFile->getValue("video","resolutionX") ).c_str() );
            gm->s_DisplayStruct->s_GameResolutionY = atoi( (gm->s_IniFile->getValue("video","resolutionY") ).c_str() );
            gm->s_DisplayStruct->s_WindowResolutionX = atoi( (gm->s_IniFile->getValue("video", "windowresolutionX") ).c_str() );
            gm->s_DisplayStruct->s_WindowResolutionY = atoi( (gm->s_IniFile->getValue("video", "windowresolutionY") ).c_str() );
            if(gm->s_DisplayStruct->s_GameResolutionY <= 0 || gm->s_DisplayStruct->s_GameResolutionX <= 0)
            {
                gm->s_Logger->registerEvent(EVENT_TYPE_LOGIC_VIOLATION, "Game resolution is < 0 or equals 0.", true);
                system("pause");
                return 0;
            }
            //gm->s_Window = new Window("DD2 Remake: ME v0.2 pre-beta", gm->s_DisplayStruct->s_WindowResolutionX, gm->s_DisplayStruct->s_WindowResolutionY, atoi( (gm->s_IniFile->getValue("video","scale") ).c_str() ));
            /*gm->s_Window = new Window(nostart);
            gm->s_Window->geometry(gm->s_DisplayStruct->s_WindowResolutionX, gm->s_DisplayStruct->s_WindowResolutionY, atoi( (gm->s_IniFile->getValue("video", "gamescale") ).c_str() ));
            gm->s_Window->title(STRING_CONSTANTS::SC_TITLE_WINDOW);*/
            //...
            string modpack = s_ServerList->getValue(iter->first, "modpack");
            string texturepack = s_ServerList->getValue(iter->first, "texturepack");
            string monsterpack = s_ServerList->getValue(iter->first, "monsterpack");
            string bonuspack = s_ServerList->getValue(iter->first, "bonuspack");
            string screenpack = s_ServerList->getValue(iter->first, "screenpack");
            string soundpack = s_ServerList->getValue(iter->first, "soundpack");
            string musicpack = s_ServerList->getValue(iter->first, "musicpack");
            string guipack = s_ServerList->getValue(iter->first, "guipack");
            string levelpack = s_ServerList->getValue(iter->first, "levelpack");
            string playerpack = s_ServerList->getValue(iter->first, "playerpack");
            bool changeAll = false;
            if(modpack != "standard" || texturepack != "" || monsterpack != "" || bonuspack != "" || screenpack != "" || soundpack != "" || soundpack == "" || levelpack != "" || playerpack != "" || musicpack != "" || guipack != "")
            {
                if(gm->s_IniFile->getValue("resources", "pooling") == "false")
                {
                    gm->s_IniFile->setValue("resources", "pooling", "true");
                    gm->s_IniFile->setValue("resources", "modpack", "");
                    changeAll = true;
                }
            }
            if(modpack != "standard") gm->s_IniFile->setValue("resources", "modpack", modpack);
            else gm->s_IniFile->setValue("resources", "standard", "true");
            if(changeAll || texturepack != "") gm->s_IniFile->setValue("resources", "texturepack", texturepack);
            if(changeAll || monsterpack != "") gm->s_IniFile->setValue("resources", "monsterpack", monsterpack);
            if(changeAll || bonuspack != "") gm->s_IniFile->setValue("resources", "bonuspack", bonuspack);
            if(changeAll || screenpack != "") gm->s_IniFile->setValue("resources", "screenpack", screenpack);
            if(changeAll || soundpack != "") gm->s_IniFile->setValue("resources", "soundpack", soundpack);
            if(changeAll || musicpack != "") gm->s_IniFile->setValue("resources", "musicpack", musicpack);
            if(changeAll || levelpack != "") gm->s_IniFile->setValue("resources", "levelpack", levelpack);
            if(changeAll || playerpack != "") gm->s_IniFile->setValue("resources", "playerpack", playerpack);
            if(changeAll || musicpack != "") gm->s_IniFile->setValue("resources", "musicpack", musicpack);
            if(changeAll || guipack != "") gm->s_IniFile->setValue("resources", "guipack", guipack);
            //...
            if(!gm->loadPack())
            {
                cout<<"Error: Load game pack."<<endl;
                return false;
            }
            gm->startGame(id_srv);
            gm->changeLevel( atoi(s_ServerList->getValue(iter->first, "level").c_str() ) );
            gm->s_StateManager->switchState(3);
            gm->s_GameInfo->s_MyPlayer->s_CoordX = -1000;
            gm->s_GameInfo->s_MyPlayer->s_CoordY = -1000;
            gm->s_GameInfo->s_ScreenCoordX = -1*atoi( gm->s_IniFile->getValue("video", "windowresolutionX").c_str() ) - atoi( gm->s_IniFile->getValue("gamesettings", "distancelivemonstersX").c_str() ) - 1000;
            gm->s_GameInfo->s_ScreenCoordY = -1*atoi( gm->s_IniFile->getValue("video", "windowresolutionY").c_str() ) - atoi( gm->s_IniFile->getValue("gamesettings", "distancelivemonstersY").c_str() ) - 1000;
        }
    }
    return true;
}

bool MainServer::run()
{
    s_Server->run(8001);
    return true;
}
