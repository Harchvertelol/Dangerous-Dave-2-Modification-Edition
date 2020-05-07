#include "Game.h"

#include "WorkFunction.h"

#include "Defines.h"

#include <ctime>

namespace
{
    static Game* ourInst = 0;

    static LRESULT CALLBACK MyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {


        if(ourInst->s_NetClient->s_NetInfo->getValue("settings", "keysaction") == "false") return DefWindowProc(hwnd, uMsg, wParam, lParam);

        switch (uMsg) //������������� ��� ���������
        {
            case WM_PAINT: //��������� WM_PAINT - ��������� ������������ ����
                ourInst->s_Window->paint(); //������ Canvas ��������������
                break;

            case WM_DESTROY: //WM_DESTROY - ������������ ����� �� ������� �� ���� (�������)
                ourInst->s_NetClient->s_NetInfoStruct->s_goGameOnServer = false;
                PostQuitMessage(0); //������� ������������ �������, ��� ��� ����� ������� ���� ���������
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

bool Game::changeLevel(int number, bool switchstate)
{
    if(number < 1 || number > atoi( s_Data->s_LevelsInfo->getValue("info", "numberoflevels").c_str()  ) )
    {
        s_GameInfo->s_Stop = true;
        return false;
    }
    s_GameInfo->s_CurrentLevel = number;
    s_GameInfo->s_FactoryMonsters->clear();
    if(!s_Data->s_Level->loadLevel( s_Data->PathToLevelPack + WorkFunction::ConvertFunction::itos(number))) return false;
    //...
    map<int, Creature*>::iterator iter;
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
    s_GameInfo->s_Stop = false;
    s_GameInfo->s_MyDave->s_Cartridges = s_GameInfo->s_MyDave->s_MaxCartridges;
    if(switchstate == true) s_StateManager->switchState(2);
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
    //��� ������ � ID = 7, �������� ������ 100 �����������. �� ���� wparam ����� ����� 7, ���� ������� - ������.

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
        if(m.is_timer()) cout << m.timer_id() << endl; // ����� �������� � id1, id2
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
            map<int, Creature*>::iterator iter;
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

void Game::drawAll()
{
    if(s_GameInfo->s_GameState == 3)
    {
        if(s_NetClient->s_NetInfo->getValue("program", "mode") == "client")
        {
            s_GameInfo->correctionScreen(s_GameInfo->s_MyDave);
            s_GameInfo->s_ScreenCoordX = s_GameInfo->s_MyDave->s_ScreenCoordX;
            s_GameInfo->s_ScreenCoordY = s_GameInfo->s_MyDave->s_ScreenCoordY;
        }
        map<int, Creature*>::iterator iter;
        for ( iter = s_GameInfo->s_Daves.begin(); iter != s_GameInfo->s_Daves.end(); iter++)
        {
            s_GameInfo->correctionScreen(iter->second);
        }
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
        s_Data->drawScreenState3();
        if(s_GameInfo->s_Stop == true && s_GameInfo->s_DeathType != 0) s_GameInfo->playDeath();
    }
    if(s_GameInfo->s_Stop == true && s_GameInfo->s_DeathType == 0)
    {
        s_Window->draw(Label("Thank you for playing in DD2:ME", s_DisplayStruct->s_ResolutionX/2 - 110, s_DisplayStruct->s_ResolutionY/2 - 30), Pen(255, 255, 20));
        s_Window->draw(Label("pre-beta 0.4!", s_DisplayStruct->s_ResolutionX/2 - 40, s_DisplayStruct->s_ResolutionY/2), Pen(255, 255, 20));
        s_Window->draw(Label("You win!", s_DisplayStruct->s_ResolutionX/2 - 20, s_DisplayStruct->s_ResolutionY/2 + 30), Pen(255, 255, 20));
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
            cpps->setValue("monster_" + WorkFunction::ConvertFunction::itos(monsterid), "coordX", WorkFunction::ConvertFunction::itos(iter->second->s_CoordX) );
            cpps->setValue("monster_" + WorkFunction::ConvertFunction::itos(monsterid), "coordY", WorkFunction::ConvertFunction::itos(iter->second->s_CoordY) );
            cpps->setValue("monster_" + WorkFunction::ConvertFunction::itos(monsterid), "currentLives", WorkFunction::ConvertFunction::itos(iter->second->s_CurrentLives) );
            cpps->setValue("monster_" + WorkFunction::ConvertFunction::itos(monsterid), "state", iter->second->s_State);
            cpps->setValue("monster_" + WorkFunction::ConvertFunction::itos(monsterid), "number", WorkFunction::ConvertFunction::itos(iter->second->s_Number) );
            cpps->setValue("monster_" + WorkFunction::ConvertFunction::itos(monsterid), "numberOfAction", WorkFunction::ConvertFunction::itos(iter->second->s_NumberOfAction) );
            cpps->setValue("monster_" + WorkFunction::ConvertFunction::itos(monsterid), "dopNumberOfAction", WorkFunction::ConvertFunction::itos(iter->second->s_AdditionalNumberOfAction) );
            cpps->setValue("monster_" + WorkFunction::ConvertFunction::itos(monsterid), "id", WorkFunction::ConvertFunction::itos(monsterid) );
            map<string, string>::iterator iter1;
            for( iter1 = iter->second->s_GlobalValues.begin(); iter1 != iter->second->s_GlobalValues.end(); iter1++)
            {
                cpps->setValue("GlobalValues_monster_" + WorkFunction::ConvertFunction::itos(monsterid), iter1->first, iter1->second );
            }
        }
    map<int, Creature*>::iterator iter1;
    for( iter1 = s_GameInfo->s_Daves.begin(); iter1 != s_GameInfo->s_Daves.end(); iter1++)
    {
        //daveid++;
        daveid = iter1->first;
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "coordX", WorkFunction::ConvertFunction::itos(iter1->second->s_CoordX) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "coordY", WorkFunction::ConvertFunction::itos(iter1->second->s_CoordY) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "currentHealth", WorkFunction::ConvertFunction::itos(iter1->second->s_CurrentHealth) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "state", iter1->second->s_State);
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "numberOfAction", WorkFunction::ConvertFunction::itos(iter1->second->s_NumberOfAction) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "dopNumberOfAction", WorkFunction::ConvertFunction::itos(iter1->second->s_AdditionalNumberOfAction) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "CurrentPoints", WorkFunction::ConvertFunction::itos(iter1->second->s_CurrentPoints) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "MaxHealth", WorkFunction::ConvertFunction::itos(iter1->second->s_MaxHealth) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "Cartridges", WorkFunction::ConvertFunction::itos(iter1->second->s_Cartridges) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "MaxCartridges", WorkFunction::ConvertFunction::itos(iter1->second->s_MaxCartridges) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "JumpStep", WorkFunction::ConvertFunction::itos(iter1->second->s_JumpStep) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "NumberOfTilesJump", WorkFunction::ConvertFunction::itos(iter1->second->s_NumberOfTilesJump) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "FreezeJump", WorkFunction::ConvertFunction::itos(iter1->second->s_FreezeJump) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "Acceleration", WorkFunction::ConvertFunction::itos(iter1->second->s_Acceleration) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "TimeDoorOpen", WorkFunction::ConvertFunction::itos(iter1->second->s_TimeDoorOpen) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "StateBeforeOpenDoor", iter1->second->s_StateBeforeOpenDoor);
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "HowDoorOpen", iter1->second->s_HowDoorOpen);
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "DopState", iter1->second->s_DopState);
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "OldAnSt", WorkFunction::ConvertFunction::itos(iter1->second->s_OldAnSt) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "OldNumberOfAction", WorkFunction::ConvertFunction::itos(iter1->second->s_OldNumberOfAction) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "ShootNow", WorkFunction::ConvertFunction::itos(iter1->second->s_ShootNow) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "ScreenCoordX", WorkFunction::ConvertFunction::itos(iter1->second->s_ScreenCoordX) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "ScreenCoordY", WorkFunction::ConvertFunction::itos(iter1->second->s_ScreenCoordY) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "ControlJumpPressed", WorkFunction::ConvertFunction::itos( (bool)iter1->second->s_ControlJumpPressed) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "ControlShootPressed", WorkFunction::ConvertFunction::itos( (bool)iter1->second->s_ControlShootPressed) );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "NickName", iter1->second->s_NickName );
        cpps->setValue("dave_" + WorkFunction::ConvertFunction::itos(daveid), "id", WorkFunction::ConvertFunction::itos(daveid) );
        //...
        cpps->setValue("KeysState_dave_" + WorkFunction::ConvertFunction::itos(daveid), "keyLeft", WorkFunction::ConvertFunction::itos( (int)iter1->second->s_KeysState->s_KeyLeft) );
        cpps->setValue("KeysState_dave_" + WorkFunction::ConvertFunction::itos(daveid), "keyRight", WorkFunction::ConvertFunction::itos( (int)iter1->second->s_KeysState->s_KeyRight) );
        cpps->setValue("KeysState_dave_" + WorkFunction::ConvertFunction::itos(daveid), "keyUp", WorkFunction::ConvertFunction::itos( (int)iter1->second->s_KeysState->s_KeyUp) );
        cpps->setValue("KeysState_dave_" + WorkFunction::ConvertFunction::itos(daveid), "keyDown", WorkFunction::ConvertFunction::itos( (int)iter1->second->s_KeysState->s_KeyDown) );
        cpps->setValue("KeysState_dave_" + WorkFunction::ConvertFunction::itos(daveid), "keyShoot", WorkFunction::ConvertFunction::itos( (int)iter1->second->s_KeysState->s_KeyShoot) );
        cpps->setValue("KeysState_dave_" + WorkFunction::ConvertFunction::itos(daveid), "keyJump", WorkFunction::ConvertFunction::itos( (int)iter1->second->s_KeysState->s_KeyJump) );
    }
    int size_x_level = atoi( s_Data->s_Level->s_Params->getValue("info", "sizeX").c_str() );
    int size_y_level = atoi( s_Data->s_Level->s_Params->getValue("info", "sizeY").c_str() );
    for(int i = 0; i < size_x_level*size_y_level; i++)
    {
        if(s_Data->s_Level->s_Fields["FieldBonuses"][i] != 0)
        {
            cpps->setValue("bonuse_" + WorkFunction::ConvertFunction::itos(i), "i", WorkFunction::ConvertFunction::itos(s_Data->s_Level->s_Fields["FieldBonuses"][i]));
            cpps->setValue("bonuse_" + WorkFunction::ConvertFunction::itos(i), "id", WorkFunction::ConvertFunction::itos(i) );
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
    map<int, Creature*>::iterator iter_, iter2_;
    for (iter_ = s_GameInfo->s_Daves.begin(), iter2_ = s_GameInfo->s_Daves.end(); iter_ != iter2_;)
    {
        if(iter_->second != 0)
        {
            delete iter_->second;
            s_GameInfo->s_Daves.erase(iter_++);
        }
        else ++iter_;
    }
    int size_x_level = atoi( s_Data->s_Level->s_Params->getValue("info", "sizeX").c_str() );
    int size_y_level = atoi( s_Data->s_Level->s_Params->getValue("info", "sizeY").c_str() );
    for(int i = 0; i < size_x_level*size_y_level; i++)
    {
        s_Data->s_Level->s_Fields["FieldBonuses"][i] = 0;
    }
    string monsterid = "";
    map<string, map<string, string> >::iterator iter;
    for( iter = cpps->s_Variables.begin(); iter != cpps->s_Variables.end(); iter++)
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
            keymonster = s_GameInfo->s_FactoryMonsters->addMonster(number, coordX, coordY, false);
            s_GameInfo->s_FactoryMonsters->s_Monsters[keymonster]->s_CurrentLives = currentLives;
            s_GameInfo->s_FactoryMonsters->s_Monsters[keymonster]->s_State = state;
            s_GameInfo->s_FactoryMonsters->s_Monsters[keymonster]->s_NumberOfAction = numberOfAction;
            s_GameInfo->s_FactoryMonsters->s_Monsters[keymonster]->s_AdditionalNumberOfAction = dopNumberOfAction;
            map<string, string>::iterator iter1;
            for( iter1 = cpps->s_Variables["GlobalValues_monster_" + monsterid].begin(); iter1 != cpps->s_Variables["GlobalValues_monster_" + monsterid].end(); iter1++)
            {
                s_GameInfo->s_FactoryMonsters->s_Monsters[keymonster]->s_GlobalValues[iter1->first] = iter1->second;
            }
        }
        else if(iter->first.find("dave_") == 0)
        {
            //davekey++;
            string daveid = cpps->getValue(iter->first, "id");
            davekey = atoi( daveid.c_str() );
            coordX = atoi( cpps->getValue("dave_" + daveid, "coordX").c_str() );
            coordY = atoi( cpps->getValue("dave_" + daveid, "coordY").c_str() );
            int currentHealth = atoi( cpps->getValue("dave_" + daveid, "currentHealth").c_str() );
            state = cpps->getValue("dave_" + daveid, "state");
            numberOfAction = atoi( cpps->getValue("dave_" + daveid, "numberOfAction").c_str() );
            dopNumberOfAction = atoi( cpps->getValue("dave_" + daveid, "dopNumberOfAction").c_str() );
            int CurrentPoints = atoi( cpps->getValue("dave_" + daveid, "CurrentPoints").c_str() );
            int MaxHealth = atoi( cpps->getValue("dave_" + daveid, "MaxHealth").c_str() );
            int Cartridges = atoi( cpps->getValue("dave_" + daveid, "Cartridges").c_str() );
            int MaxCartridges = atoi( cpps->getValue("dave_" + daveid, "MaxCartridges").c_str() );
            int JumpStep = atoi( cpps->getValue("dave_" + daveid, "JumpStep").c_str() );
            int NumberOfTilesJump = atoi( cpps->getValue("dave_" + daveid, "NumberOfTilesJump").c_str() );
            int FreezeJump = atoi( cpps->getValue("dave_" + daveid, "FreezeJump").c_str() );
            int Acceleration = atoi( cpps->getValue("dave_" + daveid, "Acceleration").c_str() );
            int TimeDoorOpen = atoi( cpps->getValue("dave_" + daveid, "TimeDoorOpen").c_str() );
            string StateBeforeOpenDoor = cpps->getValue("dave_" + daveid, "StateBeforeOpenDoor");
            string HowDoorOpen = cpps->getValue("dave_" + daveid, "HowDoorOpen");
            string DopState = cpps->getValue("dave_" + daveid, "DopState");
            int OldAnSt = atoi( cpps->getValue("dave_" + daveid, "OldAnSt").c_str() );
            int OldNumberOfAction = atoi( cpps->getValue("dave_" + daveid, "OldNumberOfAction").c_str() );
            int ShootNow = atoi( cpps->getValue("dave_" + daveid, "ShootNow").c_str() );
            int ScreenCoordX = atoi( cpps->getValue("dave_" + daveid, "ScreenCoordX").c_str() );
            int ScreenCoordY = atoi( cpps->getValue("dave_" + daveid, "ScreenCoordY").c_str() );
            bool ControlShootPressed = (bool)atoi( cpps->getValue("dave_" + daveid, "ControlShootPressed").c_str() );
            bool ControlJumpPressed = (bool)atoi( cpps->getValue("dave_" + daveid, "ControlJumpPressed").c_str() );
            string NickName = cpps->getValue("dave_" + daveid, "NickName");
            //...
            s_GameInfo->s_Daves[davekey] = new Creature(this);
            s_GameInfo->s_Daves[davekey]->s_CoordX = coordX;
            s_GameInfo->s_Daves[davekey]->s_CoordY = coordY;
            s_GameInfo->s_Daves[davekey]->s_CurrentHealth = currentHealth;
            s_GameInfo->s_Daves[davekey]->s_State = state;
            s_GameInfo->s_Daves[davekey]->s_NumberOfAction = numberOfAction;
            s_GameInfo->s_Daves[davekey]->s_AdditionalNumberOfAction = dopNumberOfAction;
            s_GameInfo->s_Daves[davekey]->s_CurrentPoints = CurrentPoints;
            s_GameInfo->s_Daves[davekey]->s_MaxHealth = MaxHealth;
            s_GameInfo->s_Daves[davekey]->s_Cartridges = Cartridges;
            s_GameInfo->s_Daves[davekey]->s_MaxCartridges = MaxCartridges;
            s_GameInfo->s_Daves[davekey]->s_JumpStep = JumpStep;
            s_GameInfo->s_Daves[davekey]->s_NumberOfTilesJump = NumberOfTilesJump;
            s_GameInfo->s_Daves[davekey]->s_FreezeJump = FreezeJump;
            s_GameInfo->s_Daves[davekey]->s_Acceleration = Acceleration;
            s_GameInfo->s_Daves[davekey]->s_TimeDoorOpen = TimeDoorOpen;
            s_GameInfo->s_Daves[davekey]->s_StateBeforeOpenDoor = StateBeforeOpenDoor;
            s_GameInfo->s_Daves[davekey]->s_HowDoorOpen = HowDoorOpen;
            s_GameInfo->s_Daves[davekey]->s_DopState = DopState;
            s_GameInfo->s_Daves[davekey]->s_OldAnSt = OldAnSt;
            s_GameInfo->s_Daves[davekey]->s_OldNumberOfAction = OldNumberOfAction;
            s_GameInfo->s_Daves[davekey]->s_ShootNow = ShootNow;
            s_GameInfo->s_Daves[davekey]->s_ScreenCoordX = ScreenCoordX;
            s_GameInfo->s_Daves[davekey]->s_ScreenCoordY = ScreenCoordY;
            s_GameInfo->s_Daves[davekey]->s_ControlShootPressed = ControlShootPressed;
            s_GameInfo->s_Daves[davekey]->s_ControlJumpPressed = ControlJumpPressed;
            s_GameInfo->s_Daves[davekey]->s_NickName = NickName;
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
}

bool Game::insertDave(int id, int numberOfSpawn, string nickname)
{
    if(s_GameInfo->s_Daves[id] != 0)
    {
        cout<<"Error inserting Dave: this ID already used."<<endl;
        return false;
    }
    s_GameInfo->s_Daves[id] = new Creature(this);
    Creature* s_Dave = s_GameInfo->s_Daves[id];
    string str = s_Data->s_Level->s_Params->getValue("daves", "numberofdaves");
    if(numberOfSpawn > atoi(str.c_str()))
    {
        cout<<"Error: this level haven't point of spawn with this number."<<endl;
        return false;
    }
    str = s_Data->s_Level->s_Params->getValue("daves", "dave" + WorkFunction::ConvertFunction::itos(numberOfSpawn) + "X");
    if(str == "") str = "0";
    s_Dave->s_CoordX = 16*atoi( str.c_str() );
    str = s_Data->s_Level->s_Params->getValue("daves", "dave" + WorkFunction::ConvertFunction::itos(numberOfSpawn) + "Y");
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