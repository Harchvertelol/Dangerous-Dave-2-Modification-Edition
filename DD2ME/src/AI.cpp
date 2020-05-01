#include "AI.h"

#include "Game.h"

#include "WorkFunction.h"
#include "Defines.h"

using namespace WorkFunction;
using namespace MathFunction;
using namespace GameFunction;
using namespace ConvertFunction;

AI::AI(Game* gameclass):
    s_GameClass(gameclass),
    s_AIRun(0)
{
    s_LuaBindFunctions = new LuaBindFunctions(gameclass);
    createLuaState();
}

AI::~AI()
{
    if(s_LuaBindFunctions != 0) delete s_LuaBindFunctions;
    if(s_AIRun != 0) lua_close(s_AIRun);
}

void AI::createLuaState()
{
    if(s_AIRun != 0) lua_close(s_AIRun);
    s_AIRun = luaL_newstate();
    luaL_openlibs(s_AIRun);
    s_LuaBindFunctions->registerFunctions(s_AIRun, "AI");
}

void AI::createOptions(CreatureMonster* monster, int number, bool getstate)
{
    lua_settop(s_AIRun, 0);
    monster->s_CurrentLives = atoi( s_GameClass->s_Data->s_Monsters->s_MonstersInfo[number - 1]->getValue("options","lives").c_str() );
    if(getstate == false) return;
    if(!preLuaRun(monster)) cout<<"Error! Intellect Lua file is missing!"<<endl;
    else
    {
        lua_getglobal(s_AIRun, "setFirstState");
        lua_call(s_AIRun, 0, 1);
        monster->s_State = lua_tostring(s_AIRun, -1);
    }
}

void AI::runAI(CreatureMonster* monster)
{
    if(!preLuaRun(monster)) return;
    s_LuaBindFunctions->prepareAIRun();
    lua_getglobal(s_AIRun, "mainFunc");
    lua_call(s_AIRun, 0, 0);
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
        lua_getglobal(s_AIRun, specfunc.c_str());
        lua_pushnumber(s_AIRun, ifsf.killtype);
    }
    else if(tsf == TSF_ON_DAMAGE)
    {
        specfunc = "onDamage";
        narg = 1;
        lua_getglobal(s_AIRun, specfunc.c_str());
        lua_pushnumber(s_AIRun, ifsf.damage);
    }
    lua_call(s_AIRun, narg, retres);
}

bool AI::preLuaRun(CreatureMonster* monster)
{
    if(s_GameClass->s_GameInfo->s_AIOn == false) return false;
    s_LuaBindFunctions->s_CurrentMonster = monster;
    luaL_dostring(s_AIRun, "function mainFunc()\nprint(\"Error: mainFunc is missed!\")\nend\nfunction setFirstState()\nprint(\"Error: setFirstState is missed!\")\nreturn \"errorstate\"\nend\nfunction onKill(type)\nend\nfunction onDamage(damage)\nend\n");
    if(luaL_dofile(s_AIRun, (s_GameClass->s_Data->PathToMonsterPack + WorkFunction::ConvertFunction::itos(monster->s_Number) + "/intellect.lua").c_str()))
    {
        cout<<"Error! Intellect Lua file is missing!"<<endl;
        return false;
    }
    return true;
}
