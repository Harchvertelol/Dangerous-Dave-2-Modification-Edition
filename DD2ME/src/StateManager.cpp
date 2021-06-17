#include "StateManager.h"

#include "Defines.h"

#include "Game.h"

StateManager::StateManager(Game* gameclass):
    s_MaxState(3)
{
    s_GameClass = gameclass;
    s_MainscreenPar = new MainscreenPar;
}

StateManager::~StateManager()
{
    if(s_MainscreenPar != 0) delete s_MainscreenPar;
}

LRESULT StateManager::s0(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) //рассматриваем ИМЯ сообщения
    {
        case WM_KEYDOWN: //сообщение WM_KEYDOWN - пользователь нажал какую-то клавишу
                if(wParam == s_GameClass->s_GameInfo->s_KeySkip) //в параметре wParam код клавишы. Рассматриваем код
                {
                    switchState(1);
                }
                break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam); //все остальные сообщения идут сюда. Лучше не трогать =)
}
LRESULT StateManager::s1(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) //рассматриваем ИМЯ сообщения
    {
        case WM_KEYDOWN: //сообщение WM_KEYDOWN - пользователь нажал какую-то клавишу
                if(wParam == s_GameClass->s_GameInfo->s_KeySkip) //в параметре wParam код клавишы. Рассматриваем код
                {
                    switchState(2);
                }
                break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam); //все остальные сообщения идут сюда. Лучше не трогать =)
}
LRESULT StateManager::s2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) //рассматриваем ИМЯ сообщения
    {
        case WM_KEYDOWN: //сообщение WM_KEYDOWN - пользователь нажал какую-то клавишу
                if(wParam == s_GameClass->s_GameInfo->s_KeySkip) //в параметре wParam код клавишы. Рассматриваем код
                {
                    switchState(3);
                }
                break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam); //все остальные сообщения идут сюда. Лучше не трогать =)
}
LRESULT StateManager::s3(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    string str;
    switch (uMsg) //рассматриваем ИМЯ сообщения
    {
        case WM_KEYDOWN: //сообщение WM_KEYDOWN - пользователь нажал какую-то клавишу
                if(wParam == s_GameClass->s_GameInfo->s_KeySkip) //в параметре wParam код клавишы. Рассматриваем код
                {
                    switchState(0);
                    if(s_GameClass->s_GameInfo->s_CurrentLives <= 0)
                    {
                        s_GameClass->s_GameInfo->s_CurrentLives = GC_START_LIVES_NUMBER;
                        if(s_GameClass->s_GameInfo->s_CurrentLevel > 1) s_GameClass->s_GameInfo->s_CurrentLevel--;
                    }
                }
                else if(wParam == s_GameClass->s_GameInfo->s_KeyConsole)
                {
                    if(s_GameClass->s_IniFile->getValue("system", "consolemode") == "true")
                    {
                        cout<<"console> ";
                        char buf[5000];
                        cin.getline(buf, 5000);
                        str = buf;
                        if(str.find("level ") == 0)
                        {
                            s_GameClass->changeLevel(atoi( str.substr(str.find(" ") + 1).c_str()));
                        }
                        if(str.find("god ") == 0)
                        {
                            if(str.substr(str.find(" ") + 1) == "on")
                            {
                                s_GameClass->s_GameInfo->s_CheatGod = true;
                                cout << "God on." << endl;
                            }
                            else if(str.substr(str.find(" ") + 1) == "off")
                            {
                                s_GameClass->s_GameInfo->s_CheatGod = false;
                                cout << "God off." << endl;
                            }
                        }
                        if(str.find("AI ") == 0)
                        {
                            if(str.substr(str.find(" ") + 1) == "on")
                            {
                                s_GameClass->s_GameInfo->s_IsAIOn = true;
                                cout << "AI on." << endl;
                            }
                            else if(str.substr(str.find(" ") + 1) == "off")
                            {
                                s_GameClass->s_GameInfo->s_IsAIOn = false;
                                cout << "AI off." << endl;
                            }
                            else if(str.substr(str.find(" ") + 1) == "reload")
                            {
                                s_GameClass->s_GameInfo->s_FactoryMonsters->reloadAIAll();
                                cout << "Reload AI completed." << endl;
                            }
                        }
                        if(str.find("ghost ") == 0)
                        {
                            if(str.substr(str.find(" ") + 1) == "on")
                            {
                                s_GameClass->s_GameInfo->s_IsGhostOn = true;
                                cout << "Ghost on." << endl;
                            }
                            else if(str.substr(str.find(" ") + 1) == "off")
                            {
                                s_GameClass->s_GameInfo->s_IsGhostOn = false;
                                cout << "Ghost off." << endl;
                            }
                        }
                    }
                }
                break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam); //все остальные сообщения идут сюда. Лучше не трогать =)
}

