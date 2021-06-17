#include "Game.h"

#include "WorkFunctions.h"

#include "Defines.h"

#include <ctime>

using namespace IniParser;

namespace
{
    static Game* ourInst = 0;

    static LRESULT CALLBACK MyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {


        if(ourInst->s_NetClient->s_NetInfo->getValue("settings", "keysaction") == "false") return DefWindowProc(hwnd, uMsg, wParam, lParam);

        switch (uMsg) //рассматриваем ИМЯ сообщения
        {
            case WM_PAINT: //сообщение WM_PAINT - требуется перерисовать окно
                ourInst->s_Window->paint(); //просим Canvas перерисоваться
                break;

            case WM_DESTROY: //WM_DESTROY - пользователь нажал на крестик на окне (закрыть)
                ourInst->s_NetClient->s_NetInfoStruct->s_goGameOnServer = false;
                PostQuitMessage(0); //говорим операционной системе, что она может закрыть нашу программу
                break;
        }
        if(wParam==SC_KEYMENU && (lParam>>16)<=0) return 0;
        if(GetForegroundWindow() != hwnd) return DefWindowProc(hwnd, uMsg, wParam, lParam);
        int gameState = ourInst->s_GameInfo->s_GameState;
        if(gameState == 0) return ourInst->s_StateManager->s0(hwnd, uMsg, wParam, lParam);
        if(gameState == 1) return ourInst->s_StateManager->s1(hwnd, uMsg, wParam, lParam);
        if(gameState == 2) return ourInst->s_StateManager->s2(hwnd, uMsg, wParam, lParam);
        if(gameState == 3) return ourInst->s_StateManager->s3(hwnd, uMsg, wParam, lParam);
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

}

Game::Game():
    s_Window(0),
    s_IniFile(0),
    s_NetClient(0),
    s_AnimationStep(0),
    s_IdGameClass(0),
    s_IdTimerAnimationStep(0),
    s_IdTimerCreaturesAnimationStep(0),
    s_IdTimerDrawStep(0),
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
    s_GameInfo = new GameInfo(this);
    s_DisplayStruct = new DisplayStruct;
    s_Data = new GameData(this);
    s_StateManager = new StateManager(this);
    s_FactoryTmpImgs = new FactoryTemporaryImages(this);
    s_AI = new AI(this);
    s_Gui = new Gui(this);
}

