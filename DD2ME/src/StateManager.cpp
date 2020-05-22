#include "StateManager.h"

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
                switch(wParam) //в параметре wParam код клавишы. Рассматриваем код
                {
                    case VK_SPACE:
                        switchState(1);
                        break;
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
                switch(wParam) //в параметре wParam код клавишы. Рассматриваем код
                {
                    case VK_SPACE:
                        switchState(2);
                        break;
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
                switch(wParam) //в параметре wParam код клавишы. Рассматриваем код
                {
                    case VK_SPACE:
                        switchState(3);
                        break;
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
                switch(wParam) //в параметре wParam код клавишы. Рассматриваем код
                {
                    case VK_SPACE:
                        switchState(0);
                        break;
                    case 0xC0:
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
                                if(str.substr(str.find(" ") + 1) == "off")
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
                                if(str.substr(str.find(" ") + 1) == "off")
                                {
                                    s_GameClass->s_GameInfo->s_IsAIOn = false;
                                    cout << "AI off." << endl;
                                }
                                if(str.substr(str.find(" ") + 1) == "reload")
                                {
                                    s_GameClass->s_GameInfo->s_FactoryMonsters->reloadAIAll();
                                    cout << "Reload AI completed." << endl;
                                }
                            }
                        }
                        break;
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
        s_GameClass->s_GameInfo->s_DopScreenCoordX = 0;
        s_GameClass->s_GameInfo->s_DopScreenCoordY = 0;
        s_MainscreenPar->s_StateMove = false;
        s_MainscreenPar->s_Direction = 1;
        s_MainscreenPar->s_Fix = atoi ( s_GameClass->s_IniFile->getValue("video", "mainscreentimefreez").c_str() );
        s_MainscreenPar->s_CurrentScreen = 0;
    }
    if(state == 2)
    {
        if(!s_GameClass->changeLevel(s_GameClass->s_GameInfo->s_CurrentLevel, false)) return;
    }
    if(state == 3)
    {
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