void StateManager::s0I()
{
    //...
}

void StateManager::s1I()
{
    //...
}

void StateManager::s2I()
{
    //...
}

void StateManager::s3I()
{
    if(s_GameClass->s_NetClient->s_NetInfo->getValue("settings", "keysaction") == "false") return;

    if(!(GetKeyState(s_GameClass->s_GameInfo->s_KeyLeft) & 0x80)) s_GameClass->s_GameInfo->s_MyDave->s_KeysState->s_KeyLeft = false;
    if((GetKeyState(s_GameClass->s_GameInfo->s_KeyLeft) & 0x80)) s_GameClass->s_GameInfo->s_MyDave->s_KeysState->s_KeyLeft = true;

    if(!(GetKeyState(s_GameClass->s_GameInfo->s_KeyRight) & 0x80)) s_GameClass->s_GameInfo->s_MyDave->s_KeysState->s_KeyRight = false;
    if((GetKeyState(s_GameClass->s_GameInfo->s_KeyRight) & 0x80)) s_GameClass->s_GameInfo->s_MyDave->s_KeysState->s_KeyRight = true;

    if(!(GetKeyState(s_GameClass->s_GameInfo->s_KeyUp) & 0x80)) s_GameClass->s_GameInfo->s_MyDave->s_KeysState->s_KeyUp = false;
    if((GetKeyState(s_GameClass->s_GameInfo->s_KeyUp) & 0x80)) s_GameClass->s_GameInfo->s_MyDave->s_KeysState->s_KeyUp = true;

    if(!(GetKeyState(s_GameClass->s_GameInfo->s_KeyDown) & 0x80)) s_GameClass->s_GameInfo->s_MyDave->s_KeysState->s_KeyDown = false;
    if((GetKeyState(s_GameClass->s_GameInfo->s_KeyDown) & 0x80)) s_GameClass->s_GameInfo->s_MyDave->s_KeysState->s_KeyDown = true;

    if(!(GetKeyState(s_GameClass->s_GameInfo->s_KeyJump) & 0x80)) s_GameClass->s_GameInfo->s_MyDave->s_KeysState->s_KeyJump = false;
    if((GetKeyState(s_GameClass->s_GameInfo->s_KeyJump) & 0x80)) s_GameClass->s_GameInfo->s_MyDave->s_KeysState->s_KeyJump = true;

    if(!(GetKeyState(s_GameClass->s_GameInfo->s_KeyShoot) & 0x80)) s_GameClass->s_GameInfo->s_MyDave->s_KeysState->s_KeyShoot = false;
    if((GetKeyState(s_GameClass->s_GameInfo->s_KeyShoot) & 0x80)) s_GameClass->s_GameInfo->s_MyDave->s_KeysState->s_KeyShoot = true;

    if(!(GetKeyState(s_GameClass->s_GameInfo->s_KeyLeftDown) & 0x80)) s_GameClass->s_GameInfo->s_MyDave->s_KeysState->s_KeyLeftDown = false;
    if((GetKeyState(s_GameClass->s_GameInfo->s_KeyLeftDown) & 0x80)) s_GameClass->s_GameInfo->s_MyDave->s_KeysState->s_KeyLeftDown = true;

    if(!(GetKeyState(s_GameClass->s_GameInfo->s_KeyLeftUp) & 0x80)) s_GameClass->s_GameInfo->s_MyDave->s_KeysState->s_KeyLeftUp = false;
    if((GetKeyState(s_GameClass->s_GameInfo->s_KeyLeftUp) & 0x80)) s_GameClass->s_GameInfo->s_MyDave->s_KeysState->s_KeyLeftUp = true;

    if(!(GetKeyState(s_GameClass->s_GameInfo->s_KeyRightDown) & 0x80)) s_GameClass->s_GameInfo->s_MyDave->s_KeysState->s_KeyRightDown = false;
    if((GetKeyState(s_GameClass->s_GameInfo->s_KeyRightDown) & 0x80)) s_GameClass->s_GameInfo->s_MyDave->s_KeysState->s_KeyRightDown = true;

    if(!(GetKeyState(s_GameClass->s_GameInfo->s_KeyRightUp) & 0x80)) s_GameClass->s_GameInfo->s_MyDave->s_KeysState->s_KeyRightUp = false;
    if((GetKeyState(s_GameClass->s_GameInfo->s_KeyRightUp) & 0x80)) s_GameClass->s_GameInfo->s_MyDave->s_KeysState->s_KeyRightUp = true;
}

