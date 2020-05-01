#ifndef CREATUREMONSTER_H
#define CREATUREMONSTER_H
#include <iostream>
#include <map>
using namespace std;

class Game;

class CreatureMonster
{
    public:
        CreatureMonster(Game*,int,bool getstate = true);
        ~CreatureMonster();
        Game* s_GameClass;
        int s_CoordX;
        int s_CoordY;
        int s_CurrentLives;
        bool s_DeleteNow;
        string s_State;
        int s_Number;
        unsigned int s_NumberOfAction;
        unsigned int s_AdditionalNumberOfAction;
        map<string, string> s_GlobalValues;
        bool s_IsAlwaysLiveInStep;
        void live();
        bool correctionPhys(int,int, bool ladder = false);
        void draw();
        int getFrame();
        void kill(int);
};
#endif
