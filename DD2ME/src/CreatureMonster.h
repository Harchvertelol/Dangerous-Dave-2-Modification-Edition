#ifndef CREATUREMONSTER_H
#define CREATUREMONSTER_H
#include <iostream>
#include <map>

#include "lua/lua.hpp"

class Game;

class CreatureMonster
{
    public:
        CreatureMonster(Game*, int, int, bool getstate = true);
        ~CreatureMonster();
        Game* s_GameClass;
        int s_CoordX;
        int s_CoordY;
        int s_CurrentLives;
        unsigned int s_ID;
        bool s_DeleteNow;
        bool s_Activated;
        std::string s_State;
        int s_Number;
        unsigned int s_NumberOfAction;
        unsigned int s_AdditionalNumberOfAction;
        std::map<std::string, std::string> s_AIMonsterValues;
        bool s_IsAlwaysLiveInStep;
        lua_State* s_AILuaState;
        void createLuaState();
        void live();
        bool correctionPhys(int,int, bool ladder = false);
        void draw();
        int getFrame();
        void kill(int);
};
#endif
