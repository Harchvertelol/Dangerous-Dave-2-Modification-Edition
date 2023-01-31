#include "Game.h"

#include "WorkFunctions.h"

#include "Defines.h"

#include "IniParser/ParserInfoFile.h"

#include <ctime>

using namespace IniParser;

using namespace std;

using namespace sf;

namespace
{
    static Game* ourInst = 0;

    //static LRESULT CALLBACK MyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    static void MyWndProc(Event event)
    {
        if(ourInst->s_NetClient->s_NetInfo->getValue("settings", "keysaction") == "false") return;
        switch(event.type)
        {
            case Event::Closed:
                ourInst->s_NetClient->s_NetInfoStruct->s_goGameOnServer = false;
                ourInst->s_RenderWindow->close();
                break;

            default:
                break;
        }

        /*switch (uMsg) //рассматриваем ИМЯ сообщения
        {
            case WM_PAINT: //сообщение WM_PAINT - требуется перерисовать окно
                ourInst->s_Window->paint(); //просим Canvas перерисоваться
                break;

            case WM_DESTROY: //WM_DESTROY - пользователь нажал на крестик на окне (закрыть)
                ourInst->s_NetClient->s_NetInfoStruct->s_goGameOnServer = false;
                PostQuitMessage(0); //говорим операционной системе, что она может закрыть нашу программу
                break;
        }*/
        //if(wParam==SC_KEYMENU && (lParam>>16)<=0) return 0;
        if(!ourInst->s_RenderWindow->hasFocus()) return;
        int gameState = ourInst->s_GameInfo->s_GameState;
        /*if(gameState == 0) return ourInst->s_StateManager->s0(hwnd, uMsg, wParam, lParam);
        if(gameState == 1) return ourInst->s_StateManager->s1(hwnd, uMsg, wParam, lParam);
        if(gameState == 2) return ourInst->s_StateManager->s2(hwnd, uMsg, wParam, lParam);
        if(gameState == 3) return ourInst->s_StateManager->s3(hwnd, uMsg, wParam, lParam);*/
        if(gameState == 0) ourInst->s_StateManager->s0(event);
        if(gameState == 1) ourInst->s_StateManager->s1(event);
        if(gameState == 2) ourInst->s_StateManager->s2(event);
        if(gameState == 3) ourInst->s_StateManager->s3(event);
    }

}

Game::Game():
    s_RenderWindow(0),
    s_IniFile(0),
    s_NetClient(0),
    s_GameRenderScale(1),
    s_AnimationStep(0),
    s_TileAnimationStep(0),
    s_IdGameClass(0),
    s_IdTimerAnimationStep(0),
    s_IdTimerCreaturesAnimationStep(0),
    s_IdTimerDrawStep(0),
    s_IdTimerTilesAnimationStep(0),
    s_IdTimerAIRunStep(0),
    s_GameDrawFPS(0),
    s_GameDrawFPSMaximal(0),
    s_GameDrawFPSMinimal(MAXIMAL_UNSIGNED_NUMBER),
    s_GameDrawFPSCalc(0),
    s_TimeStartGameDrawFPS(0),
    s_TechnicalFPS(0),
    s_TechnicalFPSMaximal(0),
    s_TechnicalFPSMinimal(MAXIMAL_UNSIGNED_NUMBER),
    s_TechnicalFPSCalc(0),
    s_TimeStartTechnicalFPS(0)
{
    s_RenderTexture = new RenderTexture;
    s_GameInfo = new GameInfo(this);
    s_DisplayStruct = new DisplayStruct;
    s_Data = new GameData(this);
    s_StateManager = new StateManager(this);
    s_FactoryTmpImgs = new FactoryTemporaryImages(this);
    s_AI = new AI(this);
    s_Gui = new Gui(this);
    s_Logger = new Logger();
}

Game::~Game()
{
    if(s_GameInfo != 0) delete s_GameInfo;
    if(s_DisplayStruct != 0) delete s_DisplayStruct;
    if(s_IniFile != 0) delete s_IniFile;
    if(s_StateManager != 0) delete s_StateManager;
    if(s_AI != 0) delete s_AI;
    if(s_Gui != 0) delete s_Gui;
    if(s_FactoryTmpImgs != 0) delete s_FactoryTmpImgs;
    if(s_Data != 0) delete s_Data;
    if(s_RenderTexture != 0) delete s_RenderTexture;
    if(s_RenderWindow != 0) { s_RenderWindow->close(); delete s_RenderWindow; }
    if(s_Logger != 0) delete s_Logger;
}

void Game::deleteAllGDIObjects()
{
    s_Data->deleteAllGDIObjects();
}

void Game::calculateGameDrawFPS()
{
    calculateFPS(&s_GameDrawFPS, &s_GameDrawFPSCalc, &s_TimeStartGameDrawFPS, &s_GameDrawFPSMaximal, &s_GameDrawFPSMinimal);
}

void Game::calculateTechnicalFPS()
{
    calculateFPS(&s_TechnicalFPS, &s_TechnicalFPSCalc, &s_TimeStartTechnicalFPS, &s_TechnicalFPSMaximal, &s_TechnicalFPSMinimal);
}

void Game::calculateFPS(unsigned int* fps, unsigned int* calcfps, time_t* timeold, unsigned int* maximalfps, unsigned int* minimalfps)
{
    time_t timenew = clock();
    (*calcfps)++;
    //if(timenew - (*timeold) > 0)
    if(timenew - (*timeold) >= TIME_SECOND_FPS)
    {
        (*fps) = (*calcfps);
        //int tmp_t = timenew - (*timeold);
        //(*fps) = 1000 / tmp_t;
        (*calcfps) = 0;
        (*timeold) = timenew;
        if((*maximalfps) < (*fps)) (*maximalfps) = (*fps);
        if((*minimalfps) > (*fps)) (*minimalfps) = (*fps);
        if(s_IniFile->getValue("FPS", "ShowFPS") == "true" && s_IniFile->getValue("FPS", "ShowFPSMode") == "console")
        {
            if(s_IniFile->getValue("FPS", "ShowFPSGameDraw") == "true" ) s_Logger->registerEvent(EVENT_TYPE_INFO, "FPS game draw: " + WorkFunctions::ConvertFunctions::itos(s_GameDrawFPS));
            if(s_IniFile->getValue("FPS", "ShowFPSGameDrawMaximal") == "true" ) s_Logger->registerEvent(EVENT_TYPE_INFO, "FPS game draw maximal: " + WorkFunctions::ConvertFunctions::itos(s_GameDrawFPSMaximal));
            if(s_IniFile->getValue("FPS", "ShowFPSGameDrawMinimal") == "true" ) s_Logger->registerEvent(EVENT_TYPE_INFO, "FPS game draw minimal: " + WorkFunctions::ConvertFunctions::itos(s_GameDrawFPSMinimal));
            if(s_IniFile->getValue("FPS", "ShowFPSTechnical") == "true" ) s_Logger->registerEvent(EVENT_TYPE_INFO, "FPS technical: " + WorkFunctions::ConvertFunctions::itos(s_TechnicalFPS));
            if(s_IniFile->getValue("FPS", "ShowFPSTechnicalMaximal") == "true" ) s_Logger->registerEvent(EVENT_TYPE_INFO, "FPS technical maximal: " + WorkFunctions::ConvertFunctions::itos(s_TechnicalFPSMaximal));
            if(s_IniFile->getValue("FPS", "ShowFPSTechnicalMinimal") == "true" ) s_Logger->registerEvent(EVENT_TYPE_INFO, "FPS technical minimal: " + WorkFunctions::ConvertFunctions::itos(s_TechnicalFPSMinimal));
        }
    }
}

