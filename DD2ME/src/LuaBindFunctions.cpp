#include "LuaBindFunctions.h"

#include "Game.h"

#include "WorkFunctions.h"
#include "Defines.h"

#include "IniParser/ParserInfoFile.h"

LuaBindFunctions::LuaBindFunctions(Game* gameclass):
    s_GameClass(gameclass),
    s_CurrentMonster(0)
{
    //...
}

LuaBindFunctions::~LuaBindFunctions()
{
    //...
}

static LuaBindFunctions* s_LBF = NULL;

static int __setState(lua_State* s_Lua)
{
    return s_LBF->setState(s_Lua);
}

int LuaBindFunctions::setState(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 2)
    {
        cout<<"Error! Number of arguments of function \"setState\" is incorrect!"<<endl;
        return 0;
    }
    if(!lua_isstring(s_Lua, 2))
    {
        cout<<"Error! Second argument of function \"setState\" is not string!"<<endl;
        return 0;
    }
    //...
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    //...
    mnst->s_State = lua_tostring(s_Lua, 2);
    return 0;
}

static int __getState(lua_State* s_Lua)
{
    return s_LBF->getState(s_Lua);
}

int LuaBindFunctions::getState(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1)
    {
        cout<<"Error! Number of arguments of function \"getState\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    lua_pushstring(s_Lua, mnst->s_State.c_str());
    return 1;
}

static int __goLeft(lua_State* s_Lua)
{
    return s_LBF->goLeft(s_Lua);
}

int LuaBindFunctions::goLeft(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 3 && n != 4 && n != 5)
    {
        cout<<"Error! Number of arguments of function \"goLeft\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    int shift = lua_tonumber(s_Lua, 2);
    if(shift == 0)
    {
        lua_pushnumber(s_Lua, 0);
        return 1;
    }
    bool correct = (bool)lua_tonumber(s_Lua, 3);
    bool correctStand = false;
    if(n >= 4) correctStand = (bool)lua_tonumber(s_Lua, 4);
    bool onCeil = false;
    if(n >= 5) onCeil = (bool)lua_tonumber(s_Lua, 5);
    bool isCorrect = false;
    int frame = mnst->getFrame();
    const int widthm = s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR - s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL;
    int shf = 0;
    int typeCorrect = 0;
    for(int curshift = 0; curshift < shift && !isCorrect; curshift += widthm)
    {
        shf = shift - curshift;
        if(shf > widthm) shf = widthm;
        mnst->s_CoordX -= shf;
        if(correct == true)
        {
            isCorrect = mnst->correctionPhys(mnst->s_CoordX + shf, 0);
            typeCorrect = 1;
        }
        if(correctStand == true)
        {
            int SizeXMonsterPix = WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR, 16, 1) - WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL, 16, -1);
            int SizeYMonsterPix = WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR, 16, 1) - WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL, 16, -1);
            int SizeXMonster = SizeXMonsterPix/16 + 1;
            int SizeYMonster = SizeYMonsterPix/16 + 1;
            int** TileCoordX = new int* [SizeXMonster];
            int** TileCoordY = new int* [SizeXMonster];
            for(int i = 0; i < SizeXMonster; i++) TileCoordX[i] = new int[SizeYMonster];
            for(int i = 0; i < SizeXMonster; i++) TileCoordY[i] = new int[SizeYMonster];
            for(int i = 0; i < SizeXMonster; i++)
                for(int j = 0; j < SizeYMonster; j++)
                {
                    TileCoordX[i][j] = WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordX,16,-1) + i*16;
                    TileCoordY[i][j] = WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordY,16,-1) + j*16;
                }
            int OldCoordXLeftTile = WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordX + shf,16,-1);
            int NumberLabel;
            int MoveCalc = 0;
            string where_col_stand = "up";
            if(onCeil == false) NumberLabel = SizeYMonster - 1;
            else
            {
                where_col_stand = "down";
                MoveCalc = -1;
                NumberLabel = 0;
            }
            for(int i = 0; (i < SizeXMonster) && (TileCoordX[i][NumberLabel] <= OldCoordXLeftTile); i++)
                if(WorkFunctions::GameFunctions::testCollision(mnst->s_CoordX, mnst->s_CoordY, TileCoordX[i][NumberLabel], TileCoordY[SizeXMonster - 1][NumberLabel] + MoveCalc*16, s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number - 1][mnst->s_State][mnst->getFrame()], Square(0,0,15,15), true, where_col_stand))
                {
                    if(s_GameClass->s_Data->s_Level->getTileType(TileCoordX[i][NumberLabel]/16, TileCoordY[SizeXMonster - 1][NumberLabel]/16 + MoveCalc) == EMPTY
                       || s_GameClass->s_Data->s_Level->getTileType(TileCoordX[i][NumberLabel]/16, TileCoordY[SizeXMonster - 1][NumberLabel]/16 + MoveCalc) == DEATH
                       || s_GameClass->s_Data->s_Level->getTileType(TileCoordX[i][NumberLabel]/16, TileCoordY[SizeXMonster - 1][NumberLabel]/16 + MoveCalc) == EXITLEVEL)
                    {
                        mnst->s_CoordX += shf;
                        isCorrect = true;
                        typeCorrect = 2;
                    }
                    else if(onCeil == true && s_GameClass->s_Data->s_Level->getTileType(TileCoordX[i][NumberLabel]/16, TileCoordY[SizeXMonster - 1][NumberLabel]/16 + MoveCalc) == LADDER)
                    {
                        mnst->s_CoordX += shf;
                        isCorrect = true;
                        typeCorrect = 2;
                    }
                }
            for(int i = 0; i < SizeXMonster; i++)
            {
                delete[] TileCoordX[i];
                delete[] TileCoordY[i];
            }
            delete TileCoordX;
            delete TileCoordY;
        }
    }
    lua_pushnumber(s_Lua, isCorrect);
    lua_pushnumber(s_Lua, typeCorrect);
    return 2;
}

static int __goRight(lua_State* s_Lua)
{
    return s_LBF->goRight(s_Lua);
}

int LuaBindFunctions::goRight(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 3 && n != 4 && n != 5)
    {
        cout<<"Error! Number of arguments of function \"goRight\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    int shift = lua_tonumber(s_Lua, 2);
    if(shift == 0)
    {
        lua_pushnumber(s_Lua, 0);
        return 1;
    }
    bool correct = (bool)lua_tonumber(s_Lua, 3);
    bool correctStand = false;
    if(n >= 4) correctStand = (bool)lua_tonumber(s_Lua, 4);
    bool onCeil = false;
    if(n >= 5) onCeil = (bool)lua_tonumber(s_Lua, 5);
    bool isCorrect = false;
    int frame = mnst->getFrame();
    const int widthm = s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR - s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL;
    int shf = 0;
    int typeCorrect = 0;
    for(int curshift = 0; curshift < shift && !isCorrect; curshift += widthm)
    {
        shf = shift - curshift;
        if(shf > widthm) shf = widthm;
        mnst->s_CoordX += shf;
        if(correct == true)
        {
            isCorrect = mnst->correctionPhys(mnst->s_CoordX - shf, 0);
            typeCorrect = 1;
        }
        if(correctStand == true)
        {
            int SizeXMonsterPix = WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR, 16, 1) - WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL, 16, -1);
            int SizeYMonsterPix = WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR, 16, 1) - WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL, 16, -1);
            int SizeXMonster = SizeXMonsterPix/16 + 1;
            int SizeYMonster = SizeYMonsterPix/16 + 1;
            int** TileCoordX = new int* [SizeXMonster];
            int** TileCoordY = new int* [SizeXMonster];
            for(int i = 0; i < SizeXMonster; i++) TileCoordX[i] = new int[SizeYMonster];
            for(int i = 0; i < SizeXMonster; i++) TileCoordY[i] = new int[SizeYMonster];
            for(int i = 0; i < SizeXMonster; i++)
                for(int j = 0; j < SizeYMonster; j++)
                {
                    TileCoordX[i][j] = WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordX,16,-1) + i*16;
                    TileCoordY[i][j] = WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordY,16,-1) + j*16;
                }
            int OldCoordXRightTile = WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordX - shf,16,-1) + (SizeXMonster - 1)*16;
            int NumberLabel;
            int MoveCalc = 0;
            string where_col_stand = "up";
            if(onCeil == false) NumberLabel = SizeYMonster - 1;
            else
            {
                where_col_stand = "down";
                MoveCalc = -1;
                NumberLabel = 0;
            }
            for(int i = SizeXMonster - 1; i >= 0&& TileCoordX[i][NumberLabel] >= OldCoordXRightTile; i--)
                if(WorkFunctions::GameFunctions::testCollision(mnst->s_CoordX, mnst->s_CoordY, TileCoordX[i][NumberLabel], TileCoordY[SizeXMonster - 1][NumberLabel] + MoveCalc*16, s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number - 1][mnst->s_State][mnst->getFrame()], Square(0,0,15,15), true, where_col_stand))
                {
                    if(s_GameClass->s_Data->s_Level->getTileType(TileCoordX[i][NumberLabel]/16, TileCoordY[SizeXMonster - 1][NumberLabel]/16 + MoveCalc) == EMPTY
                       || s_GameClass->s_Data->s_Level->getTileType(TileCoordX[i][NumberLabel]/16, TileCoordY[SizeXMonster - 1][NumberLabel]/16 + MoveCalc) == DEATH
                       || s_GameClass->s_Data->s_Level->getTileType(TileCoordX[i][NumberLabel]/16, TileCoordY[SizeXMonster - 1][NumberLabel]/16 + MoveCalc) == EXITLEVEL)
                    {
                        mnst->s_CoordX -= shf;
                        isCorrect = true;
                        typeCorrect = 2;
                    }
                    else if(onCeil == true && s_GameClass->s_Data->s_Level->getTileType(TileCoordX[i][NumberLabel]/16, TileCoordY[SizeXMonster - 1][NumberLabel]/16 + MoveCalc) == LADDER)
                    {
                        mnst->s_CoordX -= shf;
                        isCorrect = true;
                        typeCorrect = 2;
                    }
                }
            for(int i = 0; i < SizeXMonster; i++)
            {
                delete[] TileCoordX[i];
                delete[] TileCoordY[i];
            }
            delete TileCoordX;
            delete TileCoordY;
        }
    }
    lua_pushnumber(s_Lua, isCorrect);
    lua_pushnumber(s_Lua, typeCorrect);
    return 2;
}

