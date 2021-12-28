#ifndef CREATUREMONSTER_H
#define CREATUREMONSTER_H
#include <iostream>
#include <map>

#include "Square.h"

#include "lua/lua.hpp"

class Game;

class CreatureMonster
{
    public:
        CreatureMonster(Game*, int, int, int, int, bool getstate = true);
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
        bool s_IsHighlighted;
        lua_State* s_AILuaState;
        void createLuaState();
        void live();
        bool correctionPhys(int,int, bool ladder = false);
        void draw();
        int getFrame();
        Square getCollision();
        void kill(int);
};
#endif