unsigned int Game::getGameDrawFPS()
{
    return s_GameDrawFPS;
}

unsigned int Game::getTechnicalFPS()
{
    return s_TechnicalFPS;
}

bool Game::readConfig()
{
    ParserInfoFile prsfl;
    s_IniFile = prsfl.getParsedFromFile("DD2.ini");
    s_NetClient->s_NetInfo = prsfl.getParsedFromFile("General.ini");
    if(!s_IniFile || !s_NetClient->s_NetInfo) return false;

    if(s_IniFile->getValue("loggers", "default_standard") == "true") s_Logger->setPrintStandard(true);
    else s_Logger->setPrintStandard(false);
    if(s_IniFile->getValue("loggers", "default_info") == "true") s_Logger->setPrintInfo(true);
    else s_Logger->setPrintInfo(false);
    if(s_IniFile->getValue("loggers", "default_warning") == "true") s_Logger->setPrintWarning(true);
    else s_Logger->setPrintWarning(false);
    if(s_IniFile->getValue("loggers", "default_error") == "true") s_Logger->setPrintError(true);
    else s_Logger->setPrintError(false);
    if(s_IniFile->getValue("loggers", "default_logic_violation") == "true") s_Logger->setPrintLogicViolation(true);
    else s_Logger->setPrintLogicViolation(false);
    if(s_IniFile->getValue("loggers", "debug_max_info") == "true") s_Logger->setPrintDebugInfo(true);
    else s_Logger->setPrintDebugInfo(false);
    if(s_IniFile->getValue("loggers", "default_to_console") == "true") s_Logger->setConsolePrint(true);
    else s_Logger->setConsolePrint(false);
    if(s_IniFile->getValue("loggers", "default_to_file") == "true") s_Logger->setFilePrint(true);
    else s_Logger->setFilePrint(false);
    if(s_IniFile->isExists("loggers", "logfile")) s_Logger->setLogFileName(s_IniFile->getValue("loggers", "logfile"));

    return true;
}

bool Game::loadPack()
{
    if(!s_Data->loadData(s_IniFile)) return false;
    return true;
}

bool Game::changeNextLevel(bool switchstate)
{
    if(!changeLevel(s_GameInfo->s_CurrentLevel + 1, switchstate)) return false;
    return true;
}

string Game::getNameForSaveFolder()
{
    return "Saves/" + s_GameInfo->s_MyPlayer->s_NickName;
}

string Game::getNameForSaveFile()
{
    string save_file = "";
    string modpack = s_IniFile->getValue("resources", "modpack");
    if(s_IniFile->getValue("resources", "standard") == "true") modpack = "StandardDave";
    save_file = modpack;
    if(s_IniFile->getValue("resources", "levelpack") != "" && s_IniFile->getValue("resources", "pooling") == "true") save_file = s_IniFile->getValue("resources", "levelpack");
    save_file += ".ddmesav";
    return save_file;
}