static int __goUp(lua_State* s_Lua)
{
    return s_LBF->goUp(s_Lua);
}

int LuaBindFunctions::goUp(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 3 /* && n != 4*/ )
    {
        cout<<"Error! Number of arguments of function \"goUp\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    int shift = lua_tonumber(s_Lua, 2);
    if(shift == 0)
    {
        lua_pushnumber(s_Lua, 0);
        return 1;
    }
    bool correct = (bool)lua_tonumber(s_Lua, 3);
    /*bool correctStand;
    if(n == 4) correctStand = (bool)lua_tonumber(s_Lua, 4);
    else correctStand = false;*/
    bool isCorrect = false;
    int frame = mnst->getFrame();
    const int heightm = s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR - s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL;
    int shf = 0;
    int typeCorrect = 0;
    for(int curshift = 0; curshift < shift && !isCorrect; curshift += heightm)
    {
        shf = shift - curshift;
        if(shf > heightm) shf = heightm;
        mnst->s_CoordY -= shf;
        if(correct == true)
        {
            isCorrect = mnst->correctionPhys(mnst->s_CoordY + shf, 1);
            typeCorrect = 1;
        }
    }
    /*if(correctStand == true)
    {
        int frame = mnst->getFrame();
        int SizeXMonsterPix = WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR, 16, 1) - WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL, 16, -1);
        int SizeYMonsterPix = WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR, 16, 1) - WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL, 16, -1);
        int SizeXMonster = SizeXMonsterPix/16 + 1;
        int SizeYMonster = SizeYMonsterPix/16 + 1;
        int** TileCoordX = new int* [SizeXMonster];
        int** TileCoordY = new int* [SizeXMonster];
        for(int i = 0; i < SizeXMonster; i++) TileCoordX[i] = new int[SizeYMonster];
        for(int i = 0; i < SizeXMonster; i++) TileCoordY[i] = new int[SizeYMonster];
        for(int i = 0; i < SizeXMonster; i++)
            for(int j = 0; j < SizeYMonster; j++)
            {
                TileCoordX[i][j] = WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordX,16,-1) + i*16;
                TileCoordY[i][j] = WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordY,16,-1) + j*16;
            }
        for(int i = 0; i < SizeXMonster; i++)
            if(s_GameClass->s_Data->s_Level->getTileType(TileCoordX[i][SizeYMonster - 1]/16, TileCoordY[SizeXMonster - 1][SizeYMonster - 1]/16) == EMPTY
               || s_GameClass->s_Data->s_Level->getTileType(TileCoordX[i][SizeYMonster - 1]/16, TileCoordY[SizeXMonster - 1][SizeYMonster - 1]/16) == DEATH)
            {
                mnst->s_CoordY += v;
                isCorrect = true;
                typeCorrect = 2;
            }
        for(int i = 0; i < SizeXMonster; i++)
        {
            delete[] TileCoordX[i];
            delete[] TileCoordY[i];
        }
        delete TileCoordX;
        delete TileCoordY;
    }*/
    lua_pushnumber(s_Lua, isCorrect);
    lua_pushnumber(s_Lua, typeCorrect);
    return 2;
}

static int __goDown(lua_State* s_Lua)
{
    return s_LBF->goDown(s_Lua);
}

int LuaBindFunctions::goDown(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 3 && n != 4 /* && n != 5 */ )
    {
        cout<<"Error! Number of arguments of function \"goDown\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    int shift = lua_tonumber(s_Lua, 2);
    if(shift == 0)
    {
        lua_pushnumber(s_Lua, 0);
        return 1;
    }
    bool correct = (bool)lua_tonumber(s_Lua, 3);
    /*bool correctStand;
    if(n >= 5) correctStand = (bool)lua_tonumber(s_Lua, 5);
    else correctStand = false;*/
    bool ladder;
    if(n >= 4) ladder = (bool)lua_tonumber(s_Lua, 4);
    else ladder = false;
    bool isCorrect = false;
    int frame = mnst->getFrame();
    const int heightm = s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR - s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL;
    int shf = 0;
    int typeCorrect = 0;
    for(int curshift = 0; curshift < shift && !isCorrect; curshift += heightm)
    {
        shf = shift - curshift;
        if(shf > heightm) shf = heightm;
        mnst->s_CoordY += shf;
        if(correct == true)
        {
            isCorrect = mnst->correctionPhys(mnst->s_CoordY - shf, 1, ladder);
            typeCorrect = 1;
        }
    }
    /*if(correctStand == true)
    {
        int frame = mnst->getFrame();
        int SizeXMonsterPix = WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR, 16, 1) - WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL, 16, -1);
        int SizeYMonsterPix = WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR, 16, 1) - WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL, 16, -1);
        int SizeXMonster = SizeXMonsterPix/16 + 1;
        int SizeYMonster = SizeYMonsterPix/16 + 1;
        int** TileCoordX = new int* [SizeXMonster];
        int** TileCoordY = new int* [SizeXMonster];
        for(int i = 0; i < SizeXMonster; i++) TileCoordX[i] = new int[SizeYMonster];
        for(int i = 0; i < SizeXMonster; i++) TileCoordY[i] = new int[SizeYMonster];
        for(int i = 0; i < SizeXMonster; i++)
            for(int j = 0; j < SizeYMonster; j++)
            {
                TileCoordX[i][j] = WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordX,16,-1) + i*16;
                TileCoordY[i][j] = WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordY,16,-1) + j*16;
            }
        for(int i = 0; i < SizeXMonster; i++)
            if(s_GameClass->s_Data->s_Level->getTileType(TileCoordX[i][SizeYMonster - 1]/16, TileCoordY[SizeXMonster - 1][SizeYMonster - 1]/16) == EMPTY
               || s_GameClass->s_Data->s_Level->getTileType(TileCoordX[i][SizeYMonster - 1]/16, TileCoordY[SizeXMonster - 1][SizeYMonster - 1]/16) == DEATH)
            {
                mnst->s_CoordY -= v;
                isCorrect = true;
                typeCorrect = 2;
            }
        for(int i = 0; i < SizeXMonster; i++)
        {
            delete[] TileCoordX[i];
            delete[] TileCoordY[i];
        }
        delete TileCoordX;
        delete TileCoordY;
    }*/
    lua_pushnumber(s_Lua, isCorrect);
    lua_pushnumber(s_Lua, typeCorrect);
    return 2;
}

static int __setNumberOfAction(lua_State* s_Lua)
{
    return s_LBF->setNumberOfAction(s_Lua);
}

int LuaBindFunctions::setNumberOfAction(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 2)
    {
        cout<<"Error! Number of arguments of function \"setNumberOfAction\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    int v = lua_tonumber(s_Lua, 2);
    if(s_GameClass->s_IniFile->getValue("video", "animation") == "true") mnst->s_NumberOfAction = v;
    return 0;
}

