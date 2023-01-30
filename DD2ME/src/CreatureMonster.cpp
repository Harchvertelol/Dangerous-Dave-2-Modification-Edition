#include "CreatureMonster.h"

#include "Game.h"

#include "WorkFunctions.h"
#include "Defines.h"

using namespace std;

using namespace WorkFunctions;
using namespace MathFunctions;
using namespace GameFunctions;
using namespace ConvertFunctions;

CreatureMonster::CreatureMonster(Game* gameclass, int number, int id, int x, int y, bool getstate):
    s_GameClass(gameclass),
    s_CoordX(x),
    s_CoordY(y),
    s_CurrentLives(0),
    s_ID(id),
    s_DeleteNow(false),
    s_Activated(false),
    s_State(""),
    s_Number(number),
    s_NumberOfAction(0),
    s_AdditionalNumberOfAction(0),
    s_IsAlwaysLiveInStep(false),
    s_IsHighlighted(false),
    s_AILuaState(0)
{
    createLuaState();
    s_GameClass->s_AI->createOptions(this, s_Number, getstate);
}

CreatureMonster::~CreatureMonster()
{
    if(s_AILuaState != 0) lua_close(s_AILuaState);
}

void CreatureMonster::createLuaState()
{
    if(s_AILuaState != 0) lua_close(s_AILuaState);
    s_AILuaState = luaL_newstate();
    luaL_openlibs(s_AILuaState);
    s_GameClass->s_AI->s_LuaBindFunctions->registerFunctions(s_AILuaState, "AI");
}

void CreatureMonster::live()
{
    s_GameClass->s_AI->runAI(this);
    if(s_AdditionalNumberOfAction > MAXIMAL_UNSIGNED_NUMBER) s_AdditionalNumberOfAction = 0;
    if(s_NumberOfAction > MAXIMAL_UNSIGNED_NUMBER) s_NumberOfAction = 0;
    s_IsAlwaysLiveInStep = true;
}

bool CreatureMonster::correctionPhys(int coord, int what, bool ladder)
{
    bool yes = false;
    int frame = getFrame();
    if(s_GameClass->s_IniFile->getValue("settings", "correctionphysics") == "false") return false;
    int SizeXMonster = roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[s_Number-1][s_State][frame].s_XR, 16, 1) - roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[s_Number-1][s_State][frame].s_XL, 16, -1);
    int SizeYMonster = roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[s_Number-1][s_State][frame].s_YR, 16, 1) - roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[s_Number-1][s_State][frame].s_YL, 16, -1);
    SizeXMonster = SizeXMonster/16 + 1;
    SizeYMonster = SizeYMonster/16 + 1;
    int* TileCoordX = new int[SizeXMonster];
    int* TileCoordY = new int[SizeYMonster];
    for(int i = 0; i < SizeXMonster; i++) TileCoordX[i] = roundNumber(s_CoordX,16,-1) + i*16;
    for(int i = 0; i < SizeYMonster; i++) TileCoordY[i] = roundNumber(s_CoordY,16,-1) + i*16;
    bool col = false;
    int sign;
    if(what == 0) sign = (s_CoordX - coord)/abs(s_CoordX - coord);
    if(what == 1) sign = (s_CoordY - coord)/abs(s_CoordY - coord);
    int TileType = 0;
    do
    {
        col = false;
        for(int j = 0; j < SizeYMonster; j++)
            for(int i = 0; i < SizeXMonster; i++)
            {
                TileType = s_GameClass->s_Data->s_Level->getTileType(TileCoordX[i]/16, TileCoordY[j]/16, s_GameClass->s_Data->s_Level->getNumberPhysicTilesField());
                if( TileType == IMPASSABLE && testCollision(s_CoordX, s_CoordY, TileCoordX[i], TileCoordY[j], s_GameClass->s_Data->s_Monsters->s_Collisions[s_Number - 1][s_State][frame], Square(0,0,15,15)) ) col = true;
                if(true)
                {
                    if( TileType == LADDER && ladder == true && testCollision(s_CoordX, s_CoordY, TileCoordX[i], TileCoordY[j], s_GameClass->s_Data->s_Monsters->s_Collisions[s_Number - 1][s_State][frame], Square(0,0,15,15)) ) col = true;
                    if( what == 1 && sign == 1 && TileType == LADDER &&
                       testCollision(s_CoordX, s_CoordY, TileCoordX[i], TileCoordY[j],
                                     s_GameClass->s_Data->s_Monsters->s_Collisions[s_Number - 1][s_State][frame],
                                     Square(0,0,15,15)) &&
                       /* !testCollision(s_CoordX, coord, TileCoordX[i], TileCoordY[j],
                                      s_GameClass->s_Data->s_Monsters->s_Collisions[s_Number - 1][s_State][frame],
                                      Square(0,0,15,15), true) &&*/
                       !testCollision(s_CoordX, coord, TileCoordX[i], TileCoordY[j],
                                      s_GameClass->s_Data->s_Monsters->s_Collisions[s_Number - 1][s_State][frame],
                                      Square(0,0,15,15)) ) col = true;
                }
            }
        if(what == 0) s_CoordX -= sign;
        if(what == 1) s_CoordY -= sign;
        if(col == true) yes = true;
    }
    while(col == true);
    if(what == 0) s_CoordX += sign;
    if(what == 1) s_CoordY += sign;
    if(yes && s_GameClass->s_IniFile->getValue("loggers","correctionphysics") == "true")
    {
        if(what == 0) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "(monster) Physics correction X: old - " + WorkFunctions::ConvertFunctions::itos(coord) + ", new - " + WorkFunctions::ConvertFunctions::itos(s_CoordX));
        else if(what == 1) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "(monster) Physics correction Y: old - " + WorkFunctions::ConvertFunctions::itos(coord) + ", new - " + WorkFunctions::ConvertFunctions::itos(s_CoordY));
    }
    delete[] TileCoordX;
    delete[] TileCoordY;
    return yes;
}

void CreatureMonster::draw()
{
    int frame = getFrame();
    s_GameClass->s_Data->s_Monsters->drawMonster(s_Number, s_State, frame, s_CoordX - s_GameClass->s_GameInfo->s_ScreenCoordX, s_CoordY - s_GameClass->s_GameInfo->s_ScreenCoordY, s_IsHighlighted);
}

int CreatureMonster::getFrame()
{
    if(!s_GameClass->s_Data->s_Monsters->s_MonstersInfo[s_Number - 1]->isExists("info", "numberofframes" + s_State))
    {
        s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Does not exists monster state: numberofframes" + s_State);
        return 0;
    }
    int numberofframes = atoi( s_GameClass->s_Data->s_Monsters->s_MonstersInfo[s_Number - 1]->getValue("info", "numberofframes" + s_State).c_str() );
    return s_NumberOfAction%numberofframes;
}

void CreatureMonster::kill(int type)
{
    info_for_spec_func infofsf;
    infofsf.killtype = type;
    s_GameClass->s_AI->runSpecialFunction(this, TSF_ON_KILL, infofsf);
    s_DeleteNow = true;
}

Square CreatureMonster::getCollision()
{
    return s_GameClass->s_Data->s_Monsters->s_Collisions[s_Number - 1][s_State][getFrame()];
}