bool Game::changeLevel(int number, bool switchstate, bool playmusic)
{
    // Save
    if(!s_GameInfo->s_MyPlayer->s_Values->isEmpty())
    {
        string save_folder = getNameForSaveFolder();
        string save_file = getNameForSaveFile();
        IniParser::ParserInfoFile prs;
        prs.setCryptKey(STRING_CONSTANTS::SC_CRYPT_KEY_SAVES);
        prs.setCryptedStatus(true);
        WorkFunctions::FileFunctions::createFolders(save_folder, "/");
        prs.writeParsedToFile(s_GameInfo->s_MyPlayer->s_Values, save_folder + "/" + save_file);
    }
    //...
    tgui::Timer::clearTimers();
    if(number < 1 || number > atoi( s_Data->s_LevelsInfo->getValue("info", "numberoflevels").c_str()  ) )
    {
        s_GameInfo->s_Stop = true;
        return false;
    }
    s_GameInfo->s_CurrentLevel = number;
    s_GameInfo->s_FactoryMonsters->clear();
    if(!s_Data->s_Level->loadLevel( s_Data->PathToLevelPack + WorkFunctions::ConvertFunctions::itos(number))) return false;

    if(s_Data->s_Level->s_Params->isExists("options", "distancelivemonstersX")) s_GameInfo->s_CurrentDistanceLiveMonstersX = atoi(s_Data->s_Level->s_Params->getValue("options", "distancelivemonstersX").c_str());
    else if(s_Data->s_LevelsInfo->isExists("options", "distancelivemonstersX")) s_GameInfo->s_CurrentDistanceLiveMonstersX = atoi(s_Data->s_LevelsInfo->getValue("options", "distancelivemonstersX").c_str());
    else s_GameInfo->s_CurrentDistanceLiveMonstersX = atoi( s_IniFile->getValue("settings", "distancelivemonstersX").c_str() );
    if(s_Data->s_Level->s_Params->isExists("options", "distancelivemonstersY")) s_GameInfo->s_CurrentDistanceLiveMonstersY = atoi(s_Data->s_Level->s_Params->getValue("options", "distancelivemonstersY").c_str());
    else if(s_Data->s_LevelsInfo->isExists("options", "distancelivemonstersY")) s_GameInfo->s_CurrentDistanceLiveMonstersY = atoi(s_Data->s_LevelsInfo->getValue("options", "distancelivemonstersY").c_str());
    else s_GameInfo->s_CurrentDistanceLiveMonstersY = atoi( s_IniFile->getValue("settings", "distancelivemonstersY").c_str() );

    if(s_Data->s_Level->s_Params->isExists("options", "distanceactivatemonstersX")) s_GameInfo->s_CurrentDistanceActivateMonstersX = atoi(s_Data->s_Level->s_Params->getValue("options", "distanceactivatemonstersX").c_str());
    else if(s_Data->s_LevelsInfo->isExists("options", "distanceactivatemonstersX")) s_GameInfo->s_CurrentDistanceActivateMonstersX = atoi(s_Data->s_LevelsInfo->getValue("options", "distanceactivatemonstersX").c_str());
    else s_GameInfo->s_CurrentDistanceActivateMonstersX = atoi( s_IniFile->getValue("settings", "distanceactivatemonstersX").c_str() );
    if(s_Data->s_Level->s_Params->isExists("options", "distanceactivatemonstersY")) s_GameInfo->s_CurrentDistanceActivateMonstersY = atoi(s_Data->s_Level->s_Params->getValue("options", "distanceactivatemonstersY").c_str());
    else if(s_Data->s_LevelsInfo->isExists("options", "distanceactivatemonstersY")) s_GameInfo->s_CurrentDistanceActivateMonstersY = atoi(s_Data->s_LevelsInfo->getValue("options", "distanceactivatemonstersY").c_str());
    else s_GameInfo->s_CurrentDistanceActivateMonstersY = atoi( s_IniFile->getValue("settings", "distanceactivatemonstersY").c_str() );

    //...
    map<int, CreaturePlayer*>::iterator iter;
    for ( iter = s_GameInfo->s_Players.begin(); iter != s_GameInfo->s_Players.end(); iter++)
    {
        iter->second->s_State = "rightstand";
        iter->second->s_ShootNow = 0;
        iter->second->s_Cartridges = iter->second->s_MaxCartridges;
        iter->second->s_ScreenCoordX = 0;
        iter->second->s_ScreenCoordY = 0;
    }
    //...
    s_GameInfo->s_MyPlayer->s_State = "rightstand";
    s_FactoryTmpImgs->clear();
    s_GameInfo->s_MyPlayer->s_ShootNow = 0;
    s_GameInfo->s_ForcedChangeLevel = false;
    s_GameInfo->s_Stop = false;
    s_GameInfo->s_MyPlayer->s_Cartridges = s_GameInfo->s_MyPlayer->s_MaxCartridges;
    s_GameInfo->s_MyPlayer->s_ScreenCoordX = 0;
    s_GameInfo->s_MyPlayer->s_ScreenCoordY = 0;
    if(s_GameInfo->s_MyPlayer->s_KeysState) delete s_GameInfo->s_MyPlayer->s_KeysState;
    s_GameInfo->s_MyPlayer->s_KeysState = new KeysState;
    s_Data->s_Music->stopAllMusic();
    if(switchstate == true) s_StateManager->switchState(2, false);
    if(playmusic)
    {
        if(s_GameInfo->s_CurrentLevel == 1) s_Data->s_Sounds->play("before_first_level");
        else s_Data->s_Sounds->play("changelevel");
    }
    return true;
}

void Game::startGame(int idgameclass)
{
    s_IdGameClass = idgameclass;
    if(s_IniFile->getValue("video", "usetilecache") == "true") s_Data->s_Textures->createCache();
    if(s_IniFile->getValue("video", "usebonuscache") == "true") s_Data->s_Bonuses->createCache();
    if(s_IniFile->getValue("video", "usescreencache") == "true") s_Data->s_Screens->createCache();
    if(s_IniFile->getValue("video", "useplayercache") == "true") s_Data->s_Player->createCache();
    if(s_IniFile->getValue("video", "usemonstercache") == "true") s_Data->s_Monsters->createCache();
    if(s_IniFile->getValue("video", "usebackgroundscache") == "true") s_Data->s_Backgrounds->createCache();
    if(s_IniFile->getValue("video", "usetransparentmask") == "true")
    {
        if(s_Data->s_Textures->s_TilesInfo->getValue("draw", "transparentmaskR") != "-1" && s_Data->s_Textures->s_TilesInfo->getValue("draw", "transparentmaskG") != "-1" && s_Data->s_Textures->s_TilesInfo->getValue("draw", "transparentmaskB") != "-1") s_Data->s_Textures->createMaskTransparent(atoi(s_Data->s_Textures->s_TilesInfo->getValue("draw", "transparentmaskR").c_str()), atoi(s_Data->s_Textures->s_TilesInfo->getValue("draw", "transparentmaskG").c_str()), atoi(s_Data->s_Textures->s_TilesInfo->getValue("draw", "transparentmaskB").c_str()));
        if(s_Data->s_Bonuses->s_GlobBonusesInfo->getValue("draw", "transparentmaskR") != "-1" && s_Data->s_Bonuses->s_GlobBonusesInfo->getValue("draw", "transparentmaskG") != "-1" && s_Data->s_Bonuses->s_GlobBonusesInfo->getValue("draw", "transparentmaskB") != "-1") s_Data->s_Bonuses->createMaskTransparent(atoi(s_Data->s_Bonuses->s_GlobBonusesInfo->getValue("draw", "transparentmaskR").c_str()), atoi(s_Data->s_Bonuses->s_GlobBonusesInfo->getValue("draw", "transparentmaskG").c_str()), atoi(s_Data->s_Bonuses->s_GlobBonusesInfo->getValue("draw", "transparentmaskB").c_str()));
        if(s_Data->s_Screens->s_ScreensInfo->getValue("draw", "transparentmaskR") != "-1" && s_Data->s_Screens->s_ScreensInfo->getValue("draw", "transparentmaskG") != "-1" && s_Data->s_Screens->s_ScreensInfo->getValue("draw", "transparentmaskB") != "-1") s_Data->s_Screens->createMaskTransparent(atoi(s_Data->s_Screens->s_ScreensInfo->getValue("draw", "transparentmaskR").c_str()), atoi(s_Data->s_Screens->s_ScreensInfo->getValue("draw", "transparentmaskG").c_str()), atoi(s_Data->s_Screens->s_ScreensInfo->getValue("draw", "transparentmaskB").c_str()));
        if(s_Data->s_Player->s_PlayerInfo->getValue("draw", "transparentmaskR") != "-1" && s_Data->s_Player->s_PlayerInfo->getValue("draw", "transparentmaskG") != "-1" && s_Data->s_Player->s_PlayerInfo->getValue("draw", "transparentmaskB") != "-1") s_Data->s_Player->createMaskTransparent(atoi(s_Data->s_Player->s_PlayerInfo->getValue("draw", "transparentmaskR").c_str()), atoi(s_Data->s_Player->s_PlayerInfo->getValue("draw", "transparentmaskG").c_str()), atoi(s_Data->s_Player->s_PlayerInfo->getValue("draw", "transparentmaskB").c_str()));
        if(s_Data->s_Monsters->s_GlobMonstersInfo->getValue("draw", "transparentmaskR") != "-1" && s_Data->s_Monsters->s_GlobMonstersInfo->getValue("draw", "transparentmaskG") != "-1" && s_Data->s_Monsters->s_GlobMonstersInfo->getValue("draw", "transparentmaskB") != "-1") s_Data->s_Monsters->createMaskTransparent(atoi(s_Data->s_Monsters->s_GlobMonstersInfo->getValue("draw", "transparentmaskR").c_str()), atoi(s_Data->s_Monsters->s_GlobMonstersInfo->getValue("draw", "transparentmaskG").c_str()), atoi(s_Data->s_Monsters->s_GlobMonstersInfo->getValue("draw", "transparentmaskB").c_str()));
        if(s_Data->s_Backgrounds->s_BackgroundsInfo->getValue("draw", "transparentmaskR") != "-1" && s_Data->s_Backgrounds->s_BackgroundsInfo->getValue("draw", "transparentmaskG") != "-1" && s_Data->s_Backgrounds->s_BackgroundsInfo->getValue("draw", "transparentmaskB") != "-1") s_Data->s_Backgrounds->createMaskTransparent(atoi(s_Data->s_Backgrounds->s_BackgroundsInfo->getValue("draw", "transparentmaskR").c_str()), atoi(s_Data->s_Backgrounds->s_BackgroundsInfo->getValue("draw", "transparentmaskG").c_str()), atoi(s_Data->s_Backgrounds->s_BackgroundsInfo->getValue("draw", "transparentmaskB").c_str()));
    }
    s_StateManager->switchState(0);
    configureForGame();
}