static int __nextNumberOfAction(lua_State* s_Lua)
{
    return s_LBF->nextNumberOfAction(s_Lua);
}

int LuaBindFunctions::nextNumberOfAction(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1)
    {
        cout<<"Error! Number of arguments of function \"nextNumberOfAction\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    if(s_GameClass->s_IniFile->getValue("video", "animation") == "true") mnst->s_NumberOfAction++;
    return 0;
}

static int __setNullNumberOfAction(lua_State* s_Lua)
{
    return s_LBF->setNullNumberOfAction(s_Lua);
}

int LuaBindFunctions::setNullNumberOfAction(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1)
    {
        cout<<"Error! Number of arguments of function \"setNullNumberOfAction\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    if(s_GameClass->s_IniFile->getValue("video", "animation") == "true") mnst->s_NumberOfAction = 0;
    return 0;
}

static int __getNumberOfAction(lua_State* s_Lua)
{
    return s_LBF->getNumberOfAction(s_Lua);
}

int LuaBindFunctions::getNumberOfAction(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1)
    {
        cout<<"Error! Number of arguments of function \"getNumberOfAction\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    lua_pushnumber(s_Lua, mnst->s_NumberOfAction);
    return 1;
}

static int __setAdditionalNumberOfAction(lua_State* s_Lua)
{
    return s_LBF->setAdditionalNumberOfAction(s_Lua);
}

static int __getAdditionalNumberOfAction(lua_State* s_Lua)
{
    return s_LBF->getAdditionalNumberOfAction(s_Lua);
}

static int __nextAdditionalNumberOfAction(lua_State* s_Lua)
{
    return s_LBF->nextAdditionalNumberOfAction(s_Lua);
}

static int __setNullAdditionalNumberOfAction(lua_State* s_Lua)
{
    return s_LBF->setNullAdditionalNumberOfAction(s_Lua);
}

int LuaBindFunctions::setAdditionalNumberOfAction(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 2)
    {
        cout<<"Error! Number of arguments of function \"setAdditionalNumberOfAction\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    int v = lua_tonumber(s_Lua, 2);
    if(s_GameClass->s_IniFile->getValue("video", "animation") == "true") mnst->s_AdditionalNumberOfAction = v;
    return 0;
}

int LuaBindFunctions::getAdditionalNumberOfAction(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1)
    {
        cout<<"Error! Number of arguments of function \"getAdditionalNumberOfAction\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    lua_pushnumber(s_Lua, mnst->s_AdditionalNumberOfAction);
    return 1;
}

int LuaBindFunctions::nextAdditionalNumberOfAction(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1)
    {
        cout<<"Error! Number of arguments of function \"nextAdditionalNumberOfAction\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    if(s_GameClass->s_IniFile->getValue("video", "animation") == "true") mnst->s_AdditionalNumberOfAction++;
    return 0;
}

int LuaBindFunctions::setNullAdditionalNumberOfAction(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1)
    {
        cout<<"Error! Number of arguments of function \"setNullAdditionalNumberOfAction\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    if(s_GameClass->s_IniFile->getValue("video", "animation") == "true") mnst->s_AdditionalNumberOfAction = 0;
    return 0;
}

static int __getMonsterOption(lua_State* s_Lua)
{
    return s_LBF->getMonsterOption(s_Lua);
}

int LuaBindFunctions::getMonsterOption(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 3)
    {
        cout<<"Error! Number of arguments of function \"getMonsterOption\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    string s1 = lua_tostring(s_Lua, 2);
    string s2 = lua_tostring(s_Lua, 3);
    lua_pushstring(s_Lua, s_GameClass->s_Data->s_Monsters->s_MonstersInfo[mnst->s_Number - 1]->getValue(s1, s2).c_str());
    return 1;
}

static int __getMainValue(lua_State* s_Lua)
{
    return s_LBF->getMainValue(s_Lua);
}

int LuaBindFunctions::getMainValue(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 2)
    {
        cout<<"Error! Number of arguments of function \"getMainValue\" is incorrect!"<<endl;
        return 0;
    }
    string s1 = lua_tostring(s_Lua, 1);
    string s2 = lua_tostring(s_Lua, 2);
    lua_pushstring(s_Lua, s_GameClass->s_IniFile->getValue(s1, s2).c_str());
    return 1;
}

static int __testLookDaveX(lua_State* s_Lua)
{
    return s_LBF->testLookDaveX(s_Lua);
}

static int __testLookDaveY(lua_State* s_Lua)
{
    return s_LBF->testLookDaveY(s_Lua);
}

int LuaBindFunctions::testLookDaveX(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1)
    {
        cout<<"Error! Number of arguments of function \"testLookDaveX\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    int frame = mnst->getFrame();
    int SizeXMonster = WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR, 16, 1) - WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL, 16, -1);
    int SizeYMonster = WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR, 16, 1) - WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL, 16, -1);
    int SizeYMonsterPix = SizeYMonster;
    int CoordY1, CoordY2;
    CoordY1 = s_GameClass->s_GameInfo->s_MyDave->s_CoordY;
    CoordY2 = s_GameClass->s_GameInfo->s_MyDave->s_CoordY + 32;
    int CoordX = s_GameClass->s_GameInfo->s_MyDave->s_CoordX;
    if( ( (CoordY1 >= mnst->s_CoordY && CoordY1 <= mnst->s_CoordY + SizeYMonsterPix) ||
        (CoordY2 >= mnst->s_CoordY && CoordY2 <= mnst->s_CoordY + SizeYMonsterPix) ) )
    {
        if(CoordX - mnst->s_CoordX != 0) lua_pushnumber(s_Lua, (CoordX - mnst->s_CoordX)/abs(CoordX - mnst->s_CoordX));
        else lua_pushnumber(s_Lua, 1);
        return 1;
    }
    lua_pushnumber(s_Lua, 0);
    return 1;
}

int LuaBindFunctions::testLookDaveY(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1)
    {
        cout<<"Error! Number of arguments of function \"testLookDaveY\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    int frame = mnst->getFrame();
    int SizeXMonster = WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR, 16, 1) - WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL, 16, -1);
    int SizeYMonster = WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR, 16, 1) - WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL, 16, -1);
    int SizeXMonsterPix = SizeXMonster;
    int CoordX1, CoordX2;
    CoordX1 = s_GameClass->s_GameInfo->s_MyDave->s_CoordX;
    CoordX2 = s_GameClass->s_GameInfo->s_MyDave->s_CoordX + 16;
    int CoordY = s_GameClass->s_GameInfo->s_MyDave->s_CoordY;
    if( ( (CoordX1 >= mnst->s_CoordX && CoordX1 <= mnst->s_CoordX + SizeXMonsterPix) ||
        (CoordX2 >= mnst->s_CoordX && CoordX2 <= mnst->s_CoordX + SizeXMonsterPix) ) )
    {
        if(CoordY - mnst->s_CoordY != 0) lua_pushnumber(s_Lua, (CoordY - mnst->s_CoordY)/abs(CoordY - mnst->s_CoordY));
        else lua_pushnumber(s_Lua, 1);
        return 1;
    }
    lua_pushnumber(s_Lua, 0);
    return 1;
}

static int __getMonsterFrame(lua_State* s_Lua)
{
    return s_LBF->getMonsterFrame(s_Lua);
}

int LuaBindFunctions::getMonsterFrame(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1)
    {
        cout<<"Error! Number of arguments of function \"getMonsterFrame\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    lua_pushnumber(s_Lua, mnst->getFrame());
    return 1;
}

static int __getDistanceToDaveX(lua_State* s_Lua)
{
    return s_LBF->getDistanceToDaveX(s_Lua);
}

static int __getDistanceToDaveY(lua_State* s_Lua)
{
    return s_LBF->getDistanceToDaveY(s_Lua);
}

static int __getDistanceToDave(lua_State* s_Lua)
{
    return s_LBF->getDistanceToDave(s_Lua);
}

