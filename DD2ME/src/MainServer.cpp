#include "MainServer.h"

#include "IniParser/ParserInfoFile.h"
#include "IniParser/PostParsingStruct.h"

MainServer::MainServer()
{
    s_Server = new Server(this);
}

MainServer::~MainServer()
{
    //...
}

bool MainServer::load()
{
    int id_srv = 0;
    ParserInfoFile prs;
    s_ServerList = prs.getParsedFromFile("ServerData/ServerList.ini");
    if(!s_ServerList) return false;
    map<string, map<string, string > >::iterator iter;
    for(iter = s_ServerList->s_Variables.begin(); iter != s_ServerList->s_Variables.end(); iter++)
    {
        if(iter->first.find("Server") == 0)
        {
            id_srv++;
            cout<<endl<<"Server (ID = "<<id_srv<<"):"<<endl;
            Game* gm = new Game;
            s_ListGameClass[s_ServerList->getValue(iter->first, "id")] = gm;
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
            gm->s_IniFile = prsfl.getParsedFromFile("DD2.ini");
            nc->s_NetInfo = prsfl.getParsedFromFile("Launcher.ini");
            if(!gm->s_IniFile || !nc->s_NetInfo)
            {
                return false;
            }
            gm->s_DisplayStruct->s_ResolutionX = atoi( (gm->s_IniFile->getValue("video","resolutionX") ).c_str() );
            gm->s_DisplayStruct->s_ResolutionY = atoi( (gm->s_IniFile->getValue("video","resolutionY") ).c_str() );
            gm->s_GameInfo->s_KeyDown = atoi( gm->s_IniFile->getValue("keys","down").c_str() );
            gm->s_GameInfo->s_KeyUp = atoi( gm->s_IniFile->getValue("keys","up").c_str() );
            gm->s_GameInfo->s_KeyRight = atoi( gm->s_IniFile->getValue("keys","right").c_str() );
            gm->s_GameInfo->s_KeyLeft = atoi( gm->s_IniFile->getValue("keys","left").c_str() );
            gm->s_GameInfo->s_KeyShoot = atoi( gm->s_IniFile->getValue("keys","shoot").c_str() );
            gm->s_GameInfo->s_KeyJump = atoi( gm->s_IniFile->getValue("keys","jump").c_str() );
            if(gm->s_DisplayStruct->s_ResolutionY <= 0 || gm->s_DisplayStruct->s_ResolutionX <= 0)
            {
                cout<<"Error: Display resolution."<<endl;
                return false;
            }
            //gm->s_Window = new Window("DD2 Remake: ME v0.2 pre-beta", gm->s_DisplayStruct->s_ResolutionX, gm->s_DisplayStruct->s_ResolutionY, atoi( (gm->s_IniFile->getValue("video","scale") ).c_str() ));
            gm->s_Window = new Window(nostart);
            gm->s_Window->geometry(gm->s_DisplayStruct->s_ResolutionX, gm->s_DisplayStruct->s_ResolutionY, atoi( (gm->s_IniFile->getValue("video","scale") ).c_str() ));
            gm->s_Window->title("DD2 Remake: ME v0.2 pre-beta");
            //...
            string modpack = s_ServerList->getValue(iter->first, "modpack");
            string texturepack = s_ServerList->getValue(iter->first, "texturepack");
            string monsterpack = s_ServerList->getValue(iter->first, "monsterpack");
            string bonuspack = s_ServerList->getValue(iter->first, "bonuspack");
            string screenpack = s_ServerList->getValue(iter->first, "screenpack");
            string soundpack = s_ServerList->getValue(iter->first, "soundpack");
            string levelpack = s_ServerList->getValue(iter->first, "levelpack");
            string davepack = s_ServerList->getValue(iter->first, "davepack");
            gm->s_IniFile->setValue("resources", "pooling", "true");
            gm->s_IniFile->setValue("resources", "standard", "false");
            if(modpack != "") gm->s_IniFile->setValue("resources", "modpack", modpack);
            if(texturepack != "") gm->s_IniFile->setValue("resources", "texturepack", texturepack);
            if(monsterpack != "") gm->s_IniFile->setValue("resources", "monsterpack", monsterpack);
            if(bonuspack != "") gm->s_IniFile->setValue("resources", "bonuspack", bonuspack);
            if(screenpack != "") gm->s_IniFile->setValue("resources", "screenpack", screenpack);
            if(soundpack != "") gm->s_IniFile->setValue("resources", "soundpack", soundpack);
            if(levelpack != "") gm->s_IniFile->setValue("resources", "levelpack", levelpack);
            if(davepack != "") gm->s_IniFile->setValue("resources", "davepack", davepack);
            //...
            if(!gm->loadPack())
            {
                cout<<"Error: Load game pack."<<endl;
                return false;
            }
            gm->startGame(id_srv);
            s_ListTimerId[gm->s_IdTimerAnimationStep] = gm;
            s_ListTimerId[gm->s_IdTimerCreaturesAnimationStep] = gm;
            s_ListTimerId[gm->s_IdTimerDrawStep] = gm;
            gm->changeLevel( atoi(s_ServerList->getValue(iter->first, "level").c_str() ) );
            gm->s_StateManager->switchState(3);
            gm->s_GameInfo->s_MyDave->s_CoordX = -1000;
            gm->s_GameInfo->s_MyDave->s_CoordY = -1000;
            gm->s_GameInfo->s_ScreenCoordX = -1*atoi( gm->s_IniFile->getValue("video", "resolutionX").c_str() ) - atoi( gm->s_IniFile->getValue("settings", "distancelivemonstersX").c_str() ) - 1000;
            gm->s_GameInfo->s_ScreenCoordY = -1*atoi( gm->s_IniFile->getValue("video", "resolutionY").c_str() ) - atoi( gm->s_IniFile->getValue("settings", "distancelivemonstersY").c_str() ) - 1000;
        }
    }
    return true;
}

bool MainServer::run()
{
    s_Server->run(8001);
    return true;
}