bool Game::createWindow()
{
    if(s_RenderWindow)
    {
        s_RenderWindow->close();
        delete s_RenderWindow;
    }
    float window_scale = atof( (s_IniFile->getValue("video", "gamescale") ).c_str() );
    if( (s_DisplayStruct->s_WindowResolutionX == -1 || s_DisplayStruct->s_WindowResolutionY == -1) && window_scale == -1 )
    {
        s_Logger->registerEvent(EVENT_TYPE_LOGIC_VIOLATION, "(window resolution x, window resolution y) and gamescale cannot be equal to -1 at the same time!", true);
        return false;
    }
    s_RenderTexture->create(s_DisplayStruct->s_GameResolutionX, s_DisplayStruct->s_GameResolutionY, true);
    if(s_DisplayStruct->s_WindowResolutionX == -1 || s_DisplayStruct->s_WindowResolutionY == -1)
    {
        s_DisplayStruct->s_WindowResolutionX = s_DisplayStruct->s_GameResolutionX * window_scale;
        s_DisplayStruct->s_WindowResolutionY = s_DisplayStruct->s_GameResolutionY * window_scale;
    }
    if(window_scale == -1)
    {
        float tmp_window_scale = (float)s_DisplayStruct->s_WindowResolutionX / (float)s_DisplayStruct->s_GameResolutionX;
        window_scale = (float)s_DisplayStruct->s_WindowResolutionY / (float)s_DisplayStruct->s_GameResolutionY;
        if(window_scale - tmp_window_scale > NUMBER_CONSTANTS::NC_EPS) window_scale = tmp_window_scale;
    }
    if(s_IniFile->getValue("video", "fullscreen") == "true")
    {
        s_RenderWindow = new RenderWindow( VideoMode(s_DisplayStruct->s_WindowResolutionX, s_DisplayStruct->s_WindowResolutionY), STRING_CONSTANTS::SC_TITLE_WINDOW, Style::Fullscreen);
        //s_RenderWindow = new RenderWindow( VideoMode::getFullscreenModes()[0], STRING_CONSTANTS::SC_TITLE_WINDOW, Style::Fullscreen);
    }
    else s_RenderWindow = new RenderWindow( VideoMode(s_DisplayStruct->s_WindowResolutionX, s_DisplayStruct->s_WindowResolutionY), STRING_CONSTANTS::SC_TITLE_WINDOW);
    s_GameRenderScale = window_scale;
    return true;
}

void Game::configureForGame()
{
    ourInst = this;
    if(s_NetClient->s_NetInfo->getValue("video", "graphicmodegame") == "true")
    {
        //s_Window->start(MyWndProc);
        //s_Window->enable_frame_buffer();
    }
    else deleteAllGDIObjects();

    drawAll();

    //SetTimer(s_Window->hwnd(), 6, atoi( s_IniFile->getValue("video", "animationstep").c_str() ), 0);
    //SetTimer(s_Window->hwnd(), 5, atoi( s_IniFile->getValue("video", "creaturesanimationstep").c_str() ), 0);
    //SetTimer(s_Window->hwnd(), 7, atoi( s_IniFile->getValue("video", "drawstep").c_str() ), 0);
    //это таймер с ID = 7, тикающий каждые 100 миллисекунд. по идее wparam будет равен 7, если событие - таймер.

    /*s_IdTimerAnimationStep = set_timer(atoi( s_IniFile->getValue("video", "animationstep").c_str() ));
    s_IdTimerCreaturesAnimationStep = set_timer(atoi( s_IniFile->getValue("video", "creaturesanimationstep").c_str() ));
    s_IdTimerDrawStep = set_timer(atoi( s_IniFile->getValue("video", "drawstep").c_str() ));
    s_IdTimerTilesAnimationStep = set_timer(atoi( s_IniFile->getValue("video", "tilesanimationstep").c_str() ));
    s_IdTimerAIRunStep = set_timer(atoi( s_IniFile->getValue("video", "AIrunstep").c_str() ));*/

    s_IdTimerAnimationStep = s_Timers.setTimer(atoi( s_IniFile->getValue("video", "animationstep").c_str() ));
    s_IdTimerCreaturesAnimationStep = s_Timers.setTimer(atoi( s_IniFile->getValue("video", "creaturesanimationstep").c_str() ));
    s_IdTimerDrawStep = s_Timers.setTimer(atoi( s_IniFile->getValue("video", "drawstep").c_str() ));
    s_IdTimerTilesAnimationStep = s_Timers.setTimer(atoi( s_IniFile->getValue("video", "tilesanimationstep").c_str() ));
    s_IdTimerAIRunStep = s_Timers.setTimer(atoi( s_IniFile->getValue("video", "AIrunstep").c_str() ));
}