int LuaBindFunctions::getDistanceToDaveX(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1 && n != 2)
    {
        cout<<"Error! Number of arguments of function \"getDistanceToDaveX\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    bool sign = false;
    if(n == 2) sign = (bool)lua_tonumber(s_Lua, 2);
    int dist = 0;
    int frame = mnst->getFrame();
    int x[4], y[4], xd[4], yd[4], distances[16];
    x[0] = mnst->s_CoordX + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR;
    y[0] = mnst->s_CoordY + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR;
    x[1] = mnst->s_CoordX + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL;
    y[1] = mnst->s_CoordY + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR;
    x[2] = mnst->s_CoordX + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL;
    y[2] = mnst->s_CoordY + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL;
    x[3] = mnst->s_CoordX + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR;
    y[3] = mnst->s_CoordY + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL;
    frame = s_GameClass->s_GameInfo->s_MyDave->getFrame();
    xd[0] = s_GameClass->s_GameInfo->s_MyDave->s_CoordX + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_XR;
    yd[0] = s_GameClass->s_GameInfo->s_MyDave->s_CoordY + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_YR;
    xd[1] = s_GameClass->s_GameInfo->s_MyDave->s_CoordX + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_XL;
    yd[1] = s_GameClass->s_GameInfo->s_MyDave->s_CoordY + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_YR;
    xd[2] = s_GameClass->s_GameInfo->s_MyDave->s_CoordX + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_XL;
    yd[2] = s_GameClass->s_GameInfo->s_MyDave->s_CoordY + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_YL;
    xd[3] = s_GameClass->s_GameInfo->s_MyDave->s_CoordX + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_XR;
    yd[3] = s_GameClass->s_GameInfo->s_MyDave->s_CoordY + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_YL;
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            distances[i*4 + j] = xd[i] - x[j];
    int a = 0;
    for(int i = 0; i < 16; i++) if(abs(distances[a]) > abs(distances[i])) a = i;
    dist = distances[a];
    if(sign == false) dist = abs(dist);
    lua_pushnumber(s_Lua, dist);
    return 1;
}

int LuaBindFunctions::getDistanceToDaveY(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1 && n != 2)
    {
        cout<<"Error! Number of arguments of function \"getDistanceToDaveY\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    bool sign = false;
    if(n == 2) sign = (bool)lua_tonumber(s_Lua, 2);
    int dist = 0;
    int frame = mnst->getFrame();
    int x[4], y[4], xd[4], yd[4], distances[16];
    x[0] = mnst->s_CoordX + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR;
    y[0] = mnst->s_CoordY + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR;
    x[1] = mnst->s_CoordX + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL;
    y[1] = mnst->s_CoordY + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR;
    x[2] = mnst->s_CoordX + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL;
    y[2] = mnst->s_CoordY + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL;
    x[3] = mnst->s_CoordX + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR;
    y[3] = mnst->s_CoordY + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL;
    frame = s_GameClass->s_GameInfo->s_MyDave->getFrame();
    xd[0] = s_GameClass->s_GameInfo->s_MyDave->s_CoordX + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_XR;
    yd[0] = s_GameClass->s_GameInfo->s_MyDave->s_CoordY + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_YR;
    xd[1] = s_GameClass->s_GameInfo->s_MyDave->s_CoordX + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_XL;
    yd[1] = s_GameClass->s_GameInfo->s_MyDave->s_CoordY + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_YR;
    xd[2] = s_GameClass->s_GameInfo->s_MyDave->s_CoordX + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_XL;
    yd[2] = s_GameClass->s_GameInfo->s_MyDave->s_CoordY + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_YL;
    xd[3] = s_GameClass->s_GameInfo->s_MyDave->s_CoordX + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_XR;
    yd[3] = s_GameClass->s_GameInfo->s_MyDave->s_CoordY + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_YL;
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            distances[i*4 + j] = yd[i] - y[j];
    int a = 0;
    for(int i = 0; i < 16; i++) if(abs(distances[a]) > abs(distances[i])) a = i;
    dist = distances[a];
    if(sign == false) dist = abs(dist);
    lua_pushinteger(s_Lua, dist);
    return 1;
}

int LuaBindFunctions::getDistanceToDave(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1)
    {
        cout<<"Error! Number of arguments of function \"getDistanceToDave\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    int dist = 0;
    int frame = mnst->getFrame();
    int x[4], y[4], xd[4], yd[4], distances[16];
    x[0] = mnst->s_CoordX + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR;
    y[0] = mnst->s_CoordY + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR;
    x[1] = mnst->s_CoordX + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL;
    y[1] = mnst->s_CoordY + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR;
    x[2] = mnst->s_CoordX + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL;
    y[2] = mnst->s_CoordY + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL;
    x[3] = mnst->s_CoordX + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR;
    y[3] = mnst->s_CoordY + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL;
    frame = s_GameClass->s_GameInfo->s_MyDave->getFrame();
    xd[0] = s_GameClass->s_GameInfo->s_MyDave->s_CoordX + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_XR;
    yd[0] = s_GameClass->s_GameInfo->s_MyDave->s_CoordY + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_YR;
    xd[1] = s_GameClass->s_GameInfo->s_MyDave->s_CoordX + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_XL;
    yd[1] = s_GameClass->s_GameInfo->s_MyDave->s_CoordY + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_YR;
    xd[2] = s_GameClass->s_GameInfo->s_MyDave->s_CoordX + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_XL;
    yd[2] = s_GameClass->s_GameInfo->s_MyDave->s_CoordY + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_YL;
    xd[3] = s_GameClass->s_GameInfo->s_MyDave->s_CoordX + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_XR;
    yd[3] = s_GameClass->s_GameInfo->s_MyDave->s_CoordY + s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frame].s_YL;
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            distances[i*4 + j] = sqrt( (x[i] - xd[j])*(x[i] - xd[j]) + (y[i] - yd[j])*(y[i] - yd[j]) );
    int a = 0;
    for(int i = 0; i < 16; i++) if(distances[a] > distances[i]) a = i;
    dist = distances[a];
    lua_pushnumber(s_Lua, dist);
    return 1;
}

static int __testTileTypeDown(lua_State* s_Lua)
{
    return s_LBF->testTileTypeDown(s_Lua);
}

int LuaBindFunctions::testTileTypeDown(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 3 && n != 4)
    {
        cout<<"Error! Number of arguments of function \"testTileTypeDown\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    string tstTileType = lua_tostring(s_Lua, 2);
    int typeTest = lua_tonumber(s_Lua, 3);
    int numberOfLabel = 0;
    if(n == 4) numberOfLabel = lua_tonumber(s_Lua, 4);
    bool testTrue = true;
    if(typeTest == 1) testTrue = false;
    int testTileType = -1;
    if(tstTileType == "EMPTY") testTileType = EMPTY;
    if(tstTileType == "IMPASSABLE") testTileType = IMPASSABLE;
    if(tstTileType == "LADDER") testTileType = LADDER;
    if(tstTileType == "DEATH") testTileType = DEATH;
    if(tstTileType == "EXITLEVEL") testTileType = EXITLEVEL;
    int frame = mnst->getFrame();

    int SizeXMonsterPix = WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR, 16, 1) - WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL, 16, -1);
    int SizeYMonsterPix = WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR, 16, 1) - WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL, 16, -1);
    int SizeXMonster = SizeXMonsterPix/16 + 1;
    int SizeYMonster = SizeYMonsterPix/16 + 1;

    /*int SizeXMonsterPix = s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR - s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL;
    int SizeYMonsterPix = s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR - s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL;
    int SizeXMonster = (WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordX + SizeXMonsterPix, 16, 1) - WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordX, 16, -1)) / 16;
    int SizeYMonster = (WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordY + SizeYMonsterPix, 16, 1) - WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordY, 16, -1)) / 16;*/

    int** TileCoordX = new int* [SizeXMonster];
    int** TileCoordY = new int* [SizeXMonster];
    for(int i = 0; i < SizeXMonster; i++) TileCoordX[i] = new int[SizeYMonster];
    for(int i = 0; i < SizeXMonster; i++) TileCoordY[i] = new int[SizeYMonster];
    for(int i = 0; i < SizeXMonster; i++)
        for(int j = 0; j < SizeYMonster; j++)
        {
            TileCoordX[i][j] = WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordX,16,-1) + i*16;
            TileCoordY[i][j] = WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordY,16,-1) + j*16;
        }
    for(int i = 0; i < SizeXMonster; i++)
    {
        if(WorkFunctions::GameFunctions::testCollision(mnst->s_CoordX, mnst->s_CoordY, TileCoordX[i][SizeYMonster - 1], TileCoordY[i][SizeYMonster - 1], s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number - 1][mnst->s_State][mnst->getFrame()], Square(0,0,15,15), true))
        {
            if(typeTest == 0 && s_GameClass->s_Data->s_Level->getTileType(TileCoordX[i][SizeYMonster - 1]/16, TileCoordY[i][SizeYMonster - 1]/16 + numberOfLabel) != testTileType) testTrue = false;
            else if(typeTest == 1 && s_GameClass->s_Data->s_Level->getTileType(TileCoordX[i][SizeYMonster - 1]/16, TileCoordY[i][SizeYMonster - 1]/16 + numberOfLabel) == testTileType) testTrue = true;
        }
        //s_GameClass->s_Data->s_Level->s_Fields["FieldTiles"][(TileCoordY[i][SizeYMonster - 1]/16 + numberOfLabel)*atoi( ( s_GameClass->s_Data->s_Level->s_Params->getValue("info", "sizeX") ).c_str() ) + TileCoordX[i][SizeYMonster - 1]/16] = 0;
    }
    for(int i = 0; i < SizeXMonster; i++)
    {
        delete[] TileCoordX[i];
        delete[] TileCoordY[i];
    }
    delete TileCoordX;
    delete TileCoordY;
    lua_pushnumber(s_Lua, testTrue);
    return 1;
}