Game::~Game()
{
    if(s_GameInfo != 0) delete s_GameInfo;
    if(s_DisplayStruct != 0) delete s_DisplayStruct;
    if(s_IniFile != 0) delete s_IniFile;
    if(s_Data != 0) delete s_Data;
    if(s_StateManager != 0) delete s_StateManager;;
    if(s_FactoryTmpImgs != 0) delete s_FactoryTmpImgs;
    if(s_AI != 0) delete s_AI;
    if(s_Window != 0) delete s_Window;
    if(s_Gui != 0) delete s_Gui;
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
    if(timenew - (*timeold) > TIME_SECOND_FPS)
    {
        (*timeold) = timenew;
        (*fps) = (*calcfps);
        (*calcfps) = 0;
        if((*maximalfps) < (*fps)) (*maximalfps) = (*fps);
        if((*minimalfps) > (*fps)) (*minimalfps) = (*fps);
        if(s_IniFile->getValue("FPS", "ShowFPS") == "true" && s_IniFile->getValue("FPS", "ShowFPSMode") == "console")
        {
            if(s_IniFile->getValue("FPS", "ShowFPSGameDraw") == "true" ) cout<<"FPS game draw: "<<s_GameDrawFPS<<endl;
            if(s_IniFile->getValue("FPS", "ShowFPSGameDrawMaximal") == "true" ) cout<<"FPS game draw maximal: "<<s_GameDrawFPSMaximal<<endl;
            if(s_IniFile->getValue("FPS", "ShowFPSGameDrawMinimal") == "true" ) cout<<"FPS game draw minimal: "<<s_GameDrawFPSMinimal<<endl;
            if(s_IniFile->getValue("FPS", "ShowFPSTechnical") == "true" ) cout<<"FPS technical: "<<s_TechnicalFPS<<endl;
            if(s_IniFile->getValue("FPS", "ShowFPSTechnicalMaximal") == "true" ) cout<<"FPS technical maximal: "<<s_TechnicalFPSMaximal<<endl;
            if(s_IniFile->getValue("FPS", "ShowFPSTechnicalMinimal") == "true" ) cout<<"FPS technical minimal: "<<s_TechnicalFPSMinimal<<endl;
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

bool Game::changeLevel(int number, bool switchstate, bool playmusic)
{
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
    map<int, CreatureDave*>::iterator iter;
    for ( iter = s_GameInfo->s_Daves.begin(); iter != s_GameInfo->s_Daves.end(); iter++)
    {
        iter->second->s_State = "rightstand";
        iter->second->s_ShootNow = 0;
        iter->second->s_Cartridges = iter->second->s_MaxCartridges;
    }
    //...
    s_GameInfo->s_MyDave->s_State = "rightstand";
    s_FactoryTmpImgs->clear();
    s_GameInfo->s_MyDave->s_ShootNow = 0;
    s_GameInfo->s_ForcedChangeLevel = false;
    s_GameInfo->s_Stop = false;
    s_GameInfo->s_MyDave->s_Cartridges = s_GameInfo->s_MyDave->s_MaxCartridges;
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
    if(s_IniFile->getValue("video", "usedavecache") == "true") s_Data->s_Dave->createCache();
    if(s_IniFile->getValue("video", "usemonstercache") == "true") s_Data->s_Monsters->createCache();
    if(s_IniFile->getValue("video", "usetransparentmask") == "true")
    {
        //s_Data->s_Textures->createMaskTransparent(204, 255, 204);
        s_Data->s_Bonuses->createMaskTransparent(atoi(s_Data->s_Bonuses->s_GlobBonusesInfo->getValue("draw", "transparentmaskR").c_str()), atoi(s_Data->s_Bonuses->s_GlobBonusesInfo->getValue("draw", "transparentmaskG").c_str()), atoi(s_Data->s_Bonuses->s_GlobBonusesInfo->getValue("draw", "transparentmaskB").c_str()));
        s_Data->s_Screens->createMaskTransparent(atoi(s_Data->s_Screens->s_ScreensInfo->getValue("draw", "transparentmaskR").c_str()), atoi(s_Data->s_Screens->s_ScreensInfo->getValue("draw", "transparentmaskG").c_str()), atoi(s_Data->s_Screens->s_ScreensInfo->getValue("draw", "transparentmaskB").c_str()));
        s_Data->s_Dave->createMaskTransparent(atoi(s_Data->s_Dave->s_DaveInfo->getValue("draw", "transparentmaskR").c_str()), atoi(s_Data->s_Dave->s_DaveInfo->getValue("draw", "transparentmaskG").c_str()), atoi(s_Data->s_Dave->s_DaveInfo->getValue("draw", "transparentmaskB").c_str()));
        s_Data->s_Monsters->createMaskTransparent(atoi(s_Data->s_Monsters->s_GlobMonstersInfo->getValue("draw", "transparentmaskR").c_str()), atoi(s_Data->s_Monsters->s_GlobMonstersInfo->getValue("draw", "transparentmaskG").c_str()), atoi(s_Data->s_Monsters->s_GlobMonstersInfo->getValue("draw", "transparentmaskB").c_str()));
    }
    s_StateManager->switchState(0);
    configureForGame();
}

void Game::configureForGame()
{
    ourInst = this;
    if(s_NetClient->s_NetInfo->getValue("video", "graphicmodegame") == "true")
    {
        s_Window->start(MyWndProc);
        s_Window->enable_frame_buffer();
    }
    else deleteAllGDIObjects();

    drawAll();

    //SetTimer(s_Window->hwnd(), 6, atoi( s_IniFile->getValue("video", "animationstep").c_str() ), 0);
    //SetTimer(s_Window->hwnd(), 5, atoi( s_IniFile->getValue("video", "creaturesanimationstep").c_str() ), 0);
    //SetTimer(s_Window->hwnd(), 7, atoi( s_IniFile->getValue("video", "drawstep").c_str() ), 0);
    //это таймер с ID = 7, тикающий каждые 100 миллисекунд. по идее wparam будет равен 7, если событие - таймер.

    s_IdTimerAnimationStep = set_timer(atoi( s_IniFile->getValue("video", "animationstep").c_str() ));
    s_IdTimerCreaturesAnimationStep = set_timer(atoi( s_IniFile->getValue("video", "creaturesanimationstep").c_str() ));
    s_IdTimerDrawStep = set_timer(atoi( s_IniFile->getValue("video", "drawstep").c_str() ));
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
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        calculateTechnicalFPS();
        if(msg.message == WM_TIMER) onTimer(msg.wParam);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void Game::processAllEvents(int maxEvents)
{
    Message m;
    for(int i = 0; i < maxEvents && m.peek() == true; i++)
    {
        if(m.is_timer())
        {
            onTimer(m.timer_id());
        }
    }
}

void Game::onTimer(unsigned int timer_id)
{
    if(timer_id == s_IdTimerDrawStep)
    {
        s_Data->s_Sounds->clearInactiveTempSounds();
        calculateGameDrawFPS();
        if(s_Window != 0 && GetForegroundWindow() == s_Window->hwnd() )
        {
            if(s_GameInfo->s_GameState == 0 && s_GameInfo->s_Stop == false) s_StateManager->s0I();
            if(s_GameInfo->s_GameState == 1 && s_GameInfo->s_Stop == false) s_StateManager->s1I();
            if(s_GameInfo->s_GameState == 2 && s_GameInfo->s_Stop == false) s_StateManager->s2I();
            if(s_GameInfo->s_GameState == 3 && s_GameInfo->s_Stop == false) s_StateManager->s3I();
        }
        s_StateManager->doState(s_GameInfo->s_GameState);
        if(s_GameInfo->s_GameState == 3 && s_GameInfo->s_Stop == false) s_GameInfo->live();
        drawAll();
    }
    if(timer_id == s_IdTimerAnimationStep)
    {
        if(s_IniFile->getValue("video", "animation") == "true")
        {
            s_AnimationStep++;
            if(s_AnimationStep > MAXIMAL_UNSIGNED_NUMBER) s_AnimationStep = 0;
        }
        if(s_GameInfo->s_GameState == 1) s_StateManager->s_MainscreenPar->s_Fix--;
    }
    if(timer_id == s_IdTimerCreaturesAnimationStep)
    {
        if(s_IniFile->getValue("video", "animation") == "true")
        {
            //...
            map<int, CreatureDave*>::iterator iter;
            for ( iter = s_GameInfo->s_Daves.begin(); iter != s_GameInfo->s_Daves.end(); iter++)
            {
                iter->second->s_NumberOfAction++;
                if(iter->second->s_NumberOfAction > MAXIMAL_UNSIGNED_NUMBER) iter->second->s_NumberOfAction = 0;
            }
            //...
            s_GameInfo->s_MyDave->s_NumberOfAction++;
            if(s_GameInfo->s_MyDave->s_NumberOfAction > MAXIMAL_UNSIGNED_NUMBER) s_GameInfo->s_MyDave->s_NumberOfAction = 0;
        }
    }
}

void Game::correctionAllScreens()
{
    if(s_NetClient->s_NetInfo->getValue("program", "mode") == "client")
    {
        s_GameInfo->correctionScreen(s_GameInfo->s_MyDave);
        s_GameInfo->s_ScreenCoordX = s_GameInfo->s_MyDave->s_ScreenCoordX;
        s_GameInfo->s_ScreenCoordY = s_GameInfo->s_MyDave->s_ScreenCoordY;
    }
    map<int, CreatureDave*>::iterator iter;
    for ( iter = s_GameInfo->s_Daves.begin(); iter != s_GameInfo->s_Daves.end(); iter++)
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
    s_Window->clear();
    if(s_GameInfo->s_GameState == 0) s_Data->drawScreenState0();
    if(s_GameInfo->s_GameState == 1) s_Data->drawScreenState1();
    if(s_GameInfo->s_GameState == 2) s_Data->drawScreenState2();
    if(s_GameInfo->s_GameState == 3)
    {
        s_Data->s_Level->draw();
        s_FactoryTmpImgs->drawAll();
        s_GameInfo->s_FactoryMonsters->drawAll();
        map<int, CreatureDave*>::iterator iter;
        for ( iter = s_GameInfo->s_Daves.begin(); iter != s_GameInfo->s_Daves.end(); iter++)
        {
            iter->second->draw();
        }
        s_GameInfo->s_MyDave->draw();
        s_Data->drawScreenState3();
        if(s_GameInfo->s_Stop == true && s_GameInfo->s_DeathType != 0) s_GameInfo->playDeath();
    }
    s_Window->paint();
}

PostParsingStruct* Game::getObjects()
{
    PostParsingStruct* cpps = new PostParsingStruct;
    unsigned int monsterid = 0;
    unsigned int daveid = 0;
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
    for( iter1m = s_GameInfo->s_FactoryMonsters->s_AIMonstersValues.begin(); iter1m != s_GameInfo->s_FactoryMonsters->s_AIMonstersValues.end(); iter1m++)
    {
        cpps->setValue("AIMonstersValues", iter1m->first, iter1m->second );
    }
    map<int, CreatureDave*>::iterator iter1;
    for( iter1 = s_GameInfo->s_Daves.begin(); iter1 != s_GameInfo->s_Daves.end(); iter1++)
    {
        //daveid++;
        daveid = iter1->first;
        PostParsingStruct* gdpps = iter1->second->getListOfVariables("dave_" + WorkFunctions::ConvertFunctions::itos(daveid));
        cpps->addPostParsingStruct(gdpps);
        delete gdpps;

        cpps->setValue("dave_" + WorkFunctions::ConvertFunctions::itos(daveid), "id", WorkFunctions::ConvertFunctions::itos(daveid) );
        //...
        cpps->setValue("KeysState_dave_" + WorkFunctions::ConvertFunctions::itos(daveid), "keyLeft", WorkFunctions::ConvertFunctions::itos( (int)iter1->second->s_KeysState->s_KeyLeft) );
        cpps->setValue("KeysState_dave_" + WorkFunctions::ConvertFunctions::itos(daveid), "keyRight", WorkFunctions::ConvertFunctions::itos( (int)iter1->second->s_KeysState->s_KeyRight) );
        cpps->setValue("KeysState_dave_" + WorkFunctions::ConvertFunctions::itos(daveid), "keyUp", WorkFunctions::ConvertFunctions::itos( (int)iter1->second->s_KeysState->s_KeyUp) );
        cpps->setValue("KeysState_dave_" + WorkFunctions::ConvertFunctions::itos(daveid), "keyDown", WorkFunctions::ConvertFunctions::itos( (int)iter1->second->s_KeysState->s_KeyDown) );
        cpps->setValue("KeysState_dave_" + WorkFunctions::ConvertFunctions::itos(daveid), "keyShoot", WorkFunctions::ConvertFunctions::itos( (int)iter1->second->s_KeysState->s_KeyShoot) );
        cpps->setValue("KeysState_dave_" + WorkFunctions::ConvertFunctions::itos(daveid), "keyJump", WorkFunctions::ConvertFunctions::itos( (int)iter1->second->s_KeysState->s_KeyJump) );
    }
    int size_x_level = atoi( s_Data->s_Level->s_Params->getValue("info", "sizeX").c_str() );
    int size_y_level = atoi( s_Data->s_Level->s_Params->getValue("info", "sizeY").c_str() );
    for(int i = 0; i < size_x_level*size_y_level; i++)
    {
        if(s_Data->s_Level->s_Fields["FieldBonuses"][i] != 0)
        {
            cpps->setValue("bonuse_" + WorkFunctions::ConvertFunctions::itos(i), "i", WorkFunctions::ConvertFunctions::itos(s_Data->s_Level->s_Fields["FieldBonuses"][i]));
            cpps->setValue("bonuse_" + WorkFunctions::ConvertFunctions::itos(i), "id", WorkFunctions::ConvertFunctions::itos(i) );
        }
        if(s_Data->s_Level->s_Fields["FieldBonusDoors"][i] != 0)
        {
            cpps->setValue("bonusdoor_" + WorkFunctions::ConvertFunctions::itos(i), "i", WorkFunctions::ConvertFunctions::itos(s_Data->s_Level->s_Fields["FieldBonusDoors"][i]));
            cpps->setValue("bonusdoor_" + WorkFunctions::ConvertFunctions::itos(i), "id", WorkFunctions::ConvertFunctions::itos(i) );
        }
        if(s_Data->s_Level->s_Fields["FieldDoors"][i] != 0)
        {
            cpps->setValue("door_" + WorkFunctions::ConvertFunctions::itos(i), "i", WorkFunctions::ConvertFunctions::itos(s_Data->s_Level->s_Fields["FieldDoors"][i]));
            cpps->setValue("door_" + WorkFunctions::ConvertFunctions::itos(i), "id", WorkFunctions::ConvertFunctions::itos(i) );
        }
        if(s_Data->s_Level->s_Fields["FieldExitLevelDoors"][i] != 0)
        {
            cpps->setValue("exitleveldoor_" + WorkFunctions::ConvertFunctions::itos(i), "i", WorkFunctions::ConvertFunctions::itos(s_Data->s_Level->s_Fields["FieldExitLevelDoors"][i]));
            cpps->setValue("exitleveldoor_" + WorkFunctions::ConvertFunctions::itos(i), "id", WorkFunctions::ConvertFunctions::itos(i) );
        }
    }
    return cpps;
}

void Game::setObjects(PostParsingStruct* cpps)
{
    int keymonster = 0, davekey = 0;
    int coordX, coordY, currentLives, number, numberOfAction, dopNumberOfAction;
    string state;
    s_GameInfo->s_FactoryMonsters->clear();
    map<int, CreatureDave*>::iterator iter_, iter2_;
    for (iter_ = s_GameInfo->s_Daves.begin(), iter2_ = s_GameInfo->s_Daves.end(); iter_ != iter2_;)
    {
        if(iter_->second != 0)
        {
            if(!cpps->isExists("dave_" + WorkFunctions::ConvertFunctions::itos(iter_->first)))
            {
                delete iter_->second;
                s_GameInfo->s_Daves.erase(iter_++);
            }
            else ++iter_;
        }
        else ++iter_;
    }
    int size_x_level = atoi( s_Data->s_Level->s_Params->getValue("info", "sizeX").c_str() );
    int size_y_level = atoi( s_Data->s_Level->s_Params->getValue("info", "sizeY").c_str() );
    for(int i = 0; i < size_x_level*size_y_level; i++)
    {
        s_Data->s_Level->s_Fields["FieldBonuses"][i] = 0;
        s_Data->s_Level->s_Fields["FieldBonusDoors"][i] = 0;
        s_Data->s_Level->s_Fields["FieldDoors"][i] = 0;
        s_Data->s_Level->s_Fields["FieldExitLevelDoors"][i] = 0;
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
            for( iter1 = cpps->getMapVariables()["AIMonstersValues"].begin(); iter1 != cpps->getMapVariables()["AIMonstersValues"].end(); iter1++)
            {
                s_GameInfo->s_FactoryMonsters->s_AIMonstersValues[iter1->first] = iter1->second;
            }
        }
        else if(iter->first.find("dave_") == 0)
        {
            //davekey++;
            string daveid = cpps->getValue(iter->first, "id");
            davekey = atoi( daveid.c_str() );
            if(!s_GameInfo->s_Daves[davekey]) s_GameInfo->s_Daves[davekey] = new CreatureDave(this);
            s_GameInfo->s_Daves[davekey]->setListOfVariables(cpps, "dave_" + daveid);
            //...
            s_GameInfo->s_Daves[davekey]->s_KeysState->s_KeyLeft = (bool)atoi( cpps->getValue("KeysState_dave_" + daveid, "keyLeft").c_str() );
            s_GameInfo->s_Daves[davekey]->s_KeysState->s_KeyRight = (bool)atoi( cpps->getValue("KeysState_dave_" + daveid, "keyRight").c_str() );
            s_GameInfo->s_Daves[davekey]->s_KeysState->s_KeyUp = (bool)atoi( cpps->getValue("KeysState_dave_" + daveid, "keyUp").c_str() );
            s_GameInfo->s_Daves[davekey]->s_KeysState->s_KeyDown = (bool)atoi( cpps->getValue("KeysState_dave_" + daveid, "keyDown").c_str() );
            s_GameInfo->s_Daves[davekey]->s_KeysState->s_KeyShoot = (bool)atoi( cpps->getValue("KeysState_dave_" + daveid, "keyShoot").c_str() );
            s_GameInfo->s_Daves[davekey]->s_KeysState->s_KeyJump = (bool)atoi( cpps->getValue("KeysState_dave_" + daveid, "keyJump").c_str() );
        }
        else if(iter->first.find("bonuse_") == 0)
        {
            string bonuseid = cpps->getValue(iter->first, "id");
            int key = atoi( bonuseid.c_str() );
            s_Data->s_Level->s_Fields["FieldBonuses"][key] = atoi( cpps->getValue(iter->first, "i").c_str() );
        }
        else if(iter->first.find("bonusdoor_") == 0)
        {
            string bonuseid = cpps->getValue(iter->first, "id");
            int key = atoi( bonuseid.c_str() );
            s_Data->s_Level->s_Fields["FieldBonusDoors"][key] = atoi( cpps->getValue(iter->first, "i").c_str() );
        }
        else if(iter->first.find("door_") == 0)
        {
            string bonuseid = cpps->getValue(iter->first, "id");
            int key = atoi( bonuseid.c_str() );
            s_Data->s_Level->s_Fields["FieldDoors"][key] = atoi( cpps->getValue(iter->first, "i").c_str() );
        }
        else if(iter->first.find("exitleveldoor_") == 0)
        {
            string bonuseid = cpps->getValue(iter->first, "id");
            int key = atoi( bonuseid.c_str() );
            s_Data->s_Level->s_Fields["FieldExitLevelDoors"][key] = atoi( cpps->getValue(iter->first, "i").c_str() );
        }
}

bool Game::insertDave(int id, int numberOfSpawn, string nickname)
{
    if(s_GameInfo->s_Daves[id] != 0)
    {
        cout<<"Error inserting Dave: this ID already used."<<endl;
        return false;
    }
    s_GameInfo->s_Daves[id] = new CreatureDave(this);
    CreatureDave* s_Dave = s_GameInfo->s_Daves[id];
    string str = s_Data->s_Level->s_Params->getValue("daves", "numberofdaves");
    if(numberOfSpawn > atoi(str.c_str()))
    {
        cout<<"Error: this level haven't point of spawn with this number."<<endl;
        return false;
    }
    str = s_Data->s_Level->s_Params->getValue("daves", "dave" + WorkFunctions::ConvertFunctions::itos(numberOfSpawn) + "X");
    if(str == "") str = "0";
    s_Dave->s_CoordX = 16*atoi( str.c_str() );
    str = s_Data->s_Level->s_Params->getValue("daves", "dave" + WorkFunctions::ConvertFunctions::itos(numberOfSpawn) + "Y");
    if(str == "") str = "0";
    s_Dave->s_CoordY = 16*atoi( str.c_str() );
    s_Dave->s_NickName = nickname;
    return true;
}

bool Game::removeDave(int id)
{
    if(s_GameInfo->s_Daves[id] == 0)
    {
        cout<<"Error removing Dave: this ID is empty."<<endl;
        return false;
    }
    delete s_GameInfo->s_Daves[id];
    s_GameInfo->s_Daves.erase(id);
    return true;
}