void Game::play()
{
    /*const int id1 = set_timer(500);
    const int id2 = set_timer(1000);
    Message m;
    while(m.wait())
    {
        if(m.is_timer()) cout << m.timer_id() << endl; // можно сравнить с id1, id2
    }*/

    while(s_RenderWindow->isOpen())
    {
        Event event;
        while(s_RenderWindow->pollEvent(event))
        {
            calculateTechnicalFPS();
            MyWndProc(event);
        }
        vector<int> timers_ticks = s_Timers.getTimersTicks();
        for(int i = 0; i < timers_ticks.size(); i++) onTimer(timers_ticks[i]);
        s_Gui->s_TGUI->handleEvent(event);
    }

    /*MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        calculateTechnicalFPS();
        if(msg.message == WM_TIMER) onTimer(msg.wParam);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }*/
}

void Game::processAllEvents(int maxEvents)
{
    vector<int> timers_ticks = s_Timers.getTimersTicks();
    for(int i = 0; i < timers_ticks.size(); i++) onTimer(timers_ticks[i]);
    /*Message m;
    for(int i = 0; i < maxEvents && m.peek() == true; i++)
    {
        if(m.is_timer())
        {
            onTimer(m.timer_id());
        }
    }*/
}

void Game::onTimer(unsigned int timer_id)
{
    if(timer_id == s_IdTimerDrawStep)
    {
        #if defined(DEBUG) | defined(_DEBUG) | defined(NDEBUG)
            s_Logger->registerEvent(EVENT_TYPE_DEBUG_INFO, "Timer Draw Step was ticked.");
        #endif
        s_Data->s_Sounds->clearInactiveTempSounds();
        calculateGameDrawFPS();
        //if(s_Window != 0 && GetForegroundWindow() == s_Window->hwnd() )
        if(s_RenderWindow != 0 && s_RenderWindow->hasFocus() )
        {
            if(s_GameInfo->s_GameState == 0 && s_GameInfo->s_Stop == false) s_StateManager->s0I();
            if(s_GameInfo->s_GameState == 1 && s_GameInfo->s_Stop == false) s_StateManager->s1I();
            if(s_GameInfo->s_GameState == 2 && s_GameInfo->s_Stop == false) s_StateManager->s2I();
            if(s_GameInfo->s_GameState == 3 && s_GameInfo->s_Stop == false) s_StateManager->s3I();
        }
        #if defined(DEBUG) | defined(_DEBUG) | defined(NDEBUG)
            s_Logger->registerEvent(EVENT_TYPE_DEBUG_INFO, "State manager was ended work.");
        #endif
        s_StateManager->doState(s_GameInfo->s_GameState);
        #if defined(DEBUG) | defined(_DEBUG) | defined(NDEBUG)
            s_Logger->registerEvent(EVENT_TYPE_DEBUG_INFO, "State Do function was ended.");
        #endif
        drawAll();
    }
    if(timer_id == s_IdTimerAnimationStep)
    {
        if(s_IniFile->getValue("video", "animation") == "true")
        {
            #if defined(DEBUG) | defined(_DEBUG) | defined(NDEBUG)
                s_Logger->registerEvent(EVENT_TYPE_DEBUG_INFO, "Timer Animation Step was ticked.");
            #endif
            s_AnimationStep++;
            if(s_AnimationStep > MAXIMAL_UNSIGNED_NUMBER) s_AnimationStep = 0;
        }
    }
    if(timer_id == s_IdTimerCreaturesAnimationStep)
    {
        if(s_IniFile->getValue("video", "animation") == "true")
        {
            #if defined(DEBUG) | defined(_DEBUG) | defined(NDEBUG)
                s_Logger->registerEvent(EVENT_TYPE_DEBUG_INFO, "Timer Creature Animation Step was ticked.");
            #endif
            //...
            map<int, CreaturePlayer*>::iterator iter;
            for ( iter = s_GameInfo->s_Players.begin(); iter != s_GameInfo->s_Players.end(); iter++)
            {
                iter->second->s_NumberOfAction++;
                if(iter->second->s_State == "doorexit" || iter->second->s_State == "recharge") iter->second->s_NumberOfAction--;
                if(iter->second->s_NumberOfAction > MAXIMAL_UNSIGNED_NUMBER) iter->second->s_NumberOfAction = 0;
            }
            //...
            s_GameInfo->s_MyPlayer->s_NumberOfAction++;
            if(s_GameInfo->s_MyPlayer->s_State == "doorexit" || s_GameInfo->s_MyPlayer->s_State == "recharge") s_GameInfo->s_MyPlayer->s_NumberOfAction--;
            if(s_GameInfo->s_MyPlayer->s_NumberOfAction > MAXIMAL_UNSIGNED_NUMBER) s_GameInfo->s_MyPlayer->s_NumberOfAction = 0;
        }
    }
    if(timer_id == s_IdTimerTilesAnimationStep)
    {
        #if defined(DEBUG) | defined(_DEBUG) | defined(NDEBUG)
            s_Logger->registerEvent(EVENT_TYPE_DEBUG_INFO, "Timer Tiles Animation Step was ticked.");
        #endif
        s_TileAnimationStep++;
        if(s_TileAnimationStep > MAXIMAL_UNSIGNED_NUMBER) s_TileAnimationStep = 0;
    }
    if(timer_id == s_IdTimerAIRunStep)
    {
        if(s_GameInfo->s_GameState == 3 && s_GameInfo->s_Stop == false)
        {
            #if defined(DEBUG) | defined(_DEBUG) | defined(NDEBUG)
                s_Logger->registerEvent(EVENT_TYPE_DEBUG_INFO, "Timer AI Run Step was ticked.");
            #endif
            s_GameInfo->live();
            #if defined(DEBUG) | defined(_DEBUG) | defined(NDEBUG)
                s_Logger->registerEvent(EVENT_TYPE_DEBUG_INFO, "Game live tick was ended.");
            #endif
        }
    }
}