static int __getDistanceToDaveXHead(lua_State* s_Lua)
{
    return s_LBF->getDistanceToDaveXHead(s_Lua);
}

static int __getDistanceToDaveYHead(lua_State* s_Lua)
{
    return s_LBF->getDistanceToDaveYHead(s_Lua);
}

static int __getDistanceToDaveHead(lua_State* s_Lua)
{
    return s_LBF->getDistanceToDaveHead(s_Lua);
}

int LuaBindFunctions::getDistanceToDaveXHead(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1 && n != 2)
    {
        cout<<"Error! Number of arguments of function \"getDistanceToDaveXHead\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    bool sign = false;
    if(n == 2) sign = (bool)lua_tonumber(s_Lua, 2);
    int frame = mnst->getFrame();
    int frameDave = s_GameClass->s_GameInfo->s_MyDave->getFrame();
    int dist = (-1)*(mnst->s_CoordX + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL - s_GameClass->s_GameInfo->s_MyDave->s_CoordX - s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frameDave].s_XL);
    if(sign == false) dist = abs(dist);
    lua_pushnumber(s_Lua, dist);
    return 1;
}

int LuaBindFunctions::getDistanceToDaveYHead(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1 && n != 2)
    {
        cout<<"Error! Number of arguments of function \"getDistanceToDaveYHead\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    bool sign = false;
    if(n == 2) sign = (bool)lua_tonumber(s_Lua, 2);
    int frame = mnst->getFrame();
    int frameDave = s_GameClass->s_GameInfo->s_MyDave->getFrame();
    int dist = (-1)*(mnst->s_CoordY + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL - s_GameClass->s_GameInfo->s_MyDave->s_CoordY - s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frameDave].s_YL);
    if(sign == false) dist = abs(dist);
    lua_pushnumber(s_Lua, dist);
    return 1;
}

int LuaBindFunctions::getDistanceToDaveHead(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1)
    {
        cout<<"Error! Number of arguments of function \"getDistanceToDaveHead\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    int frame = mnst->getFrame();
    int frameDave = s_GameClass->s_GameInfo->s_MyDave->getFrame();
    int d1 = abs(mnst->s_CoordY + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL - s_GameClass->s_GameInfo->s_MyDave->s_CoordY - s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frameDave].s_YL);
    int d2 = abs(mnst->s_CoordX + s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL - s_GameClass->s_GameInfo->s_MyDave->s_CoordX - s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][frameDave].s_XL);
    lua_pushnumber(s_Lua, sqrt(d1*d1 + d2*d2));
    return 1;
}

static int __testCollisionDave(lua_State* s_Lua)
{
    return s_LBF->testCollisionDave(s_Lua);
}

int LuaBindFunctions::testCollisionDave(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1)
    {
        cout<<"Error! Number of arguments of function \"testCollisionDave\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    bool isCollision = false;
    if(WorkFunctions::GameFunctions::testCollision(mnst->s_CoordX, mnst->s_CoordY, s_GameClass->s_GameInfo->s_MyDave->s_CoordX, s_GameClass->s_GameInfo->s_MyDave->s_CoordY, s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number - 1][mnst->s_State][mnst->getFrame()], s_GameClass->s_Data->s_Dave->s_Collisions[s_GameClass->s_GameInfo->s_MyDave->s_State][s_GameClass->s_GameInfo->s_MyDave->getFrame()]) == true)
        isCollision = true;
    lua_pushnumber(s_Lua, isCollision);
    return 1;
}

static int __killDave(lua_State* s_Lua)
{
    return s_LBF->killDave(s_Lua);
}

int LuaBindFunctions::killDave(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1)
    {
        cout<<"Error! Number of arguments of function \"killDave\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    s_GameClass->s_GameInfo->deathDave(mnst->s_Number);
    return 0;
}

static int __killMonster(lua_State* s_Lua)
{
    return s_LBF->killMonster(s_Lua);
}

int LuaBindFunctions::killMonster(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 2)
    {
        cout<<"Error! Number of arguments of function \"killMonster\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    int type = lua_tonumber(s_Lua, 2);
    mnst->kill(type);
    return 0;
}

static int __addDuplicateMonster(lua_State* s_Lua)
{
    return s_LBF->addDuplicateMonster(s_Lua);
}

int LuaBindFunctions::addDuplicateMonster(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 6 && n != 7 && n != 8)
    {
        cout<<"Error! Number of arguments of function \"addDuplicateMonster\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst = 0;
    CreatureMonster* curmonst = s_CurrentMonster;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    int coordX = lua_tonumber(s_Lua, 2);
    int coordY = lua_tonumber(s_Lua, 3);
    string state = lua_tostring(s_Lua, 4);
    int numberofaction = lua_tonumber(s_Lua, 5);
    int additionalnumberofaction = lua_tonumber(s_Lua, 6);
    int lives = -1;
    string parameters = "";
    if(n >= 7) lives = lua_tonumber(s_Lua, 7);
    if(n >= 8) parameters = lua_tostring(s_Lua, 8);
    CreatureMonster* newMonster = s_GameClass->s_GameInfo->s_FactoryMonsters->addMonster(mnst->s_Number, coordX, coordY, false);
    newMonster->s_Activated = true;
    newMonster->s_State = state;
    newMonster->s_NumberOfAction = numberofaction;
    newMonster->s_AdditionalNumberOfAction = additionalnumberofaction;
    if(lives != 0) newMonster->s_CurrentLives = lives;
    if(parameters != "")
    {
        parameters = "[mnst];" + parameters;
        IniParser::ParserInfoFile prs;
        IniParser::PostParsingStruct* pps = prs.getParsedFromString(parameters, STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
        map<string, string>::iterator iter1;
        for( iter1 = pps->getMapVariables()["mnst"].begin(); iter1 != pps->getMapVariables()["mnst"].end(); iter1++)
        {
            newMonster->s_AIMonsterValues[iter1->first] = iter1->second;
        }
        delete pps;
    }
    lua_pushnumber(s_Lua, newMonster->s_ID);
    s_CurrentMonster = curmonst;
    return 1;
}

static int __setMonsterValue(lua_State* s_Lua)
{
    return s_LBF->setMonsterValue(s_Lua);
}

int LuaBindFunctions::setMonsterValue(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 3)
    {
        cout<<"Error! Number of arguments of function \"setMonsterValue\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    string key = lua_tostring(s_Lua, 2);
    string value = lua_tostring(s_Lua, 3);
    mnst->s_AIMonsterValues[key] = value;
    return 0;
}

static int __getMonsterValue(lua_State* s_Lua)
{
    return s_LBF->getMonsterValue(s_Lua);
}

int LuaBindFunctions::getMonsterValue(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 2)
    {
        cout<<"Error! Number of arguments of function \"getMonsterValue\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    string key = lua_tostring(s_Lua, 2);
    string value = mnst->s_AIMonsterValues[key];
    lua_pushstring(s_Lua, value.c_str());
    return 1;
}

static int __testTileTypeRight(lua_State* s_Lua)
{
    return s_LBF->testTileTypeRight(s_Lua);
}

static int __testTileTypeLeft(lua_State* s_Lua)
{
    return s_LBF->testTileTypeLeft(s_Lua);
}

static int __testTileTypeUp(lua_State* s_Lua)
{
    return s_LBF->testTileTypeUp(s_Lua);
}

int LuaBindFunctions::testTileTypeRight(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 3 && n != 4)
    {
        cout<<"Error! Number of arguments of function \"testTileTypeRight\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    string tstTileType = lua_tostring(s_Lua, 2);
    int typeTest = lua_tonumber(s_Lua, 3);
    int numberOfLabel = 0;
    if(n == 4) numberOfLabel = lua_tonumber(s_Lua, 4);
    bool testTrue = true;
    if(typeTest == 1) testTrue = false;
    int testTileType = -1;
    if(tstTileType == "EMPTY") testTileType = EMPTY;
    if(tstTileType == "IMPASSABLE") testTileType = IMPASSABLE;
    if(tstTileType == "LADDER") testTileType = LADDER;
    if(tstTileType == "DEATH") testTileType = DEATH;
    if(tstTileType == "EXITLEVEL") testTileType = EXITLEVEL;
    int frame = mnst->getFrame();

    int SizeXMonsterPix = WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR, 16, 1) - WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL, 16, -1);
    int SizeYMonsterPix = WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR, 16, 1) - WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL, 16, -1);
    int SizeXMonster = SizeXMonsterPix/16 + 1;
    int SizeYMonster = SizeYMonsterPix/16 + 1;

    /*int SizeXMonsterPix = s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR - s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL;
    int SizeYMonsterPix = s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR - s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL;
    int SizeXMonster = (WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordX + SizeXMonsterPix, 16, 1) - WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordX, 16, -1)) / 16;
    int SizeYMonster = (WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordY + SizeYMonsterPix, 16, 1) - WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordY, 16, -1)) / 16;*/

    int** TileCoordX = new int* [SizeXMonster];
    int** TileCoordY = new int* [SizeXMonster];
    for(int i = 0; i < SizeXMonster; i++) TileCoordX[i] = new int[SizeYMonster];
    for(int i = 0; i < SizeXMonster; i++) TileCoordY[i] = new int[SizeYMonster];
    for(int i = 0; i < SizeXMonster; i++)
        for(int j = 0; j < SizeYMonster; j++)
        {
            TileCoordX[i][j] = WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordX,16,-1) + i*16;
            TileCoordY[i][j] = WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordY,16,-1) + j*16;
        }
    for(int i = 0; i < SizeYMonster; i++)
    {
        if(WorkFunctions::GameFunctions::testCollision(mnst->s_CoordX, mnst->s_CoordY, TileCoordX[i][SizeYMonster - 1], TileCoordY[i][SizeYMonster - 1], s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number - 1][mnst->s_State][mnst->getFrame()], Square(0,0,15,15), true, "left"))
        {
            if(typeTest == 0 && s_GameClass->s_Data->s_Level->getTileType(TileCoordX[SizeXMonster - 1][i]/16 + numberOfLabel, TileCoordY[SizeXMonster - 1][i]/16) != testTileType) testTrue = false;
            else if(typeTest == 1 && s_GameClass->s_Data->s_Level->getTileType(TileCoordX[SizeXMonster - 1][i]/16 + numberOfLabel, TileCoordY[SizeXMonster - 1][i]/16) == testTileType) testTrue = true;
        }
        //s_GameClass->s_Data->s_Level->s_Fields["FieldTiles"][(TileCoordY[SizeXMonster - 1][i]/16)*atoi( ( s_GameClass->s_Data->s_Level->s_Params->getValue("info", "sizeX") ).c_str() ) + TileCoordX[SizeXMonster - 1][i]/16 + numberOfLabel] = 0;
    }
    for(int i = 0; i < SizeXMonster; i++)
    {
        delete[] TileCoordX[i];
        delete[] TileCoordY[i];
    }
    delete TileCoordX;
    delete TileCoordY;
    lua_pushnumber(s_Lua, testTrue);
    return 1;
}

