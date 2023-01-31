#include <ctime>

#include "Game.h"

#include "IniParser/ParserInfoFile.h"
#include "IniParser/PostParsingStruct.h"

#include "WorkFunctions.h"
#include "Defines.h"

#include "NetClient/NetClient.h"

using namespace IniParser;

using namespace std;

using namespace sf;

int main(int argc, char** argv)
{
    srand((unsigned)time(NULL));
    Game* gm = new Game;

    if(!gm)
    {
        cout << "Error creating GameClass!" << endl;
        return 0;
    }
    NetClient* nc = new NetClient(gm);
    gm->s_NetClient = nc;

    gm->readConfig();

    #if defined(DEBUG) | defined(_DEBUG) | defined(NDEBUG)
        gm->s_Logger->registerEvent(EVENT_TYPE_DEBUG_INFO, "Config was read succesfully");
    #endif

    string name = "Player", pass = "", host = "127.0.0.1";
    int port = 11237;
    bool launch_launcher = false;
    if(argc == 1) launch_launcher = true;
    else if(argc == 2)
    {
        string first_par = argv[1];
        if(first_par  != "nolauncher") launch_launcher = true;
    }
    else if(argc > 2)
    {
        name = argv[1];
        pass = argv[2];
    }
    if(argc > 3) host = argv[3];
    if(argc > 4) port = atoi(argv[4]);

    if(launch_launcher)
    {
        string file_launcher = "Launcher.exe";
        system(("start " + file_launcher).c_str());
        return 0;
    }

    nc->s_NetInfoStruct->s_Name = name;
    nc->s_NetInfoStruct->s_Pass = pass;
    nc->s_NetInfoStruct->s_Host = host;
    nc->s_NetInfoStruct->s_Port = port;
    nc->s_NetInfoStruct->s_WaitingGettingCreatureList = false;

    if(nc->s_NetInfoStruct->s_Pass == "")
    {
        nc->s_NetInfoStruct->s_goGame = false;
        nc->s_NetInfoStruct->s_goGameOnServer = false;
        gm->s_Logger->registerEvent(EVENT_TYPE_INFO, "Singleplayer mode On...");
        nc->s_NetInfoStruct->s_Mode = "singleplayer";
    }
    else
    {
        nc->s_NetInfoStruct->s_goGame = true;
        nc->s_NetInfoStruct->s_goGameOnServer = true;
        gm->s_Logger->registerEvent(EVENT_TYPE_INFO, "Multiplayer mode On...");
        nc->s_NetInfoStruct->s_Mode = "multiplayer";
        gm->s_Logger->registerEvent(EVENT_TYPE_INFO, "Connecting to server ( " + string(nc->s_NetInfoStruct->s_Host) + ":" + WorkFunctions::ConvertFunctions::itos(nc->s_NetInfoStruct->s_Port) + " )...");
        if(!nc->connect()) return 0;
        gm->s_Logger->registerEvent(EVENT_TYPE_INFO, "Connected.");
    }

    do
    {
        if(nc->s_NetInfoStruct->s_Mode == "multiplayer")
            if(!nc->netGameStartWork())
                return 0;
        //...
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
        //gm->s_Window = new Window("DD2 Remake: ME v0.2 pre-beta", gm->s_DisplayStruct->s_GameResolutionX, gm->s_DisplayStruct->s_GameResolutionY, atoi( (gm->s_IniFile->getValue("video","scale") ).c_str() ));
        /*gm->s_Window = new Window(nostart);
        gm->s_Window->geometry(gm->s_DisplayStruct->s_GameResolutionX, gm->s_DisplayStruct->s_GameResolutionY, );
        gm->s_Window->title(STRING_CONSTANTS::SC_TITLE_WINDOW);*/

        /*gm->s_RenderTexture->create(gm->s_DisplayStruct->s_GameResolutionX, gm->s_DisplayStruct->s_GameResolutionY, true);
        float window_scale = atof( (gm->s_IniFile->getValue("video","scale") ).c_str() );
        gm->s_RenderWindow = new RenderWindow( VideoMode(gm->s_DisplayStruct->s_GameResolutionX * window_scale, gm->s_DisplayStruct->s_GameResolutionY * window_scale), STRING_CONSTANTS::SC_TITLE_WINDOW);
        gm->s_GameRenderScale = window_scale;*/

        if(!gm->createWindow())
        {
            gm->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Create window.", true);
            return 0;
        }

        //...
        if(!gm->loadPack())
        {
            gm->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Load game pack.", true);
            delete gm;
            system("pause");
            return 0;
        }
        gm->startGame(0);
        if(nc->s_NetInfoStruct->s_Mode == "singleplayer") gm->play();
        else if(nc->s_NetInfoStruct->s_Mode == "multiplayer")
        {
            string keySL = "";
            map<string, map<string, string > >::iterator iter__;
            for(iter__ = nc->s_NetInfoStruct->s_ServerList->getMapVariables().begin(); iter__ != nc->s_NetInfoStruct->s_ServerList->getMapVariables().end(); iter__++)
            {
                if(iter__->first.find("Server") == 0 && atoi( nc->s_NetInfoStruct->s_ServerList->getValue(iter__->first, "id").c_str() ) == nc->s_NetInfoStruct->s_ServerIdNow)
                {
                    keySL = iter__->first;
                }
            }
            gm->changeLevel(atoi( nc->s_NetInfoStruct->s_ServerList->getValue(keySL, "level").c_str() ), false);
            while(nc->s_NetInfoStruct->s_goGameOnServer)
            {
                if(nc->s_NetInfoStruct->s_WaitingGettingCreatureList == false) nc->getCreaturesList();
                if(nc->s_NetInfoStruct->s_WaitingConfirmGettingInfoFromClient == false) nc->sendInfoFromClient();
                gm->processAllEvents( atoi( nc->s_NetInfo->getValue("internet", "maxnumberofeventsatatime").c_str() ) );
                net::run( atoi( nc->s_NetInfo->getValue("internet", "sleepfornetwork").c_str() ) );
            }
            nc->leaveServer();
            gm->s_Logger->registerEvent(EVENT_TYPE_INFO, "Leaving server...");
            for(int i = 0; nc->s_NetInfoStruct->s_WaitingConfirmLeaveServer == true; i++)
            {
                net::run(1000);
                cout << ".";
                if(i == 10)
                {
                    cout << endl;
                    gm->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Server timeout.");
                    return false;
                }
            }
            cout << endl;
            gm->s_Logger->registerEvent(EVENT_TYPE_INFO, "Server leaved.");
            nc->s_NetInfoStruct->s_goGameOnServer = true;
        }
        delete gm;
        gm = new Game;
        if(!gm)
        {
            cout<<"Error creating GameClass!"<<endl;
            return 0;
        }
        gm->s_NetClient = nc;
        nc->s_GameClass = gm;
        gm->readConfig();
    } while(nc->s_NetInfoStruct->s_goGame);
    delete gm;
    delete nc;
    system("pause");
    return 0;
}