void Game::correctionAllScreens()
{
    if(s_NetClient->s_NetInfo->getValue("program", "mode") == "client")
    {
        s_GameInfo->correctionScreen(s_GameInfo->s_MyPlayer);
        s_GameInfo->s_ScreenCoordX = s_GameInfo->s_MyPlayer->s_ScreenCoordX;
        s_GameInfo->s_ScreenCoordY = s_GameInfo->s_MyPlayer->s_ScreenCoordY;
    }
    map<int, CreaturePlayer*>::iterator iter;
    for ( iter = s_GameInfo->s_Players.begin(); iter != s_GameInfo->s_Players.end(); iter++)
    {
        s_GameInfo->correctionScreen(iter->second);
    }
}

void Game::drawAll()
{
    if(s_GameInfo->s_GameState == 3)
    {
        correctionAllScreens();
    }
    if(s_NetClient->s_NetInfo->getValue("video", "graphicmodegame") == "false") return;
    if(!s_RenderWindow->isOpen()) return;
    //s_Window->clear();
    s_RenderTexture->clear(Color(0, 0, 0, 255));
    if(s_GameInfo->s_GameState == 0) s_Data->drawScreenState0();
    if(s_GameInfo->s_GameState == 1) s_Data->drawScreenState1();
    if(s_GameInfo->s_GameState == 2) s_Data->drawScreenState2();
    if(s_GameInfo->s_GameState == 3)
    {
        s_Data->s_Level->draw(true);
        s_FactoryTmpImgs->drawAll(false);
        s_GameInfo->s_FactoryMonsters->drawAll();
        map<int, CreaturePlayer*>::iterator iter;
        for ( iter = s_GameInfo->s_Players.begin(); iter != s_GameInfo->s_Players.end(); iter++)
        {
            iter->second->draw();
        }
        s_GameInfo->s_MyPlayer->draw();
        s_Data->s_Level->draw(false);
        s_Data->drawScreenState3();
        s_FactoryTmpImgs->drawAll(true);
        if(s_GameInfo->s_Stop == true && s_GameInfo->s_DeathType != 0) s_GameInfo->playDeath();
    }
    s_RenderTexture->display();
    const Texture& txture = s_RenderTexture->getTexture();
    Sprite renderSprite(txture);
    renderSprite.setScale(s_GameRenderScale, s_GameRenderScale);
    if(s_IniFile->getValue("video", "center") == "true") renderSprite.setPosition(Vector2f(s_DisplayStruct->s_WindowResolutionX / 2 - s_DisplayStruct->s_GameResolutionX / 2 * s_GameRenderScale, s_DisplayStruct->s_WindowResolutionY / 2 - s_DisplayStruct->s_GameResolutionY / 2 * s_GameRenderScale));
    else renderSprite.setPosition(Vector2f(0, 0));
    map<int, int> tmp_mas;
    int res_col = WorkFunctions::ParserFunctions::splitMass(&tmp_mas, 0, 0, s_Data->s_GuiData->s_GuiInfo->getValue("gui", "fillcolorbackgroundwindow"), ";");
    if(res_col < 3)
    {
        s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error with gui parameters for popup window: fillcolorbackgroundwindow");
        s_RenderWindow->clear();
    }
    else s_RenderWindow->clear(Color(tmp_mas[0], tmp_mas[1], tmp_mas[2], tmp_mas[3]));
    s_RenderWindow->draw(renderSprite);
    s_Gui->s_TGUI->setTarget(*s_RenderWindow);
    s_Gui->s_TGUI->draw();
    s_RenderWindow->display();
}

