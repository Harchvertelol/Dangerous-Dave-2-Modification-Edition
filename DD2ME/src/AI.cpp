#include "AI.h"

#include "Game.h"

#include "WorkFunctions.h"
#include "Defines.h"

using namespace WorkFunctions;
using namespace MathFunctions;
using namespace GameFunctions;
using namespace ConvertFunctions;

using namespace std;

AI::AI(Game* gameclass):
    s_GameClass(gameclass)
{
    s_LuaBindFunctions = new LuaBindFunctions(gameclass);
}

AI::~AI()
{
    if(s_LuaBindFunctions != 0) delete s_LuaBindFunctions;
}

void AI::createOptions(CreatureMonster* monster, int number, bool getstate)
{
    if(!monster->s_AILuaState)
    {
        s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Lua state of monster not created!", true);
        return;
    }
    s_LuaBindFunctions->prepareAIRun();
    lua_settop(monster->s_AILuaState, 0);
    monster->s_CurrentLives = atoi( s_GameClass->s_Data->s_Monsters->s_MonstersInfo[number - 1]->getValue("options", "lives").c_str() );
    if(!preLuaRun(monster, false)) return;
    if(!loadAI(monster)) return;
    if(getstate == false) return;
    else
    {
        lua_getglobal(monster->s_AILuaState, "setFirstState");
        lua_call(monster->s_AILuaState, 0, 1);
        monster->s_State = lua_tostring(monster->s_AILuaState, -1);
    }
}

void AI::runAI(CreatureMonster* monster)
{
    if(!preLuaRun(monster)) return;
    s_LuaBindFunctions->prepareAIRun();
    lua_getglobal(monster->s_AILuaState, "mainFunc");
    lua_call(monster->s_AILuaState, 0, 0);
}

void AI::runSpecialFunction(CreatureMonster* monster, TYPE_SPECIAL_FUNCTION tsf, info_for_spec_func ifsf)
{
    if(!preLuaRun(monster)) return;
    string specfunc = "";
    int narg = 0, retres = 0;
    if(tsf == TSF_ON_KILL)
    {
        specfunc = "onKill";
        narg = 1;
        lua_getglobal(monster->s_AILuaState, specfunc.c_str());
        lua_pushnumber(monster->s_AILuaState, ifsf.killtype);
    }
    else if(tsf == TSF_ON_DAMAGE)
    {
        specfunc = "onDamage";
        narg = 1;
        lua_getglobal(monster->s_AILuaState, specfunc.c_str());
        lua_pushnumber(monster->s_AILuaState, ifsf.damage);
    }
    lua_call(monster->s_AILuaState, narg, retres);
}

bool AI::preLuaRun(CreatureMonster* monster, bool testAIon)
{
    if(testAIon && !s_GameClass->s_GameInfo->s_IsAIOn) return false;
    s_LuaBindFunctions->s_CurrentMonster = monster;
    return true;
}

bool AI::loadAI(CreatureMonster* monster)
{
    luaL_dostring(monster->s_AILuaState, "math.randomseed(os.time())");
    luaL_dostring(monster->s_AILuaState, ("function mainFunc()\nprint(\"Error: mainFunc is missed!\")\nend\nfunction setFirstState()\nprint(\"Error: setFirstState is missed!\")\nreturn \"errorstate\"\nend\nfunction onKill(type)\nend\nfunction onDamage(damage)\nend\nfunction getAPIVersion()\nreturn " + WorkFunctions::ConvertFunctions::ftos(NUMBER_CONSTANTS::NC_API_VERSION) + "\nend\n").c_str());
    if(luaL_dofile(monster->s_AILuaState, (s_GameClass->s_Data->PathToMonsterPack + WorkFunctions::ConvertFunctions::itos(monster->s_Number) + "/intellect.lua").c_str()))
    {
        s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Intellect Lua file is missing!", true);
        return false;
    }
    lua_getglobal(monster->s_AILuaState, "getAPIVersion");
    lua_call(monster->s_AILuaState, 0, 1);
    float tmp_cur_mon_api_version = lua_tonumber(monster->s_AILuaState, -1);
    if(tmp_cur_mon_api_version < NUMBER_CONSTANTS::NC_API_VERSION) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_WARNING, "Warning: monster with number " + WorkFunctions::ConvertFunctions::itos(monster->s_Number) + " has an API version below the current one!");
    return true;
}
