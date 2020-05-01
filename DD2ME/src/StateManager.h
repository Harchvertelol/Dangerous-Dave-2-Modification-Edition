#ifndef STATEMANAGER_H
#define STATEMANAGER_H
#include "windows.h"

#include <iostream>
using namespace std;

class Game;

struct MainscreenPar
{
    bool s_StateMove;
    int s_Direction;
    int s_Fix;
    int s_CurrentScreen;
};

class StateManager
{
    public:
        StateManager(Game*);
        ~StateManager();
        LRESULT s0(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT s1(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT s2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT s3(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        void s0I();
        void s1I();
        void s2I();
        void s3I();
        bool switchState(int);
        void startState(int);
        void doState(int);
        Game* s_GameClass;
        const int s_MaxState;
        MainscreenPar* s_MainscreenPar;
        bool getKeyPressed(string);
};
#endif