PostParsingStruct* Game::getObjects()
{
    PostParsingStruct* cpps = new PostParsingStruct;
    unsigned int monsterid = 0;
    unsigned int playerid = 0;
    map<int, CreatureMonster*>::iterator iter;
    for( iter = s_GameInfo->s_FactoryMonsters->s_Monsters.begin(); iter != s_GameInfo->s_FactoryMonsters->s_Monsters.end(); iter++)
        if(iter->second->s_DeleteNow == false)
        {
            monsterid++;
            cpps->setValue("monster_" + WorkFunctions::ConvertFunctions::itos(monsterid), "coordX", WorkFunctions::ConvertFunctions::itos(iter->second->s_CoordX) );
            cpps->setValue("monster_" + WorkFunctions::ConvertFunctions::itos(monsterid), "coordY", WorkFunctions::ConvertFunctions::itos(iter->second->s_CoordY) );
            cpps->setValue("monster_" + WorkFunctions::ConvertFunctions::itos(monsterid), "currentLives", WorkFunctions::ConvertFunctions::itos(iter->second->s_CurrentLives) );
            cpps->setValue("monster_" + WorkFunctions::ConvertFunctions::itos(monsterid), "state", iter->second->s_State);
            cpps->setValue("monster_" + WorkFunctions::ConvertFunctions::itos(monsterid), "number", WorkFunctions::ConvertFunctions::itos(iter->second->s_Number) );
            cpps->setValue("monster_" + WorkFunctions::ConvertFunctions::itos(monsterid), "numberOfAction", WorkFunctions::ConvertFunctions::itos(iter->second->s_NumberOfAction) );
            cpps->setValue("monster_" + WorkFunctions::ConvertFunctions::itos(monsterid), "dopNumberOfAction", WorkFunctions::ConvertFunctions::itos(iter->second->s_AdditionalNumberOfAction) );
            cpps->setValue("monster_" + WorkFunctions::ConvertFunctions::itos(monsterid), "id", WorkFunctions::ConvertFunctions::itos(monsterid) );
            map<string, string>::iterator iter1;
            for( iter1 = iter->second->s_AIMonsterValues.begin(); iter1 != iter->second->s_AIMonsterValues.end(); iter1++)
            {
                cpps->setValue("AIMonsterValues_monster_" + WorkFunctions::ConvertFunctions::itos(monsterid), iter1->first, iter1->second );
            }
        }
    map<string, string>::iterator iter1m;
    /*for( iter1m = s_GameInfo->s_FactoryMonsters->s_AIMonstersValues.begin(); iter1m != s_GameInfo->s_FactoryMonsters->s_AIMonstersValues.end(); iter1m++)
    {
        cpps->setValue("AIMonstersValues", iter1m->first, iter1m->second );
    }*/
    map<int, CreaturePlayer*>::iterator iter1;
    for( iter1 = s_GameInfo->s_Players.begin(); iter1 != s_GameInfo->s_Players.end(); iter1++)
    {
        //playerid++;
        playerid = iter1->first;
        PostParsingStruct* gdpps = iter1->second->getListOfVariables("player_" + WorkFunctions::ConvertFunctions::itos(playerid));
        cpps->addPostParsingStruct(gdpps);
        delete gdpps;

        cpps->setValue("player_" + WorkFunctions::ConvertFunctions::itos(playerid), "id", WorkFunctions::ConvertFunctions::itos(playerid) );
        //...
        cpps->setValue("KeysState_player_" + WorkFunctions::ConvertFunctions::itos(playerid), "keyLeft", WorkFunctions::ConvertFunctions::itos( (int)iter1->second->s_KeysState->s_KeyLeft) );
        cpps->setValue("KeysState_player_" + WorkFunctions::ConvertFunctions::itos(playerid), "keyRight", WorkFunctions::ConvertFunctions::itos( (int)iter1->second->s_KeysState->s_KeyRight) );
        cpps->setValue("KeysState_player_" + WorkFunctions::ConvertFunctions::itos(playerid), "keyUp", WorkFunctions::ConvertFunctions::itos( (int)iter1->second->s_KeysState->s_KeyUp) );
        cpps->setValue("KeysState_player_" + WorkFunctions::ConvertFunctions::itos(playerid), "keyDown", WorkFunctions::ConvertFunctions::itos( (int)iter1->second->s_KeysState->s_KeyDown) );
        cpps->setValue("KeysState_player_" + WorkFunctions::ConvertFunctions::itos(playerid), "keyShoot", WorkFunctions::ConvertFunctions::itos( (int)iter1->second->s_KeysState->s_KeyShoot) );
        cpps->setValue("KeysState_player_" + WorkFunctions::ConvertFunctions::itos(playerid), "keyJump", WorkFunctions::ConvertFunctions::itos( (int)iter1->second->s_KeysState->s_KeyJump) );
    }
    int size_x_level = atoi( s_Data->s_Level->s_Params->getValue("info", "sizeX").c_str() );
    int size_y_level = atoi( s_Data->s_Level->s_Params->getValue("info", "sizeY").c_str() );
    for(int i = 0; i < size_x_level*size_y_level; i++)
    {
        if(s_Data->s_Level->s_Fields[STRING_CONSTANTS::NAME_FIELD_BONUSES][i] != 0)
        {
            cpps->setValue("bonuse_" + WorkFunctions::ConvertFunctions::itos(i), "i", WorkFunctions::ConvertFunctions::itos(s_Data->s_Level->s_Fields[STRING_CONSTANTS::NAME_FIELD_BONUSES][i]));
            cpps->setValue("bonuse_" + WorkFunctions::ConvertFunctions::itos(i), "id", WorkFunctions::ConvertFunctions::itos(i) );
        }
        if(s_Data->s_Level->s_Fields[STRING_CONSTANTS::NAME_FIELD_BONUSDOORS][i] != 0)
        {
            cpps->setValue("bonusdoor_" + WorkFunctions::ConvertFunctions::itos(i), "i", WorkFunctions::ConvertFunctions::itos(s_Data->s_Level->s_Fields[STRING_CONSTANTS::NAME_FIELD_BONUSDOORS][i]));
            cpps->setValue("bonusdoor_" + WorkFunctions::ConvertFunctions::itos(i), "id", WorkFunctions::ConvertFunctions::itos(i) );
        }
        if(s_Data->s_Level->s_Fields[STRING_CONSTANTS::NAME_FIELD_DOORS][i] != 0)
        {
            cpps->setValue("door_" + WorkFunctions::ConvertFunctions::itos(i), "i", WorkFunctions::ConvertFunctions::itos(s_Data->s_Level->s_Fields[STRING_CONSTANTS::NAME_FIELD_DOORS][i]));
            cpps->setValue("door_" + WorkFunctions::ConvertFunctions::itos(i), "id", WorkFunctions::ConvertFunctions::itos(i) );
        }
        if(s_Data->s_Level->s_Fields[STRING_CONSTANTS::NAME_FIELD_EXITLEVELDOORS][i] != 0)
        {
            cpps->setValue("exitleveldoor_" + WorkFunctions::ConvertFunctions::itos(i), "i", WorkFunctions::ConvertFunctions::itos(s_Data->s_Level->s_Fields[STRING_CONSTANTS::NAME_FIELD_EXITLEVELDOORS][i]));
            cpps->setValue("exitleveldoor_" + WorkFunctions::ConvertFunctions::itos(i), "id", WorkFunctions::ConvertFunctions::itos(i) );
        }
    }
    return cpps;
}

