#ifndef LUABINDFUNCTIONS_H
#define LUABINDFUNCTIONS_H

#include "lua/lua.hpp"

#include <math.h>

#include "CreatureMonster.h"

class Game;

class LuaBindFunctions
{
    public:
        LuaBindFunctions(Game*);
        ~LuaBindFunctions();
        Game* s_GameClass;
        CreatureMonster* s_CurrentMonster;
        void prepareAIRun();
        void registerFunctions(lua_State* s_Lua, std::string);
        void registerFunctionsMonster(lua_State* s_Lua);
        void registerFunctionsAll(lua_State* s_Lua);
        //Lua Functions All
        int addPackImagesToFactoryTemporaryImage(lua_State* s_Lua);
        int addImageToFactoryTemporaryImage(lua_State* s_Lua);
        int getMainValue(lua_State* s_Lua);
        int changeNextLevel(lua_State* s_Lua);
        int playSound(lua_State* s_Lua);
        int stopSound(lua_State* s_Lua);
        int pauseSound(lua_State* s_Lua);
        int playMusic(lua_State* s_Lua);
        int stopMusic(lua_State* s_Lua);
        int pauseMusic(lua_State* s_Lua);
        int stopAllMusic(lua_State* s_Lua);
        //Lua Functions Monsters
        int setState(lua_State* s_Lua);
        int getState(lua_State* s_Lua);
        int goLeft(lua_State* s_Lua);
        int goRight(lua_State* s_Lua);
        int goUp(lua_State* s_Lua);
        int goDown(lua_State* s_Lua);
        int setNumberOfAction(lua_State* s_Lua);
        int getNumberOfAction(lua_State* s_Lua);
        int nextNumberOfAction(lua_State* s_Lua);
        int setNullNumberOfAction(lua_State* s_Lua);
        int setAdditionalNumberOfAction(lua_State* s_Lua);
        int getAdditionalNumberOfAction(lua_State* s_Lua);
        int nextAdditionalNumberOfAction(lua_State* s_Lua);
        int setNullAdditionalNumberOfAction(lua_State* s_Lua);
        int getMonsterOption(lua_State* s_Lua);
        int testLookDaveX(lua_State* s_Lua);
        int testLookDaveY(lua_State* s_Lua);
        int getMonsterFrame(lua_State* s_Lua);
        int getDistanceToDaveX(lua_State* s_Lua);
        int getDistanceToDaveY(lua_State* s_Lua);
        int getDistanceToDave(lua_State* s_Lua);
        int testTileTypeDown(lua_State* s_Lua);
        int getDistanceToDaveXHead(lua_State* s_Lua);
        int getDistanceToDaveYHead(lua_State* s_Lua);
        int getDistanceToDaveHead(lua_State* s_Lua);
        int testCollisionDave(lua_State* s_Lua);
        int killDave(lua_State* s_Lua);
        int killMonster(lua_State* s_Lua);
        int addDuplicateMonster(lua_State* s_Lua);
        int setMonsterValue(lua_State* s_Lua);
        int getMonsterValue(lua_State* s_Lua);
        int setGlobalValue(lua_State* s_Lua);
        int getGlobalValue(lua_State* s_Lua);
        int setModSettingsValue(lua_State* s_Lua);
        int getModSettingsValue(lua_State* s_Lua);
        int getMonsterID(lua_State* s_Lua);
        int testTileTypeRight(lua_State* s_Lua);
        int testTileTypeLeft(lua_State* s_Lua);
        int testTileTypeUp(lua_State* s_Lua);
        int getCoordMonsterX(lua_State* s_Lua);
        int getCoordMonsterY(lua_State* s_Lua);
        int getCoordDaveX(lua_State* s_Lua);
        int getCoordDaveY(lua_State* s_Lua);
        int getStateDave(lua_State* s_Lua);
        int getNumberOfLives(lua_State* s_Lua);
        int setNumberOfLives(lua_State* s_Lua);
        //...
};
#endif
