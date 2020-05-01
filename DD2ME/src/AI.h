#ifndef AI_H
#define AI_H

#include "lua/lua.hpp"

#include "CreatureMonster.h"

#include "LuaBindFunctions.h"

class Game;

enum TYPE_SPECIAL_FUNCTION
{
    TSF_ON_KILL,
    TSF_ON_DAMAGE
};

struct info_for_spec_func
{
    info_for_spec_func():
        damage(0),
        killtype(0)
    {}
    int damage;
    int killtype;
};

class AI
{
    public:
        AI(Game*);
        ~AI();
        Game* s_GameClass;
        lua_State* s_AIRun;
        LuaBindFunctions* s_LuaBindFunctions;
        void createLuaState();
        void createOptions(CreatureMonster*, int, bool getstate = true);
        void runAI(CreatureMonster*);
        void runSpecialFunction(CreatureMonster*, TYPE_SPECIAL_FUNCTION, info_for_spec_func);
        bool preLuaRun(CreatureMonster*);
};
#endif