int LuaBindFunctions::testTileTypeLeft(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 3 && n != 4)
    {
        cout<<"Error! Number of arguments of function \"testTileTypeLeft\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    string tstTileType = lua_tostring(s_Lua, 2);
    int typeTest = lua_tonumber(s_Lua, 3);
    int numberOfLabel = 0;
    if(n == 4) numberOfLabel = lua_tonumber(s_Lua, 4);
    bool testTrue = true;
    if(typeTest == 1) testTrue = false;
    int testTileType = -1;
    if(tstTileType == "EMPTY") testTileType = EMPTY;
    if(tstTileType == "IMPASSABLE") testTileType = IMPASSABLE;
    if(tstTileType == "LADDER") testTileType = LADDER;
    if(tstTileType == "DEATH") testTileType = DEATH;
    if(tstTileType == "EXITLEVEL") testTileType = EXITLEVEL;
    int frame = mnst->getFrame();

    int SizeXMonsterPix = WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR, 16, 1) - WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL, 16, -1);
    int SizeYMonsterPix = WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR, 16, 1) - WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL, 16, -1);
    int SizeXMonster = SizeXMonsterPix/16 + 1;
    int SizeYMonster = SizeYMonsterPix/16 + 1;

    /*int SizeXMonsterPix = s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR - s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL;
    int SizeYMonsterPix = s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR - s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL;
    int SizeXMonster = (WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordX + SizeXMonsterPix, 16, 1) - WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordX, 16, -1)) / 16;
    int SizeYMonster = (WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordY + SizeYMonsterPix, 16, 1) - WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordY, 16, -1)) / 16;*/

    int** TileCoordX = new int* [SizeXMonster];
    int** TileCoordY = new int* [SizeXMonster];
    for(int i = 0; i < SizeXMonster; i++) TileCoordX[i] = new int[SizeYMonster];
    for(int i = 0; i < SizeXMonster; i++) TileCoordY[i] = new int[SizeYMonster];
    for(int i = 0; i < SizeXMonster; i++)
        for(int j = 0; j < SizeYMonster; j++)
        {
            TileCoordX[i][j] = WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordX,16,-1) + i*16;
            TileCoordY[i][j] = WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordY,16,-1) + j*16;
        }
    for(int i = 0; i < SizeYMonster; i++)
        if(typeTest == 0 && s_GameClass->s_Data->s_Level->getTileType(TileCoordX[0][i]/16 - numberOfLabel, TileCoordY[SizeXMonster - 1][i]/16) != testTileType) testTrue = false;
        else if(typeTest == 1 && s_GameClass->s_Data->s_Level->getTileType(TileCoordX[0][i]/16 - numberOfLabel, TileCoordY[SizeXMonster - 1][i]/16) == testTileType) testTrue = true;
    for(int i = 0; i < SizeXMonster; i++)
    {
        delete[] TileCoordX[i];
        delete[] TileCoordY[i];
    }
    delete TileCoordX;
    delete TileCoordY;
    lua_pushnumber(s_Lua, testTrue);
    return 1;
}

int LuaBindFunctions::testTileTypeUp(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 3 && n != 4)
    {
        cout<<"Error! Number of arguments of function \"testTileTypeUp\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    string tstTileType = lua_tostring(s_Lua, 2);
    int typeTest = lua_tonumber(s_Lua, 3);
    int numberOfLabel = 0;
    if(n == 4) numberOfLabel = lua_tonumber(s_Lua, 4);
    bool testTrue = true;
    if(typeTest == 1) testTrue = false;
    int testTileType = -1;
    if(tstTileType == "EMPTY") testTileType = EMPTY;
    if(tstTileType == "IMPASSABLE") testTileType = IMPASSABLE;
    if(tstTileType == "LADDER") testTileType = LADDER;
    if(tstTileType == "DEATH") testTileType = DEATH;
    if(tstTileType == "EXITLEVEL") testTileType = EXITLEVEL;
    int frame = mnst->getFrame();

    int SizeXMonsterPix = WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR, 16, 1) - WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL, 16, -1);
    int SizeYMonsterPix = WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR, 16, 1) - WorkFunctions::MathFunctions::roundNumber(s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL, 16, -1);
    int SizeXMonster = SizeXMonsterPix/16 + 1;
    int SizeYMonster = SizeYMonsterPix/16 + 1;

    /*int SizeXMonsterPix = s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XR - s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_XL;
    int SizeYMonsterPix = s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YR - s_GameClass->s_Data->s_Monsters->s_Collisions[mnst->s_Number-1][mnst->s_State][frame].s_YL;
    int SizeXMonster = (WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordX + SizeXMonsterPix, 16, 1) - WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordX, 16, -1)) / 16;
    int SizeYMonster = (WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordY + SizeYMonsterPix, 16, 1) - WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordY, 16, -1)) / 16;*/

    int** TileCoordX = new int* [SizeXMonster];
    int** TileCoordY = new int* [SizeXMonster];
    for(int i = 0; i < SizeXMonster; i++) TileCoordX[i] = new int[SizeYMonster];
    for(int i = 0; i < SizeXMonster; i++) TileCoordY[i] = new int[SizeYMonster];
    for(int i = 0; i < SizeXMonster; i++)
        for(int j = 0; j < SizeYMonster; j++)
        {
            TileCoordX[i][j] = WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordX,16,-1) + i*16;
            TileCoordY[i][j] = WorkFunctions::MathFunctions::roundNumber(mnst->s_CoordY,16,-1) + j*16;
        }
    for(int i = 0; i < SizeXMonster; i++)
        if(typeTest == 0 && s_GameClass->s_Data->s_Level->getTileType(TileCoordX[i][0]/16, TileCoordY[i][0]/16 - numberOfLabel) != testTileType) testTrue = false;
        else if(typeTest == 1 && s_GameClass->s_Data->s_Level->getTileType(TileCoordX[i][0]/16, TileCoordY[i][0]/16 - numberOfLabel) == testTileType) testTrue = true;
    for(int i = 0; i < SizeXMonster; i++)
    {
        delete[] TileCoordX[i];
        delete[] TileCoordY[i];
    }
    delete TileCoordX;
    delete TileCoordY;
    lua_pushnumber(s_Lua, testTrue);
    return 1;
}