void Game::setObjects(PostParsingStruct* cpps)
{
    int keymonster = 0, playerkey = 0;
    int coordX, coordY, currentLives, number, numberOfAction, dopNumberOfAction;
    string state;
    s_GameInfo->s_FactoryMonsters->clear();
    map<int, CreaturePlayer*>::iterator iter_, iter2_;
    for (iter_ = s_GameInfo->s_Players.begin(), iter2_ = s_GameInfo->s_Players.end(); iter_ != iter2_;)
    {
        if(iter_->second != 0)
        {
            if(!cpps->isExists("player_" + WorkFunctions::ConvertFunctions::itos(iter_->first)))
            {
                delete iter_->second;
                s_GameInfo->s_Players.erase(iter_++);
            }
            else ++iter_;
        }
        else ++iter_;
    }
    int size_x_level = atoi( s_Data->s_Level->s_Params->getValue("info", "sizeX").c_str() );
    int size_y_level = atoi( s_Data->s_Level->s_Params->getValue("info", "sizeY").c_str() );
    for(int i = 0; i < size_x_level*size_y_level; i++)
    {
        s_Data->s_Level->s_Fields[STRING_CONSTANTS::NAME_FIELD_BONUSES][i] = 0;
        s_Data->s_Level->s_Fields[STRING_CONSTANTS::NAME_FIELD_BONUSDOORS][i] = 0;
        s_Data->s_Level->s_Fields[STRING_CONSTANTS::NAME_FIELD_DOORS][i] = 0;
        s_Data->s_Level->s_Fields[STRING_CONSTANTS::NAME_FIELD_EXITLEVELDOORS][i] = 0;
    }
    string monsterid = "";
    map<string, map<string, string> >::iterator iter;
    for( iter = cpps->getMapVariables().begin(); iter != cpps->getMapVariables().end(); iter++)
        if(iter->first.find("monster_") == 0)
        {
            monsterid = cpps->getValue(iter->first, "id");
            coordX = atoi( cpps->getValue("monster_" + monsterid, "coordX").c_str() );
            coordY = atoi( cpps->getValue("monster_" + monsterid, "coordY").c_str() );
            number = atoi( cpps->getValue("monster_" + monsterid, "number").c_str() );
            currentLives = atoi( cpps->getValue("monster_" + monsterid, "currentLives").c_str() );
            state = cpps->getValue("monster_" + monsterid, "state");
            numberOfAction = atoi( cpps->getValue("monster_" + monsterid, "numberOfAction").c_str() );
            dopNumberOfAction = atoi( cpps->getValue("monster_" + monsterid, "dopNumberOfAction").c_str() );
            keymonster = s_GameInfo->s_FactoryMonsters->addMonsterImmediately(number, coordX, coordY, false);
            s_GameInfo->s_FactoryMonsters->s_Monsters[keymonster]->s_CurrentLives = currentLives;
            s_GameInfo->s_FactoryMonsters->s_Monsters[keymonster]->s_State = state;
            s_GameInfo->s_FactoryMonsters->s_Monsters[keymonster]->s_NumberOfAction = numberOfAction;
            s_GameInfo->s_FactoryMonsters->s_Monsters[keymonster]->s_AdditionalNumberOfAction = dopNumberOfAction;
            map<string, string>::iterator iter1;
            for( iter1 = cpps->getMapVariables()["AIMonsterValues_monster_" + monsterid].begin(); iter1 != cpps->getMapVariables()["AIMonsterValues_monster_" + monsterid].end(); iter1++)
            {
                s_GameInfo->s_FactoryMonsters->s_Monsters[keymonster]->s_AIMonsterValues[iter1->first] = iter1->second;
            }
        }
        else if(iter->first.find("AIMonstersValues") == 0)
        {
            map<string, string>::iterator iter1;
            /*for( iter1 = cpps->getMapVariables()["AIMonstersValues"].begin(); iter1 != cpps->getMapVariables()["AIMonstersValues"].end(); iter1++)
            {
                s_GameInfo->s_FactoryMonsters->s_AIMonstersValues[iter1->first] = iter1->second;
            }*/
        }
        else if(iter->first.find("player_") == 0)
        {
            //playerkey++;
            string playerid = cpps->getValue(iter->first, "id");
            playerkey = atoi( playerid.c_str() );
            if(!s_GameInfo->s_Players[playerkey]) s_GameInfo->s_Players[playerkey] = new CreaturePlayer(this);
            s_GameInfo->s_Players[playerkey]->setListOfVariables(cpps, "player_" + playerid);
            //...
            s_GameInfo->s_Players[playerkey]->s_KeysState->s_KeyLeft = (bool)atoi( cpps->getValue("KeysState_player_" + playerid, "keyLeft").c_str() );
            s_GameInfo->s_Players[playerkey]->s_KeysState->s_KeyRight = (bool)atoi( cpps->getValue("KeysState_player_" + playerid, "keyRight").c_str() );
            s_GameInfo->s_Players[playerkey]->s_KeysState->s_KeyUp = (bool)atoi( cpps->getValue("KeysState_player_" + playerid, "keyUp").c_str() );
            s_GameInfo->s_Players[playerkey]->s_KeysState->s_KeyDown = (bool)atoi( cpps->getValue("KeysState_player_" + playerid, "keyDown").c_str() );
            s_GameInfo->s_Players[playerkey]->s_KeysState->s_KeyShoot = (bool)atoi( cpps->getValue("KeysState_player_" + playerid, "keyShoot").c_str() );
            s_GameInfo->s_Players[playerkey]->s_KeysState->s_KeyJump = (bool)atoi( cpps->getValue("KeysState_player_" + playerid, "keyJump").c_str() );
        }
        else if(iter->first.find("bonuse_") == 0)
        {
            string bonuseid = cpps->getValue(iter->first, "id");
            int key = atoi( bonuseid.c_str() );
            s_Data->s_Level->s_Fields[STRING_CONSTANTS::NAME_FIELD_BONUSES][key] = atoi( cpps->getValue(iter->first, "i").c_str() );
        }
        else if(iter->first.find("bonusdoor_") == 0)
        {
            string bonuseid = cpps->getValue(iter->first, "id");
            int key = atoi( bonuseid.c_str() );
            s_Data->s_Level->s_Fields[STRING_CONSTANTS::NAME_FIELD_BONUSDOORS][key] = atoi( cpps->getValue(iter->first, "i").c_str() );
        }
        else if(iter->first.find("door_") == 0)
        {
            string bonuseid = cpps->getValue(iter->first, "id");
            int key = atoi( bonuseid.c_str() );
            s_Data->s_Level->s_Fields[STRING_CONSTANTS::NAME_FIELD_DOORS][key] = atoi( cpps->getValue(iter->first, "i").c_str() );
        }
        else if(iter->first.find("exitleveldoor_") == 0)
        {
            string bonuseid = cpps->getValue(iter->first, "id");
            int key = atoi( bonuseid.c_str() );
            s_Data->s_Level->s_Fields[STRING_CONSTANTS::NAME_FIELD_EXITLEVELDOORS][key] = atoi( cpps->getValue(iter->first, "i").c_str() );
        }
}

bool Game::insertPlayer(int id, int numberOfSpawn, string nickname)
{
    if(s_GameInfo->s_Players[id] != 0)
    {
        s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error inserting player: this ID already used.");
        return false;
    }
    s_GameInfo->s_Players[id] = new CreaturePlayer(this);
    CreaturePlayer* s_Player = s_GameInfo->s_Players[id];
    string str = s_Data->s_Level->s_Params->getValue("Players", "numberofplayers");
    if(numberOfSpawn > atoi(str.c_str()))
    {
        s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error: this level haven't point of spawn with this number.");
        return false;
    }
    str = s_Data->s_Level->s_Params->getValue("Players", "player" + WorkFunctions::ConvertFunctions::itos(numberOfSpawn) + "X");
    if(str == "") str = "0";
    s_Player->s_CoordX = 16*atoi( str.c_str() );
    str = s_Data->s_Level->s_Params->getValue("Players", "player" + WorkFunctions::ConvertFunctions::itos(numberOfSpawn) + "Y");
    if(str == "") str = "0";
    s_Player->s_CoordY = 16*atoi( str.c_str() );
    s_Player->s_NickName = nickname;
    return true;
}

bool Game::removePlayer(int id)
{
    if(s_GameInfo->s_Players[id] == 0)
    {
        s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error removing player: this ID is empty.");
        return false;
    }
    delete s_GameInfo->s_Players[id];
    s_GameInfo->s_Players.erase(id);
    return true;
}