bool StateManager::switchState(int state, bool callstartstate)
{
    if(state > s_MaxState) return false;
    s_GameClass->s_GameInfo->s_GameState = state;
    if(callstartstate) startState(state);
    return true;
}

void StateManager::startState(int state)
{
    if(state == 0)
    {
        s_GameClass->s_Data->s_Music->stopAllMusic();
        s_GameClass->s_Data->s_Sounds->play("game_start");
        if(s_GameClass->s_GameInfo->s_Stop == true && s_GameClass->s_GameInfo->s_DeathType == 0)
        {
            s_GameClass->s_GameInfo->s_CurrentLevel = 1;
            s_GameClass->s_GameInfo->s_Stop = false;
        }
        s_GameClass->s_GameInfo->s_DopScreenCoordX = 0;
        s_GameClass->s_GameInfo->s_DopScreenCoordY = s_GameClass->s_DisplayStruct->s_ResolutionY;

    }
    if(state == 1)
    {
        s_GameClass->s_Data->s_Sounds->stop("game_start");
        s_GameClass->s_Data->s_Music->play("main_menu");
        s_GameClass->s_GameInfo->s_DopScreenCoordX = 0;
        s_GameClass->s_GameInfo->s_DopScreenCoordY = 0;
        s_MainscreenPar->s_StateMove = false;
        s_MainscreenPar->s_Direction = 1;
        s_MainscreenPar->s_Fix = atoi ( s_GameClass->s_IniFile->getValue("video", "mainscreentimefreez").c_str() );
        s_MainscreenPar->s_CurrentScreen = 0;
    }
    if(state == 2)
    {
        s_GameClass->s_Data->s_Music->stopAllMusic();
        if(!s_GameClass->changeLevel(s_GameClass->s_GameInfo->s_CurrentLevel, false)) return;
    }
    if(state == 3)
    {
        s_GameClass->s_Data->s_Music->play("in_game");
        s_GameClass->s_Data->s_Sounds->stop("before_first_level");
        s_GameClass->s_Data->s_Sounds->stop("changelevel");
        s_GameClass->s_Data->s_Sounds->stop("gameover");
        s_GameClass->s_Data->s_Sounds->stop("death");
        s_GameClass->correctionAllScreens();
    }
}

void StateManager::doState(int state)
{
    if(state == 0)
    {
        s_GameClass->s_GameInfo->s_DopScreenCoordY -= 2;
        if(s_GameClass->s_GameInfo->s_DopScreenCoordY == 0) switchState(1);
    }
    if(state == 1)
    {
        if(s_MainscreenPar->s_Fix == 0)
        {
            s_MainscreenPar->s_StateMove = true;
        }
        if(s_MainscreenPar->s_Fix < 0) s_MainscreenPar->s_Fix = -1;
        if(s_MainscreenPar->s_StateMove == true)
        {
            s_GameClass->s_GameInfo->s_DopScreenCoordX = s_GameClass->s_GameInfo->s_DopScreenCoordX - 4*s_MainscreenPar->s_Direction;
            if(s_GameClass->s_GameInfo->s_DopScreenCoordX%s_GameClass->s_DisplayStruct->s_ResolutionX == 0)
            {
                s_MainscreenPar->s_StateMove = false;
                s_MainscreenPar->s_Fix = atoi ( s_GameClass->s_IniFile->getValue("video", "mainscreentimefreez").c_str() );
                if(s_MainscreenPar->s_Direction == 1) s_MainscreenPar->s_CurrentScreen++;
                if(s_MainscreenPar->s_Direction == -1) s_MainscreenPar->s_CurrentScreen--;
                if(s_MainscreenPar->s_CurrentScreen == atoi( s_GameClass->s_Data->s_Screens->s_MainScreenInfo->getValue("info", "numberofscreens").c_str() ) - 1 ) s_MainscreenPar->s_Direction = -1;
                if(s_MainscreenPar->s_CurrentScreen == 0) s_MainscreenPar->s_Direction = 1;
            }
        }
    }
}

bool StateManager::getKeyPressed(string type)
{
    if(type == "right")
        if((GetKeyState(s_GameClass->s_GameInfo->s_KeyRight) & 0x80))
            return true;
    if(type == "left")
        if((GetKeyState(s_GameClass->s_GameInfo->s_KeyLeft) & 0x80))
            return true;
    return false;
}