static int __getCoordMonsterX(lua_State* s_Lua)
{
    return s_LBF->getCoordMonsterX(s_Lua);
}

static int __getCoordMonsterY(lua_State* s_Lua)
{
    return s_LBF->getCoordMonsterY(s_Lua);
}

int LuaBindFunctions::getCoordMonsterX(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1)
    {
        cout<<"Error! Number of arguments of function \"getCoordMonsterX\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    int p = mnst->s_CoordX;
    lua_pushnumber(s_Lua, p);
    return 1;
}

int LuaBindFunctions::getCoordMonsterY(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1)
    {
        cout<<"Error! Number of arguments of function \"getCoordMonsterY\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    int p = mnst->s_CoordY;
    lua_pushnumber(s_Lua, p);
    return 1;
}

static int __getCoordDaveX(lua_State* s_Lua)
{
    return s_LBF->getCoordDaveX(s_Lua);
}

static int __getStateDave(lua_State* s_Lua)
{
    return s_LBF->getStateDave(s_Lua);
}

static int __getCoordDaveY(lua_State* s_Lua)
{
    return s_LBF->getCoordDaveY(s_Lua);
}

int LuaBindFunctions::getCoordDaveX(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 0)
    {
        cout<<"Error! Number of arguments of function \"getCoordDaveX\" is incorrect!"<<endl;
        return 0;
    }
    lua_pushnumber(s_Lua, s_GameClass->s_GameInfo->s_MyDave->s_CoordX);
    return 1;
}

int LuaBindFunctions::getCoordDaveY(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 0)
    {
        cout<<"Error! Number of arguments of function \"getCoordDaveY\" is incorrect!"<<endl;
        return 0;
    }
    lua_pushnumber(s_Lua, s_GameClass->s_GameInfo->s_MyDave->s_CoordY);
    return 1;
}

int LuaBindFunctions::getStateDave(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 0)
    {
        cout<<"Error! Number of arguments of function \"getCoordDaveY\" is incorrect!"<<endl;
        return 0;
    }
    lua_pushstring(s_Lua, s_GameClass->s_GameInfo->s_MyDave->s_State.c_str());
    return 1;
}

static int __getNumberOfLives(lua_State* s_Lua)
{
    return s_LBF->getNumberOfLives(s_Lua);
}

static int __setNumberOfLives(lua_State* s_Lua)
{
    return s_LBF->setNumberOfLives(s_Lua);
}

int LuaBindFunctions::getNumberOfLives(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1)
    {
        cout<<"Error! Number of arguments of function \"getNumberOfLives\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    lua_pushnumber(s_Lua, mnst->s_CurrentLives);
    return 1;
}

int LuaBindFunctions::setNumberOfLives(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 2)
    {
        cout<<"Error! Number of arguments of function \"setNumberOfLives\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    int curlives = lua_tonumber(s_Lua, 2);
    mnst->s_CurrentLives = curlives;
    return 0;
}

static int __setGlobalValue(lua_State* s_Lua)
{
    return s_LBF->setGlobalValue(s_Lua);
}

int LuaBindFunctions::setGlobalValue(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 3)
    {
        cout<<"Error! Number of arguments of function \"setGlobalValue\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    string key = lua_tostring(s_Lua, 2);
    string value = lua_tostring(s_Lua, 3);
    s_GameClass->s_GameInfo->s_FactoryMonsters->s_AIMonstersValues[key] = value;
    return 0;
}

static int __getGlobalValue(lua_State* s_Lua)
{
    return s_LBF->getGlobalValue(s_Lua);
}

int LuaBindFunctions::getGlobalValue(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 2)
    {
        cout<<"Error! Number of arguments of function \"getGlobalValue\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    string key = lua_tostring(s_Lua, 2);
    string value = s_GameClass->s_GameInfo->s_FactoryMonsters->s_AIMonstersValues[key];
    lua_pushstring(s_Lua, value.c_str());
    return 1;
}

static int __setModSettingsValue(lua_State* s_Lua)
{
    return s_LBF->setModSettingsValue(s_Lua);
}

int LuaBindFunctions::setModSettingsValue(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 4)
    {
        cout<<"Error! Number of arguments of function \"setModSettingsValue\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    string key1 = lua_tostring(s_Lua, 2);
    string key2 = lua_tostring(s_Lua, 3);
    string value = lua_tostring(s_Lua, 4);
    s_GameClass->s_Data->s_ModSettings->setValue(key1, key2, value);
    return 0;
}

static int __getModSettingsValue(lua_State* s_Lua)
{
    return s_LBF->getModSettingsValue(s_Lua);
}

int LuaBindFunctions::getModSettingsValue(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 3)
    {
        cout<<"Error! Number of arguments of function \"getModSettingsValue\" is incorrect!"<<endl;
        return 0;
    }
    int keyMonster = lua_tonumber(s_Lua, 1);
    CreatureMonster* mnst;
    if(keyMonster == -1) mnst = s_CurrentMonster;
    else mnst = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[keyMonster];
    string s1 = lua_tostring(s_Lua, 2);
    string s2 = lua_tostring(s_Lua, 3);
    lua_pushstring(s_Lua, s_GameClass->s_Data->s_ModSettings->getValue(s1, s2).c_str());
    return 1;
}

static int __getMonsterID(lua_State* s_Lua)
{
    return s_LBF->getMonsterID(s_Lua);
}

int LuaBindFunctions::getMonsterID(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n > 0)
    {
        cout<<"Error! Number of arguments of function \"getMonsterID\" is incorrect!"<<endl;
        return 0;
    }
    lua_pushnumber(s_Lua, s_CurrentMonster->s_ID);
    return 1;
}

void LuaBindFunctions::prepareAIRun()
{
    s_LBF = this;
}

void LuaBindFunctions::registerFunctions(lua_State* s_Lua, string what)
{
    registerFunctionsAll(s_Lua);
    if(what == "AI") registerFunctionsMonster(s_Lua);
}

