#include "StateManager.h"

#include "Defines.h"

#include "WorkFunctions.h"

#include "Game.h"

using namespace std;

using namespace sf;

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

//LRESULT StateManager::s0(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
void StateManager::s0(Event event)
{
    switch (event.type)
    {
        case Event::KeyPressed:
                if(event.key.code == s_GameClass->s_GameInfo->s_KeySkip)
                {
                    switchState(1);
                }
                break;
    }
}
//LRESULT StateManager::s1(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
void StateManager::s1(Event event)
{
    switch (event.type)
    {
        case Event::KeyPressed:
                if(event.key.code == s_GameClass->s_GameInfo->s_KeySkip)
                {
                    switchState(2);
                }
                break;
    }
}
//LRESULT StateManager::s2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
void StateManager::s2(Event event)
{
    switch (event.type)
    {
        case Event::KeyPressed:
                if(event.key.code == s_GameClass->s_GameInfo->s_KeySkip) //в параметре wParam код клавишы. Рассматриваем код
                {
                    switchState(3);
                }
                break;
    }
}
//LRESULT StateManager::s3(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
void StateManager::s3(Event event)
{
    string str;
    switch (event.type)
    {
        case Event::KeyPressed:
            if(event.key.code == s_GameClass->s_GameInfo->s_KeySkip)
            {
                switchState(0);
                if(s_GameClass->s_GameInfo->s_CurrentLives <= 0) s_GameClass->s_GameInfo->doChangeLevelOnGameOver();
            }
            else if(event.key.code == s_GameClass->s_GameInfo->s_KeyConsole)
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
                        else if(str.substr(str.find(" ") + 1) == "count")
                        {
                            cout << "Current AI objects loaded: " << s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters.size() << endl;
                        }
                        else if(str.substr(str.find(" ") + 1).find("kill ") == 0)
                        {
                            str = str.substr(str.find(" ") + 1);
                            str = str.substr(str.find(" ") + 1);
                            int num_kill = atoi(str.c_str());
                            int num_killed = 0;
                            int i = 0;
                            while(num_killed < num_kill && i < s_GameClass->s_GameInfo->s_FactoryMonsters->s_MaxIndex)
                            {
                                if(s_GameClass->s_GameInfo->s_FactoryMonsters->isExistsById(i))
                                {
                                    s_GameClass->s_GameInfo->s_FactoryMonsters->removeMonster(i);
                                    num_killed++;
                                }
                                i++;
                            }
                            cout << "Killed " << str << " AI objects." << endl;
                        }
                    }
                    if(str.find("add ") == 0)
                    {
                        if(str.substr(str.find(" ") + 1).find("points ") == 0)
                        {
                            str = str.substr(str.find(" ") + 1);
                            str = str.substr(str.find(" ") + 1);
                            s_GameClass->s_GameInfo->s_MyPlayer->s_CurrentPoints += atoi(str.c_str());
                            cout << "Added " << str << " points." << endl;
                        }
                        else if(str.substr(str.find(" ") + 1).find("lives ") == 0)
                        {
                            str = str.substr(str.find(" ") + 1);
                            str = str.substr(str.find(" ") + 1);
                            s_GameClass->s_GameInfo->s_CurrentLives += atoi(str.c_str());
                            cout << "Added " << str << " lives." << endl;
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
            else if(event.key.code == s_GameClass->s_GameInfo->s_KeyInfo)
            {
                if(!s_GameClass->s_GameInfo->s_IsInfoShow)
                {
                    s_GameClass->s_Gui->showInfo();
                }
                else
                {
                    s_GameClass->s_Gui->removeInfo();
                }
                s_GameClass->s_GameInfo->s_IsInfoShow = !s_GameClass->s_GameInfo->s_IsInfoShow;
            }
            break;
    }
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

    if(!sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyLeft))) s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyLeft = false;
    if(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyLeft))) s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyLeft = true;

    if(!sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyRight))) s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyRight = false;
    if(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyRight))) s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyRight = true;

    if(!sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyUp))) s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyUp = false;
    if(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyUp))) s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyUp = true;

    if(!sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyDown))) s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyDown = false;
    if(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyDown))) s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyDown = true;

    if(!sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyJump))) s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyJump = false;
    if(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyJump))) s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyJump = true;

    if(!sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyShoot))) s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyShoot = false;
    if(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyShoot))) s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyShoot = true;

    if(!sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyLeftDown))) s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyLeftDown = false;
    if(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyLeftDown))) s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyLeftDown = true;

    if(!sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyLeftUp))) s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyLeftUp = false;
    if(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyLeftUp))) s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyLeftUp = true;

    if(!sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyRightDown))) s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyRightDown = false;
    if(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyRightDown))) s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyRightDown = true;

    if(!sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyRightUp))) s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyRightUp = false;
    if(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyRightUp))) s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyRightUp = true;
}

bool StateManager::switchState(int state, bool callstartstate)
{
    s_GameClass->s_GameInfo->s_IsInfoShow = false;
    s_GameClass->s_Gui->removeInfo(true);
    s_GameClass->s_Gui->s_TGUI->removeAllWidgets();
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
        s_GameClass->s_GameInfo->s_DopScreenCoordY = s_GameClass->s_DisplayStruct->s_GameResolutionY;

    }
    if(state == 1)
    {
        s_GameClass->s_Data->s_Sounds->stop("game_start");
        s_GameClass->s_Data->s_Music->play("main_menu");
        s_GameClass->s_GameInfo->s_DopScreenCoordX = 0;
        s_GameClass->s_GameInfo->s_DopScreenCoordY = 0;
        s_MainscreenPar->s_StateMove = false;
        s_MainscreenPar->s_Direction = 1;
        s_MainscreenPar->s_Fix = atoi ( s_GameClass->s_IniFile->getValue("video", "mainscreentimefreeze").c_str() );
        s_MainscreenPar->s_CurrentScreen = 0;
    }
    if(state == 2)
    {
        s_GameClass->s_Data->s_Music->stopAllMusic();
        if(!s_GameClass->changeLevel(s_GameClass->s_GameInfo->s_CurrentLevel, false)) return;
    }
    if(state == 3)
    {
        if( !s_GameClass->s_Data->s_Music->play("level_" + WorkFunctions::ConvertFunctions::itos(s_GameClass->s_GameInfo->s_CurrentLevel)) ) s_GameClass->s_Data->s_Music->play("in_game");
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
        s_MainscreenPar->s_Fix--;
        if(s_MainscreenPar->s_Fix == 0 && atoi( s_GameClass->s_Data->s_Screens->s_MainScreenInfo->getValue("info", "numberofscreens").c_str() ) > 1)
        {
            s_MainscreenPar->s_StateMove = true;
        }
        if(s_MainscreenPar->s_Fix < 0) s_MainscreenPar->s_Fix = -1;
        if(s_MainscreenPar->s_StateMove == true)
        {
            s_GameClass->s_GameInfo->s_DopScreenCoordX = s_GameClass->s_GameInfo->s_DopScreenCoordX - 4*s_MainscreenPar->s_Direction;
            if(s_GameClass->s_GameInfo->s_DopScreenCoordX%s_GameClass->s_DisplayStruct->s_GameResolutionX == 0)
            {
                s_MainscreenPar->s_StateMove = false;
                s_MainscreenPar->s_Fix = atoi ( s_GameClass->s_IniFile->getValue("video", "mainscreentimefreeze").c_str() );
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
        if(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyRight)))
            return true;
    if(type == "left")
        if(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(s_GameClass->s_GameInfo->s_KeyLeft)))
            return true;
    return false;
}