void LuaBindFunctions::registerFunctionsMonster(lua_State* s_Lua)
{
    lua_register(s_Lua, "setState", &__setState);
    lua_register(s_Lua, "getState", &__getState);
    lua_register(s_Lua, "goLeft", &__goLeft);
    lua_register(s_Lua, "goRight", &__goRight);
    lua_register(s_Lua, "goUp", &__goUp);
    lua_register(s_Lua, "goDown", &__goDown);
    lua_register(s_Lua, "setNumberOfAction", &__setNumberOfAction);
    lua_register(s_Lua, "getNumberOfAction", &__getNumberOfAction);
    lua_register(s_Lua, "nextNumberOfAction", &__nextNumberOfAction);
    lua_register(s_Lua, "setNullNumberOfAction", &__setNullNumberOfAction);
    lua_register(s_Lua, "setAdditionalNumberOfAction", &__setAdditionalNumberOfAction);
    lua_register(s_Lua, "getAdditionalNumberOfAction", &__getAdditionalNumberOfAction);
    lua_register(s_Lua, "nextAdditionalNumberOfAction", &__nextAdditionalNumberOfAction);
    lua_register(s_Lua, "setNullAdditionalNumberOfAction", &__setNullAdditionalNumberOfAction);
    lua_register(s_Lua, "getMonsterOption", &__getMonsterOption);
    lua_register(s_Lua, "testLookDaveX", &__testLookDaveX);
    lua_register(s_Lua, "testLookDaveY", &__testLookDaveY);
    lua_register(s_Lua, "getMonsterFrame", &__getMonsterFrame);
    lua_register(s_Lua, "getDistanceToDave", &__getDistanceToDave);
    lua_register(s_Lua, "getDistanceToDaveX", &__getDistanceToDaveX);
    lua_register(s_Lua, "getDistanceToDaveY", &__getDistanceToDaveY);
    lua_register(s_Lua, "testTileTypeDown", &__testTileTypeDown);
    lua_register(s_Lua, "getDistanceToDaveHead", &__getDistanceToDaveHead);
    lua_register(s_Lua, "getDistanceToDaveXHead", &__getDistanceToDaveXHead);
    lua_register(s_Lua, "getDistanceToDaveYHead", &__getDistanceToDaveYHead);
    lua_register(s_Lua, "testCollisionDave", &__testCollisionDave);
    lua_register(s_Lua, "killDave", &__killDave);
    lua_register(s_Lua, "killMonster", &__killMonster);
    lua_register(s_Lua, "addDuplicateMonster", &__addDuplicateMonster);
    lua_register(s_Lua, "setMonsterValue", &__setMonsterValue);
    lua_register(s_Lua, "getMonsterValue", &__getMonsterValue);
    lua_register(s_Lua, "setGlobalValue", &__setGlobalValue);
    lua_register(s_Lua, "getGlobalValue", &__getGlobalValue);
    lua_register(s_Lua, "setModSettingsValue", &__setModSettingsValue);
    lua_register(s_Lua, "getModSettingsValue", &__getModSettingsValue);
    lua_register(s_Lua, "getMonsterID", &__getMonsterID);
    lua_register(s_Lua, "testTileTypeRight", &__testTileTypeRight);
    lua_register(s_Lua, "testTileTypeLeft", &__testTileTypeLeft);
    lua_register(s_Lua, "testTileTypeUp", &__testTileTypeUp);
    lua_register(s_Lua, "getCoordMonsterX", &__getCoordMonsterX);
    lua_register(s_Lua, "getCoordMonsterY", &__getCoordMonsterY);
    lua_register(s_Lua, "getCoordDaveX", &__getCoordDaveX);
    lua_register(s_Lua, "getCoordDaveY", &__getCoordDaveY);
    lua_register(s_Lua, "getStateDave", &__getStateDave);
    lua_register(s_Lua, "getNumberOfLives", &__getNumberOfLives);
    lua_register(s_Lua, "setNumberOfLives", &__setNumberOfLives);
}

static int __addPackImagesToFactoryTemporaryImage(lua_State* s_Lua)
{
    return s_LBF->addPackImagesToFactoryTemporaryImage(s_Lua);
}

int LuaBindFunctions::addPackImagesToFactoryTemporaryImage(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 11)
    {
        cout<<"Error! Number of arguments of function \"addPackImagesToFactoryTemporaryImage\" is incorrect!"<<endl;
        return 0;
    }
    string pack = lua_tostring(s_Lua, 1);
    int number = lua_tonumber(s_Lua, 2) - 1;
    string state = lua_tostring(s_Lua, 3);
    int coordX = lua_tonumber(s_Lua, 4);
    int coordY = lua_tonumber(s_Lua, 5);
    int timeLive = lua_tonumber(s_Lua, 6);
    int changeX = lua_tonumber(s_Lua, 7);
    int changeY = lua_tonumber(s_Lua, 8);
    int numberofframes = lua_tonumber(s_Lua, 9);
    string type = lua_tostring(s_Lua, 10);
    bool animated = (bool)lua_tonumber(s_Lua, 11);
    map<int, Bitmap*>* bmp = 0;
    map<int, Bitmap*>* cachebmp = 0;
    if(pack == "monsters")
    {
        bmp = &s_GameClass->s_Data->s_Monsters->s_Bitmaps[number][state];
        cachebmp = &s_GameClass->s_Data->s_Monsters->s_CacheImages[number][state];
    }
    s_GameClass->s_FactoryTmpImgs->addImage(bmp, cachebmp, coordX, coordY, timeLive, changeX, changeY, numberofframes, type, animated);
    return 0;
}

static int __addImageToFactoryTemporaryImage(lua_State* s_Lua)
{
    return s_LBF->addImageToFactoryTemporaryImage(s_Lua);
}

int LuaBindFunctions::addImageToFactoryTemporaryImage(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 10)
    {
        cout<<"Error! Number of arguments of function \"addImageToFactoryTemporaryImage\" is incorrect!"<<endl;
        return 0;
    }
    string pack = lua_tostring(s_Lua, 1);
    int number = lua_tonumber(s_Lua, 2) - 1;
    string state = lua_tostring(s_Lua, 3);
    int numberofframe = lua_tonumber(s_Lua, 4) - 1;
    int coordX = lua_tonumber(s_Lua, 5);
    int coordY = lua_tonumber(s_Lua, 6);
    int timeLive = lua_tonumber(s_Lua, 7);
    int changeX = lua_tonumber(s_Lua, 8);
    int changeY = lua_tonumber(s_Lua, 9);
    string type = lua_tostring(s_Lua, 10);
    Bitmap* bmp = 0;
    Bitmap* cachebmp = 0;
    if(pack == "monsters")
    {
        bmp = s_GameClass->s_Data->s_Monsters->s_Bitmaps[number][state][numberofframe];
        cachebmp = s_GameClass->s_Data->s_Monsters->s_CacheImages[number][state][numberofframe];
    }
    s_GameClass->s_FactoryTmpImgs->addImage(bmp, cachebmp, coordX, coordY, timeLive, changeX, changeY, type);
    return 0;
}

static int __changeNextLevel(lua_State* s_Lua)
{
    return s_LBF->changeNextLevel(s_Lua);
}

int LuaBindFunctions::changeNextLevel(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n > 0)
    {
        cout<<"Error! Number of arguments of function \"changeNextLevel\" is incorrect!"<<endl;
        return 0;
    }
    s_GameClass->s_GameInfo->s_ForcedChangeLevel = true;
    return 0;
}

static int __playSound(lua_State* s_Lua)
{
    return s_LBF->playSound(s_Lua);
}

int LuaBindFunctions::playSound(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1 && n != 2 && n != 3 && n != 4 && n != 5)
    {
        cout<<"Error! Number of arguments of function \"playSound\" is incorrect!"<<endl;
        return 0;
    }
    string soundname = lua_tostring(s_Lua, 1);
    int new_sound = 1;
    int restart = 0;
    int loop = 0;
    int offset_ms = 0;
    if(n > 1) new_sound = lua_tonumber(s_Lua, 2);
    if(n > 2) restart = lua_tonumber(s_Lua, 3);
    if(n > 3) loop = lua_tonumber(s_Lua, 4);
    if(n > 4) offset_ms = lua_tonumber(s_Lua, 5);
    int result_play = s_GameClass->s_Data->s_Sounds->play(soundname, new_sound, restart, loop, offset_ms);
    lua_pushnumber(s_Lua, result_play);
    return 0;
}

static int __stopSound(lua_State* s_Lua)
{
    return s_LBF->playSound(s_Lua);
}

int LuaBindFunctions::stopSound(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1)
    {
        cout<<"Error! Number of arguments of function \"stopSound\" is incorrect!"<<endl;
        return 0;
    }
    string soundname = lua_tostring(s_Lua, 1);
    bool result_stop = s_GameClass->s_Data->s_Sounds->stop(soundname);
    lua_pushnumber(s_Lua, result_stop);
    return 0;
}

static int __pauseSound(lua_State* s_Lua)
{
    return s_LBF->playSound(s_Lua);
}

int LuaBindFunctions::pauseSound(lua_State* s_Lua)
{
    int n = lua_gettop(s_Lua);
    if(n != 1)
    {
        cout<<"Error! Number of arguments of function \"pauseSound\" is incorrect!"<<endl;
        return 0;
    }
    string soundname = lua_tostring(s_Lua, 1);
    bool result_stop = s_GameClass->s_Data->s_Sounds->pause(soundname);
    lua_pushnumber(s_Lua, result_stop);
    return 0;
}

void LuaBindFunctions::registerFunctionsAll(lua_State* s_Lua)
{
    lua_register(s_Lua, "getMainValue", &__getMainValue);
    lua_register(s_Lua, "addPackImagesToFactoryTemporaryImage", &__addPackImagesToFactoryTemporaryImage);
    lua_register(s_Lua, "addImageToFactoryTemporaryImage", &__addImageToFactoryTemporaryImage);
    lua_register(s_Lua, "changeNextLevel", &__changeNextLevel);
    lua_register(s_Lua, "playSound", &__playSound);
    lua_register(s_Lua, "stopSound", &__stopSound);
    lua_register(s_Lua, "pauseSound", &__pauseSound);
}
