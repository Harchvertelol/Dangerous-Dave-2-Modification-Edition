#include "Creature.h"

#include "Game.h"

#include "WorkFunction.h"
#include "Defines.h"

#include "KeysState.h"

using namespace WorkFunction;
using namespace MathFunction;
using namespace GameFunction;
using namespace ConvertFunction;

Creature::Creature(Game* gameclass):
    s_GameClass(gameclass),
    s_CurrentPoints(0),
    s_CurrentHealth(1),
    s_MaxHealth(1),
    s_Cartridges(8),
    s_MaxCartridges(8),
    s_CoordX(0),
    s_CoordY(0),
    s_State("rightstand"),
    s_NumberOfAction(0),
    s_JumpStep(0),
    s_ControlJumpPressed(false),
    s_ControlShootPressed(false),
    s_NumberOfTilesJump(0),
    s_FreezeJump(0),
    s_Acceleration(0),
    s_TimeDoorOpen(0),
    s_StateBeforeOpenDoor(""),
    s_HowDoorOpen(""),
    s_OldAnSt(0),
    s_OldNumberOfAction(0),
    s_AdditionalNumberOfAction(0),
    s_ShootNow(0),
    s_DopState(""),
    s_ScreenCoordX(0),
    s_ScreenCoordY(0),
    s_NickName("")
{
    s_KeysState = new KeysState;
    s_KeysState->s_KeyDown = false;
    s_KeysState->s_KeyUp = false;
    s_KeysState->s_KeyLeft = false;
    s_KeysState->s_KeyRight = false;
    s_KeysState->s_KeyShoot = false;
    s_KeysState->s_KeyJump = false;
}

Creature::~Creature()
{
    if(s_KeysState != 0) delete s_KeysState;
}

void Creature::live()
{
    calculateDoKey();
    if(s_State.find("jump") != string::npos && s_ShootNow) s_ShootNow = 0;
    string direction, typeexit, statedave;
    int dir = 0, timeshoot = atoi( s_GameClass->s_IniFile->getValue("settings", "timeshoot").c_str() );
    int numberofdoors, numberofhandletiles, TileDoor, x, y;
    int SizeXLev = atoi( ( s_GameClass->s_Data->s_Level->s_Params->getValue("info", "sizeX") ).c_str() );
    if(s_State.find("right") == 0) dir = 1;
    else dir = -1;
    if(s_ShootNow > 0)
    {
        if(s_ShootNow == 2*timeshoot/3 )
        {
            if(s_Cartridges <= 0)
            {
                s_ShootNow = 0;
                if(s_State.find("down") == string::npos && s_State.find("up") == string::npos) s_State = s_State.substr(0, s_State.find("shoot")) + "stand";
            }
            else
            {
                testShoot();
                statedave = "fireshoot" + s_State.substr(0, s_State.find("shoot"));
                s_GameClass->s_FactoryTmpImgs->addImage(s_GameClass->s_Data->s_Dave->s_Bitmaps[statedave][getFrame(statedave)],
                                                        s_GameClass->s_Data->s_Dave->s_CacheImages[statedave][getFrame(statedave)],
                                                        s_CoordX + atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("shoot", "coordfire" + s_State.substr(0, s_State.find("shoot")) + "X").c_str() ),
                                                        s_CoordY + atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("shoot", "coordfire" + s_State.substr(0, s_State.find("shoot")) + "Y").c_str() ),
                                                        4, 0, 0, "fire");
                if(s_GameClass->s_GameInfo->s_CheatGod == false) s_Cartridges--;
                s_CoordX -= dir*2;
                correctionPhys(s_CoordX + dir*2, 0);
                s_State += "now";
            }
        }
        else if(s_ShootNow == timeshoot/3 )
        {
            statedave = "smokeshoot" + s_State.substr(0, s_State.find("shoot"));
                s_GameClass->s_FactoryTmpImgs->addImage(s_GameClass->s_Data->s_Dave->s_Bitmaps[statedave][getFrame(statedave)],
                                                        s_GameClass->s_Data->s_Dave->s_CacheImages[statedave][getFrame(statedave)],
                                                        dir*4 + s_CoordX + atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("shoot", "coordfire" + s_State.substr(0, s_State.find("shoot")) + "X").c_str() ),
                                                        -2 + s_CoordY + atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("shoot", "coordfire" + s_State.substr(0, s_State.find("shoot")) + "Y").c_str() ),
                                                        5, 0, -1, "fire");
            s_CoordX -= dir*2;
            correctionPhys(s_CoordX + dir*2, 0);
        }
        if(s_ShootNow > 0) s_ShootNow--;
    }
    else if(s_State.find("shootnow") != string::npos)
    {
        if(s_State.find("right") == 0) dir = 1;
        else dir = -1;
        s_CoordX -= dir*4;
        if(s_State.find("down") == string::npos && s_State.find("up") == string::npos) s_State = s_State.substr(0, s_State.find("shootnow")) + "stand";
        else s_State = s_State.substr(0, s_State.find("now"));
        correctionPhys(s_CoordX + dir*4, 0);
    }
    if(s_State.find("jumpdown") != string::npos)
    {
        s_Acceleration++;
        y = s_CoordY;
        if(s_Acceleration > atoi( s_GameClass->s_IniFile->getValue("settings","jumpstep").c_str() ) * atoi( s_GameClass->s_IniFile->getValue("settings","maxnumberoftilesforjump").c_str() ) )
        {
            s_CoordY += 2*atoi( s_GameClass->s_IniFile->getValue("settings","gravity").c_str() );
            correctionPhys(s_CoordY - 2*atoi( s_GameClass->s_IniFile->getValue("settings","gravity").c_str() ), 1);
        }
        else
        {
            s_CoordY += atoi( s_GameClass->s_IniFile->getValue("settings","gravity").c_str() );
            correctionPhys(s_CoordY - atoi( s_GameClass->s_IniFile->getValue("settings","gravity").c_str() ), 1);
        }
        if(roundNumber(y,16,1) == roundNumber(s_CoordY,16,-1)) testSmallPassage(y);
    }
    else if(s_State.find("jumpup") != string::npos)
    {
        s_JumpStep--;
        y = s_CoordY;
        s_CoordY -= atoi( s_GameClass->s_IniFile->getValue("settings","jumpspeed").c_str() );
        bool cor = correctionPhys(s_CoordY + atoi( s_GameClass->s_IniFile->getValue("settings","jumpspeed").c_str() ), 1);
        if(cor == true && s_FreezeJump == false && s_GameClass->s_IniFile->getValue("settings", "realisticphysics") == "true") s_JumpStep = 0;
        else if(cor == true && s_FreezeJump == false) s_JumpStep--;
        direction = s_State.substr(0, s_State.find("jumpup"));
        if(s_JumpStep <= 0)
        {
            s_State = direction + "jumpstand";
            if(s_GameClass->s_IniFile->getValue("settings", "realisticphysics") == "true") s_FreezeJump = atoi( s_GameClass->s_IniFile->getValue("settings","timefreezejump").c_str() );
            else s_FreezeJump = atoi( s_GameClass->s_IniFile->getValue("settings","timefreezejump").c_str() );
        }
        if(roundNumber(y,16,-1) == roundNumber(s_CoordY,16,1)) testSmallPassage(y);
    }
    else if(s_State.find("jumpstand") != string::npos)
    {
        s_Acceleration = 0;
        direction = s_State.substr(0, s_State.find("jumpstand"));
        if(s_FreezeJump == 0) s_State = direction + "jumpdown";
        else s_FreezeJump--;
    }
    else if(s_State == "dooropen")
    {
        s_TimeDoorOpen--;
        if(s_TimeDoorOpen == 0)
        {
            s_State = s_StateBeforeOpenDoor;
            if(s_HowDoorOpen == "bonus")
            {
                s_GameClass->s_Data->s_Level->s_Fields["FieldBonuses"][(s_CoordY - 1)/16*SizeXLev + s_CoordX/16] = s_GameClass->s_Data->s_Level->s_Fields["FieldBonusDoors"][s_CoordY/16*SizeXLev + s_CoordX/16];
                s_GameClass->s_Data->s_Level->s_Fields["FieldBonusDoors"][s_CoordY/16*SizeXLev + s_CoordX/16] = 0;
                typeexit = "bonus";
            }
            else if(s_HowDoorOpen == "exit")
            {
                s_GameClass->s_Data->s_Level->s_Fields["FieldDoors"][s_CoordY/16*SizeXLev + s_CoordX/16] = s_GameClass->s_Data->s_Level->s_Fields["FieldDoors"][s_CoordY/16*SizeXLev + s_CoordX/16]*(-1);
                typeexit = "exit";
            }
            else if(s_HowDoorOpen == "exitlevel")
            {
                s_GameClass->s_Data->s_Level->s_Fields["FieldExitLevelDoors"][s_CoordY/16*SizeXLev + s_CoordX/16] = s_GameClass->s_Data->s_Level->s_Fields["FieldExitLevelDoors"][s_CoordY/16*SizeXLev + s_CoordX/16]*(-1);
                typeexit = "exit";
            }
            TileDoor = s_GameClass->s_Data->s_Level->s_Fields["FieldTiles"][s_CoordY/16*SizeXLev + s_CoordX/16];
            numberofdoors = atoi( s_GameClass->s_Data->s_Textures->s_TilesInfo->getValue("info","numberofdoors").c_str() );
            for(int i = 0; i < numberofdoors; i++)
            {
                numberofhandletiles = atoi( s_GameClass->s_Data->s_Textures->s_TilesInfo->getValue("door_" + itos(i+1),"numberofhandletiles").c_str() );
                for(int j = 0; j < numberofhandletiles; j++)
                {
                    if( typeexit == s_GameClass->s_Data->s_Textures->s_TilesInfo->getValue("door_" + itos(i+1),"type") && atoi( s_GameClass->s_Data->s_Textures->s_TilesInfo->getValue("door_" + itos(i+1),"handletile" + itos(j+1) ).c_str() ) == TileDoor)
                    {
                        s_GameClass->s_Data->s_Level->s_Fields["FieldTiles"][(s_CoordY - 16)/16*SizeXLev + s_CoordX/16 - 1] = atoi( s_GameClass->s_Data->s_Textures->s_TilesInfo->getValue("door_" + itos(i+1),"opentile" + itos(1) ).c_str() );
                        s_GameClass->s_Data->s_Level->s_Fields["FieldTiles"][(s_CoordY - 16)/16*SizeXLev + s_CoordX/16] = atoi( s_GameClass->s_Data->s_Textures->s_TilesInfo->getValue("door_" + itos(i+1),"opentile" + itos(2) ).c_str() );
                        s_GameClass->s_Data->s_Level->s_Fields["FieldTiles"][(s_CoordY)/16*SizeXLev + s_CoordX/16 - 1] = atoi( s_GameClass->s_Data->s_Textures->s_TilesInfo->getValue("door_" + itos(i+1),"opentile" + itos(3) ).c_str() );
                        s_GameClass->s_Data->s_Level->s_Fields["FieldTiles"][(s_CoordY)/16*SizeXLev + s_CoordX/16] = atoi( s_GameClass->s_Data->s_Textures->s_TilesInfo->getValue("door_" + itos(i+1),"opentile" + itos(4) ).c_str() );
                        s_GameClass->s_Data->s_Level->s_Fields["FieldTiles"][(s_CoordY + 16)/16*SizeXLev + s_CoordX/16 - 1] = atoi( s_GameClass->s_Data->s_Textures->s_TilesInfo->getValue("door_" + itos(i+1),"opentile" + itos(5) ).c_str() );
                        s_GameClass->s_Data->s_Level->s_Fields["FieldTiles"][(s_CoordY + 16)/16*SizeXLev + s_CoordX/16] = atoi( s_GameClass->s_Data->s_Textures->s_TilesInfo->getValue("door_" + itos(i+1),"opentile" + itos(6) ).c_str() );
                        break;
                        break;
                    }
                }
            }
        }
    }
    else if(s_State == "doorexit")
    {
        s_NumberOfAction = s_OldNumberOfAction;
        if(s_GameClass->s_AnimationStep - s_OldAnSt > (unsigned int)atoi( s_GameClass->s_IniFile->getValue("settings","timeexitdoor").c_str() ) )
        {
            s_TimeDoorOpen--;
            s_NumberOfAction = s_OldNumberOfAction + 1;
            s_OldNumberOfAction = s_NumberOfAction;
            s_OldAnSt = s_GameClass->s_AnimationStep;
        }
        if(s_TimeDoorOpen == 0)
        {
            s_CoordX += 10;
            s_State = s_StateBeforeOpenDoor;
            if(s_HowDoorOpen == "exit")
            {
                x = 16*(s_GameClass->s_Data->s_Level->s_Fields["FieldDoors"][s_CoordY/16*SizeXLev + s_CoordX/16]%SizeXLev + 1);
                y = 16*((s_GameClass->s_Data->s_Level->s_Fields["FieldDoors"][s_CoordY/16*SizeXLev + s_CoordX/16] - s_GameClass->s_Data->s_Level->s_Fields["FieldDoors"][s_CoordX/16*SizeXLev + s_CoordY/16]%SizeXLev)/SizeXLev + 1);
                s_CoordX = x;
                s_CoordY = y;
                TileDoor = s_GameClass->s_Data->s_Level->s_Fields["FieldTiles"][s_CoordY/16*SizeXLev + s_CoordX/16];
                numberofdoors = atoi( s_GameClass->s_Data->s_Textures->s_TilesInfo->getValue("info","numberofdoors").c_str() );
                s_GameClass->s_Data->s_Level->s_Fields["FieldDoors"][s_CoordY/16*SizeXLev + s_CoordX/16] = abs(s_GameClass->s_Data->s_Level->s_Fields["FieldDoors"][s_CoordY/16*SizeXLev + s_CoordX/16]);
                typeexit = "exit";
                for(int i = 0; i < numberofdoors; i++)
                {
                    numberofhandletiles = atoi( s_GameClass->s_Data->s_Textures->s_TilesInfo->getValue("door_" + itos(i+1),"numberofhandletiles").c_str() );
                    for(int j = 0; j < numberofhandletiles; j++)
                    {
                        if( typeexit == s_GameClass->s_Data->s_Textures->s_TilesInfo->getValue("door_" + itos(i+1),"type") && atoi( s_GameClass->s_Data->s_Textures->s_TilesInfo->getValue("door_" + itos(i+1),"handletile" + itos(j+1) ).c_str() ) == TileDoor)
                        {
                            s_GameClass->s_Data->s_Level->s_Fields["FieldTiles"][(s_CoordY - 16)/16*SizeXLev + s_CoordX/16 - 1] = atoi( s_GameClass->s_Data->s_Textures->s_TilesInfo->getValue("door_" + itos(i+1),"opentile" + itos(1) ).c_str() );
                            s_GameClass->s_Data->s_Level->s_Fields["FieldTiles"][(s_CoordY - 16)/16*SizeXLev + s_CoordX/16] = atoi( s_GameClass->s_Data->s_Textures->s_TilesInfo->getValue("door_" + itos(i+1),"opentile" + itos(2) ).c_str() );
                            s_GameClass->s_Data->s_Level->s_Fields["FieldTiles"][(s_CoordY)/16*SizeXLev + s_CoordX/16 - 1] = atoi( s_GameClass->s_Data->s_Textures->s_TilesInfo->getValue("door_" + itos(i+1),"opentile" + itos(3) ).c_str() );
                            s_GameClass->s_Data->s_Level->s_Fields["FieldTiles"][(s_CoordY)/16*SizeXLev + s_CoordX/16] = atoi( s_GameClass->s_Data->s_Textures->s_TilesInfo->getValue("door_" + itos(i+1),"opentile" + itos(4) ).c_str() );
                            s_GameClass->s_Data->s_Level->s_Fields["FieldTiles"][(s_CoordY + 16)/16*SizeXLev + s_CoordX/16 - 1] = atoi( s_GameClass->s_Data->s_Textures->s_TilesInfo->getValue("door_" + itos(i+1),"opentile" + itos(5) ).c_str() );
                            s_GameClass->s_Data->s_Level->s_Fields["FieldTiles"][(s_CoordY + 16)/16*SizeXLev + s_CoordX/16] = atoi( s_GameClass->s_Data->s_Textures->s_TilesInfo->getValue("door_" + itos(i+1),"opentile" + itos(6) ).c_str() );
                            break;
                            break;
                        }
                    }
                }
                s_CoordX -= 4;
            }
            else if(s_HowDoorOpen == "exitlevel")
            {
                s_GameClass->changeLevel(s_GameClass->s_Data->s_Level->s_Fields["FieldExitLevelDoors"][s_CoordY/16*SizeXLev + s_CoordX/16]);
            }
        }
    }
    if(s_Cartridges < s_MaxCartridges && testSetStates("rightstand leftstand"))
    {
        s_DopState = s_State;
        s_State = "recharge";
        s_NumberOfAction = 0;
        s_AdditionalNumberOfAction = 0;
        s_OldNumberOfAction = 0;
    }
    else if(s_Cartridges >= s_MaxCartridges && s_State == "recharge")
    {
        s_Cartridges = s_MaxCartridges;
        s_State = s_DopState;
        s_NumberOfAction = 0;
        s_AdditionalNumberOfAction = 0;
        s_OldNumberOfAction = 0;
    }
    if(testSetStates("recharge"))
    {
        if(s_OldNumberOfAction < s_NumberOfAction)
        {
            s_NumberOfAction--;
            s_AdditionalNumberOfAction++;
            if(s_AdditionalNumberOfAction % atoi( s_GameClass->s_IniFile->getValue("settings", "rechargetime").c_str() ) == 0 )
            {
                s_NumberOfAction++;
                s_OldNumberOfAction = s_NumberOfAction;
                if(s_NumberOfAction % atoi(s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("other", "rechargeframe").c_str() ) == 0 ) s_Cartridges++;
            }
        }
    }
    int TileCoordX = roundNumber(s_CoordX, 16, -1);
    int TileCoordY = roundNumber(s_CoordY + 32, 16, 1);
    int TileType1 = s_GameClass->s_Data->s_Level->getTileType(TileCoordX/16, TileCoordY/16);
    int TileType2 = s_GameClass->s_Data->s_Level->getTileType(TileCoordX/16 + 1, TileCoordY/16);
    int numberofframes = atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("info", "numberofframes" + s_State).c_str() );
    int frame = s_NumberOfAction%numberofframes;
    bool StandCollision1 = testCollision(s_CoordX, s_CoordY, TileCoordX, TileCoordY, s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame], Square(0,0,15,15), true);
    bool StandCollision2 = testCollision(s_CoordX, s_CoordY, TileCoordX + 16, TileCoordY, s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame], Square(0,0,15,15), true);
    if( ( (TileType1 == IMPASSABLE || TileType1 == LADDER) && StandCollision1 ) || ( (TileType2 == IMPASSABLE || TileType2 == LADDER) && StandCollision2 ) )
    {
        if(s_State.find("jump") != string::npos)
        {
            if(s_State.find("right") == 0) step("right");
            else step("left");
            if(s_State.find("right") == 0) s_State = "rightstand";
            else s_State = "leftstand";
            StandCollision1 = testCollision(s_CoordX, s_CoordY, TileCoordX, TileCoordY, s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame], Square(0,0,15,15), true);
            StandCollision2 = testCollision(s_CoordX, s_CoordY, TileCoordX + 16, TileCoordY, s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame], Square(0,0,15,15), true);
            if( !( ( (TileType1 == IMPASSABLE || TileType1 == LADDER) && StandCollision1 ) || ( (TileType2 == IMPASSABLE || TileType2 == LADDER) && StandCollision2 ) ) )
            {
                if(s_State.find("right") == 0) step("right");
                else step("left");
            }
        }
    }
    else
    {
        if(s_State.find("jump") == string::npos)
        {
            if(s_State == "recharge") s_State = s_DopState;
            if(s_State.find("right") == 0) s_State = "rightjumpdown";
            else s_State = "leftjumpdown";
            s_Acceleration = 0;
        }
    }
    testGetBonuses();
    if(testChangeLevel()) s_GameClass->changeNextLevel();
    int DeathType = testDeath();
    s_GameClass->s_GameInfo->deathDave(DeathType);
}

void Creature::testSmallPassage(int y)
{
    int FreezeX = s_CoordX;
    int FreezeY = s_CoordY;
    if(s_GameClass->s_StateManager->getKeyPressed("right"))
    {
        s_CoordX += 4;
        if(correctionPhys(s_CoordX - 4, 0))
        {
            s_CoordX = FreezeX;
            if(s_CoordY > y)
            {
                s_CoordY = roundNumber(s_CoordY,16,-1);
                s_CoordX += 4;
                if(!correctionPhys(s_CoordX - 4, 0))
                {
                    FreezeX = s_CoordX;
                    FreezeY = s_CoordY;
                }
            }
            else
            {
                s_CoordY = roundNumber(s_CoordY,16,1);
                s_CoordX += 4;
                if(!correctionPhys(s_CoordX - 4, 0))
                {
                    FreezeX = s_CoordX;
                    FreezeY = s_CoordY;
                }
            }
        }
    }
    else if(s_GameClass->s_StateManager->getKeyPressed("left"))
    {
        s_CoordX -= 4;
        if(correctionPhys(s_CoordX + 4, 0))
        {
            s_CoordX = FreezeX;
            if(s_CoordY > y)
            {
                s_CoordY = roundNumber(s_CoordY,16,-1);
                s_CoordX -= 4;
                if(!correctionPhys(s_CoordX + 4, 0))
                {
                    FreezeX = s_CoordX;
                    FreezeY = s_CoordY;
                }
            }
            else
            {
                s_CoordY = roundNumber(s_CoordY,16,1);
                s_CoordX -= 4;
                if(!correctionPhys(s_CoordX + 4, 0))
                {
                    FreezeX = s_CoordX;
                    FreezeY = s_CoordY;
                }
            }
        }
    }
    s_CoordX = FreezeX;
    s_CoordY = FreezeY;
}

bool Creature::testOpenDoor()
{
    int TileCoordX[6];
    int TileCoordY[6];
    TileCoordX[0] = roundNumber(s_CoordX, 16, -1);
    TileCoordX[1] = roundNumber(s_CoordX, 16, -1) + 16;
    TileCoordX[2] = roundNumber(s_CoordX, 16, -1);
    TileCoordX[3] = roundNumber(s_CoordX, 16, -1) + 16;
    TileCoordX[4] = roundNumber(s_CoordX, 16, -1);
    TileCoordX[5] = roundNumber(s_CoordX, 16, -1) + 16;
    TileCoordY[0] = roundNumber(s_CoordY, 16, -1);
    TileCoordY[1] = roundNumber(s_CoordY, 16, -1);
    TileCoordY[2] = roundNumber(s_CoordY, 16, -1) + 16;
    TileCoordY[3] = roundNumber(s_CoordY, 16, -1) + 16;
    TileCoordY[4] = roundNumber(s_CoordY, 16, -1) + 32;
    TileCoordY[5] = roundNumber(s_CoordY, 16, -1) + 32;
    int numberofframes = atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("info", "numberofframes" + s_State).c_str() );
    int frame = s_NumberOfAction%numberofframes;
    int SizeXLev = atoi( ( s_GameClass->s_Data->s_Level->s_Params->getValue("info", "sizeX") ).c_str() );
    s_StateBeforeOpenDoor = s_State;
    if( s_GameClass->s_Data->s_Level->s_Fields["FieldBonusDoors"][TileCoordY[0]/16*SizeXLev + TileCoordX[0]/16] != 0 && testCollision(s_CoordX, s_CoordY, TileCoordX[0], TileCoordY[0], s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame], Square(0,0,15,15)) )
    {
        s_CoordX = TileCoordX[0];
        s_CoordY = TileCoordY[0];
        s_State = "dooropen";
        s_HowDoorOpen = "bonus";
        s_TimeDoorOpen = atoi( s_GameClass->s_IniFile->getValue("settings","timedooropen").c_str() );
        return true;
    }
    else if( s_GameClass->s_Data->s_Level->s_Fields["FieldBonusDoors"][TileCoordY[1]/16*SizeXLev + TileCoordX[1]/16] != 0 && testCollision(s_CoordX, s_CoordY, TileCoordX[1], TileCoordY[1], s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame], Square(0,0,15,15)) )
    {
        s_CoordX = TileCoordX[1];
        s_CoordY = TileCoordY[1];
        s_State = "dooropen";
        s_HowDoorOpen = "bonus";
        s_TimeDoorOpen = atoi( s_GameClass->s_IniFile->getValue("settings","timedooropen").c_str() );
        return true;
    }
    else if( s_GameClass->s_Data->s_Level->s_Fields["FieldDoors"][TileCoordY[0]/16*SizeXLev + TileCoordX[0]/16] < 0 && testCollision(s_CoordX, s_CoordY, TileCoordX[0], TileCoordY[0], s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame], Square(0,0,15,15)) )
    {
        s_CoordX = TileCoordX[0];
        s_CoordY = TileCoordY[0];
        s_State = "dooropen";
        s_HowDoorOpen = "exit";
        s_TimeDoorOpen = atoi( s_GameClass->s_IniFile->getValue("settings","timedooropen").c_str() );
        return true;
    }
    else if( s_GameClass->s_Data->s_Level->s_Fields["FieldDoors"][TileCoordY[1]/16*SizeXLev + TileCoordX[1]/16] < 0 && testCollision(s_CoordX, s_CoordY, TileCoordX[1], TileCoordY[1], s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame], Square(0,0,15,15)) )
    {
        s_CoordX = TileCoordX[1];
        s_CoordY = TileCoordY[1];
        s_State = "dooropen";
        s_HowDoorOpen = "exit";
        s_TimeDoorOpen = atoi( s_GameClass->s_IniFile->getValue("settings","timedooropen").c_str() );
        return true;
    }
    else if( s_GameClass->s_Data->s_Level->s_Fields["FieldExitLevelDoors"][TileCoordY[0]/16*SizeXLev + TileCoordX[0]/16] < 0 && testCollision(s_CoordX, s_CoordY, TileCoordX[0], TileCoordY[0], s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame], Square(0,0,15,15)) )
    {
        s_CoordX = TileCoordX[0];
        s_CoordY = TileCoordY[0];
        s_State = "dooropen";
        s_HowDoorOpen = "exitlevel";
        s_TimeDoorOpen = atoi( s_GameClass->s_IniFile->getValue("settings","timedooropen").c_str() );
        return true;
    }
    else if( s_GameClass->s_Data->s_Level->s_Fields["FieldExitLevelDoors"][TileCoordY[1]/16*SizeXLev + TileCoordX[1]/16] < 0 && testCollision(s_CoordX, s_CoordY, TileCoordX[1], TileCoordY[1], s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame], Square(0,0,15,15)) )
    {
        s_CoordX = TileCoordX[1];
        s_CoordY = TileCoordY[1];
        s_State = "dooropen";
        s_HowDoorOpen = "exitlevel";
        s_TimeDoorOpen = atoi( s_GameClass->s_IniFile->getValue("settings","timedooropen").c_str() );
        return true;
    }
    else if( s_GameClass->s_Data->s_Level->s_Fields["FieldDoors"][TileCoordY[0]/16*SizeXLev + TileCoordX[0]/16] > 0 && testCollision(s_CoordX, s_CoordY, TileCoordX[0], TileCoordY[0], s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame], Square(0,0,15,15)) )
    {
        s_CoordX = TileCoordX[0] - 10;
        s_CoordY = TileCoordY[0];
        s_State = "doorexit";
        s_HowDoorOpen = "exit";
        s_NumberOfAction = -1 + s_GameClass->s_AnimationStep - s_GameClass->s_AnimationStep%atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("info","numberofframesdoorexit").c_str() );
        s_TimeDoorOpen = atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("info","numberofframesdoorexit").c_str() );
        s_OldAnSt = s_GameClass->s_AnimationStep;
        return true;
    }
    else if( s_GameClass->s_Data->s_Level->s_Fields["FieldDoors"][TileCoordY[1]/16*SizeXLev + TileCoordX[1]/16] > 0 && testCollision(s_CoordX, s_CoordY, TileCoordX[1], TileCoordY[1], s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame], Square(0,0,15,15)) )
    {
        s_CoordX = TileCoordX[1] - 10;
        s_CoordY = TileCoordY[1];
        s_State = "doorexit";
        s_HowDoorOpen = "exit";
        s_NumberOfAction = -1 + s_GameClass->s_AnimationStep - s_GameClass->s_AnimationStep%atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("info","numberofframesdoorexit").c_str() );
        s_TimeDoorOpen = atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("info","numberofframesdoorexit").c_str() );
        s_OldAnSt = s_GameClass->s_AnimationStep;
        return true;
    }
    else if( s_GameClass->s_Data->s_Level->s_Fields["FieldExitLevelDoors"][TileCoordY[0]/16*SizeXLev + TileCoordX[0]/16] > 0 && testCollision(s_CoordX, s_CoordY, TileCoordX[0], TileCoordY[0], s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame], Square(0,0,15,15)) )
    {
        s_CoordX = TileCoordX[0] - 10;
        s_CoordY = TileCoordY[0];
        s_State = "doorexit";
        s_HowDoorOpen = "exitlevel";
        s_NumberOfAction = -1 + s_GameClass->s_AnimationStep - s_GameClass->s_AnimationStep%atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("info","numberofframesdoorexit").c_str() );
        s_TimeDoorOpen = atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("info","numberofframesdoorexit").c_str() );
        s_OldAnSt = s_GameClass->s_AnimationStep;
        return true;
    }
    else if( s_GameClass->s_Data->s_Level->s_Fields["FieldExitLevelDoors"][TileCoordY[1]/16*SizeXLev + TileCoordX[1]/16] > 0 && testCollision(s_CoordX, s_CoordY, TileCoordX[1], TileCoordY[1], s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame], Square(0,0,15,15)) )
    {
        s_CoordX = TileCoordX[1] - 10;
        s_CoordY = TileCoordY[1];
        s_State = "doorexit";
        s_HowDoorOpen = "exitlevel";
        s_NumberOfAction = -1 + s_GameClass->s_AnimationStep - s_GameClass->s_AnimationStep%atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("info","numberofframesdoorexit").c_str() );
        s_TimeDoorOpen = atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("info","numberofframesdoorexit").c_str() );
        s_OldAnSt = s_GameClass->s_AnimationStep;
        return true;
    }
    return false;
}

bool Creature::testChangeLevel()
{
    int TileCoordX[6];
    int TileCoordY[6];
    TileCoordX[0] = roundNumber(s_CoordX, 16, -1);
    TileCoordX[1] = roundNumber(s_CoordX, 16, -1) + 16;
    TileCoordX[2] = roundNumber(s_CoordX, 16, -1);
    TileCoordX[3] = roundNumber(s_CoordX, 16, -1) + 16;
    TileCoordX[4] = roundNumber(s_CoordX, 16, -1);
    TileCoordX[5] = roundNumber(s_CoordX, 16, -1) + 16;
    TileCoordY[0] = roundNumber(s_CoordY, 16, -1);
    TileCoordY[1] = roundNumber(s_CoordY, 16, -1);
    TileCoordY[2] = roundNumber(s_CoordY, 16, -1) + 16;
    TileCoordY[3] = roundNumber(s_CoordY, 16, -1) + 16;
    TileCoordY[4] = roundNumber(s_CoordY, 16, -1) + 32;
    TileCoordY[5] = roundNumber(s_CoordY, 16, -1) + 32;
    int TileType = 0;
    int numberofframes = atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("info", "numberofframes" + s_State).c_str() );
    int frame = s_NumberOfAction%numberofframes;
    for(int i = 0; i < 6; i++)
    {
        TileType = s_GameClass->s_Data->s_Level->getTileType(TileCoordX[i]/16, TileCoordY[i]/16);
        if( TileType == EXITLEVEL && testCollision(s_CoordX, s_CoordY, TileCoordX[i], TileCoordY[i], s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame], Square(0,0,15,15)) ) return true;
    }
    return false;
}

int Creature::testDeath()
{
    int TileCoordX[6];
    int TileCoordY[6];
    TileCoordX[0] = roundNumber(s_CoordX, 16, -1);
    TileCoordX[1] = roundNumber(s_CoordX, 16, -1) + 16;
    TileCoordX[2] = roundNumber(s_CoordX, 16, -1);
    TileCoordX[3] = roundNumber(s_CoordX, 16, -1) + 16;
    TileCoordX[4] = roundNumber(s_CoordX, 16, -1);
    TileCoordX[5] = roundNumber(s_CoordX, 16, -1) + 16;
    TileCoordY[0] = roundNumber(s_CoordY, 16, -1);
    TileCoordY[1] = roundNumber(s_CoordY, 16, -1);
    TileCoordY[2] = roundNumber(s_CoordY, 16, -1) + 16;
    TileCoordY[3] = roundNumber(s_CoordY, 16, -1) + 16;
    TileCoordY[4] = roundNumber(s_CoordY, 16, -1) + 32;
    TileCoordY[5] = roundNumber(s_CoordY, 16, -1) + 32;
    int TileType = 0;
    int numberofframes = atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("info", "numberofframes" + s_State).c_str() );
    int frame = s_NumberOfAction%numberofframes;
    int sizeXLevel = atoi( s_GameClass->s_Data->s_Level->s_Params->getValue("info","sizeX").c_str() );
    for(int i = 0; i < 6; i++)
    {
        TileType = s_GameClass->s_Data->s_Level->getTileType(TileCoordX[i]/16, TileCoordY[i]/16);
        if( TileType == DEATH && testCollision(s_CoordX, s_CoordY, TileCoordX[i], TileCoordY[i], s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame], Square(0,0,15,15)) ) return (TileCoordY[i]*sizeXLevel/16+TileCoordX[i]/16)*(-1);
    }
    return 0;
}

void Creature::testGetBonuses()
{
    int TileCoordX[6];
    int TileCoordY[6];
    TileCoordX[0] = roundNumber(s_CoordX, 16, -1);
    TileCoordX[1] = roundNumber(s_CoordX, 16, -1) + 16;
    TileCoordX[2] = roundNumber(s_CoordX, 16, -1);
    TileCoordX[3] = roundNumber(s_CoordX, 16, -1) + 16;
    TileCoordX[4] = roundNumber(s_CoordX, 16, -1);
    TileCoordX[5] = roundNumber(s_CoordX, 16, -1) + 16;
    TileCoordY[0] = roundNumber(s_CoordY, 16, -1);
    TileCoordY[1] = roundNumber(s_CoordY, 16, -1);
    TileCoordY[2] = roundNumber(s_CoordY, 16, -1) + 16;
    TileCoordY[3] = roundNumber(s_CoordY, 16, -1) + 16;
    TileCoordY[4] = roundNumber(s_CoordY, 16, -1) + 32;
    TileCoordY[5] = roundNumber(s_CoordY, 16, -1) + 32;
    int numberofframes = atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("info", "numberofframes" + s_State).c_str() );
    int frame = s_NumberOfAction%numberofframes;
    int bonus, frame_bonus, CrPoints;
    int SizeXLev = atoi( ( s_GameClass->s_Data->s_Level->s_Params->getValue("info", "sizeX") ).c_str() );
    for(int i = 0; i < 6; i++)
    {
        bonus = s_GameClass->s_Data->s_Level->s_Fields["FieldBonuses"][ TileCoordY[i]*SizeXLev/16 + TileCoordX[i]/16 ];
        if(bonus != 0)
        {
            numberofframes = atoi ( s_GameClass->s_Data->s_Bonuses->s_BonusesInfo[bonus-1]->getValue("info", "numberofframes").c_str() );
            frame_bonus = s_GameClass->s_AnimationStep%numberofframes + 1;
            if(testCollision(s_CoordX, s_CoordY, TileCoordX[i], TileCoordY[i], s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame], s_GameClass->s_Data->s_Bonuses->s_Collisions[bonus][frame_bonus] ) )
            {
                CrPoints = s_CurrentPoints;
                s_GameClass->s_Data->s_Level->s_Fields["FieldBonuses"][ TileCoordY[i]*SizeXLev/16 + TileCoordX[i]/16 ] = 0;
                s_CurrentPoints += atoi( s_GameClass->s_Data->s_Bonuses->s_BonusesInfo[bonus-1]->getValue("info", "point").c_str() );
                s_GameClass->s_GameInfo->s_CurrentLives += atoi( s_GameClass->s_Data->s_Bonuses->s_BonusesInfo[bonus-1]->getValue("info", "up").c_str() );
                s_GameClass->s_FactoryTmpImgs->addImage(s_GameClass->s_Data->s_Bonuses->s_BonusesBitmaps[bonus-1][numberofframes+1],
                                                        s_GameClass->s_Data->s_Bonuses->s_BonusesCache[bonus-1][numberofframes+1],
                                                        TileCoordX[i] + s_GameClass->s_Data->s_Bonuses->s_Collisions[bonus][frame_bonus].s_XL,
                                                        TileCoordY[i] + s_GameClass->s_Data->s_Bonuses->s_Collisions[bonus][frame_bonus].s_YL,
                                                        atoi(s_GameClass->s_IniFile->getValue("effects","timelivepointseffect").c_str() ),
                                                        atoi(s_GameClass->s_IniFile->getValue("effects","changeXpointseffect").c_str() ),
                                                        atoi(s_GameClass->s_IniFile->getValue("effects","changeYpointseffect").c_str() ),
                                                        "points");
                for(int i = s_CurrentPoints; i > CrPoints; i--)
                {
                    if(i% atoi( s_GameClass->s_Data->s_Bonuses->s_GlobBonusesInfo->getValue("info", "pointforup").c_str() ) == 0)
                    {
                        s_GameClass->s_GameInfo->s_CurrentLives++;
                    }
                }
            }
        }
    }
}

void Creature::step(string direction)
{
    int wh = -1;
    if(s_State.find("run") == string::npos) s_NumberOfAction = 0;
    if(direction == "right") wh = 1;
    if(s_State.find("jump") == string::npos) s_State = direction + "run";
    else s_State = direction + s_State.substr(s_State.find("jump"));
    s_CoordX += 4*wh;
    correctionPhys(s_CoordX - 4*wh, 0);
}

bool Creature::correctionPhys(int coord, int what)
{
    bool yes = false;
    if(s_GameClass->s_IniFile->getValue("settings", "correctionphysics") == "false") return false;
    int TileCoordX[6];
    int TileCoordY[6];
    TileCoordX[0] = roundNumber(s_CoordX, 16, -1);
    TileCoordX[1] = roundNumber(s_CoordX, 16, -1) + 16;
    TileCoordX[2] = roundNumber(s_CoordX, 16, -1);
    TileCoordX[3] = roundNumber(s_CoordX, 16, -1) + 16;
    TileCoordX[4] = roundNumber(s_CoordX, 16, -1);
    TileCoordX[5] = roundNumber(s_CoordX, 16, -1) + 16;
    TileCoordY[0] = roundNumber(s_CoordY, 16, -1);
    TileCoordY[1] = roundNumber(s_CoordY, 16, -1);
    TileCoordY[2] = roundNumber(s_CoordY, 16, -1) + 16;
    TileCoordY[3] = roundNumber(s_CoordY, 16, -1) + 16;
    TileCoordY[4] = roundNumber(s_CoordY, 16, -1) + 32;
    TileCoordY[5] = roundNumber(s_CoordY, 16, -1) + 32;
    bool col = false;
    int sign;
    if(what == 0) sign = (s_CoordX - coord)/abs(s_CoordX - coord);
    if(what == 1) sign = (s_CoordY - coord)/abs(s_CoordY - coord);
    int TileType = 0;
    int numberofframes = atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("info", "numberofframes" + s_State).c_str() );
    int frame = s_NumberOfAction%numberofframes;
    do
    {
        col = false;
        for(int i = 0; i < 6; i++)
        {
            TileType = s_GameClass->s_Data->s_Level->getTileType(TileCoordX[i]/16, TileCoordY[i]/16);
            if( TileType == IMPASSABLE && testCollision(s_CoordX, s_CoordY, TileCoordX[i], TileCoordY[i], s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame], Square(0,0,15,15)) ) col = true;
            if( what == 1 && sign == 1 && TileType == LADDER &&
               testCollision(s_CoordX, s_CoordY, TileCoordX[i], TileCoordY[i],
                             s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame],
                             Square(0,0,15,15)) &&
               !testCollision(s_CoordX, coord, TileCoordX[i], TileCoordY[i],
                              s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame],
                              Square(0,0,15,15), true) &&
               !testCollision(s_CoordX, coord, TileCoordX[i], TileCoordY[i],
                              s_GameClass->s_Data->s_Dave->s_Collisions[s_State][frame],
                              Square(0,0,15,15)) ) col = true;
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
        if(what == 0) cout<<"(Dave) Physics correction X: "<<"old - "<<coord<<", new - "<<s_CoordX<<endl;
        else if(what == 1) cout<<"(Dave) Physics correction Y: "<<"old - "<<coord<<", new - "<<s_CoordY<<endl;
    }
    return yes;
}

bool Creature::testSetStates(string states)
{
    std::istringstream ss(states);
    std::string st;
    while( (ss >> st) && (st != s_State));
    return st == s_State;
}

int Creature::getFrame()
{
    return s_NumberOfAction%atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("info", "numberofframes" + s_State).c_str() );
}

int Creature::getFrame(string anim)
{
    return s_NumberOfAction%atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("info", "numberofframes" + anim).c_str() );
}

void Creature::draw()
{
    int ScrLX = s_GameClass->s_GameInfo->s_ScreenCoordX;
    int ScrLY = s_GameClass->s_GameInfo->s_ScreenCoordY;
    s_GameClass->s_Data->s_Dave->drawDave(s_State, getFrame(), s_CoordX - ScrLX, s_CoordY - ScrLY);
}

void Creature::testShoot()
{
    string statedave;
    string str = s_State.substr(0, s_State.find("shoot"));
    int coordfireX = s_CoordX + atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("shoot", "coordfire" + str + "X").c_str() );
    int coordfireY = s_CoordY + atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("shoot", "coordfire" + str + "Y").c_str() );
    int anglefire = atoi( s_GameClass->s_Data->s_Dave->s_DaveInfo->getValue("shoot", "anglefire" + str).c_str() );
    double pi = 3.1415926535;
    double anglefirerad = double(anglefire)*pi/180;
    double xc = double(coordfireX);
    double yc = double(coordfireY);
    int xcc = xc, ycc = yc;
    int stepcalculateshoot = atoi( s_GameClass->s_IniFile->getValue("settings", "stepcalculateshoot").c_str() );
    for(double t = 0; xcc <= s_GameClass->s_DisplayStruct->s_ResolutionX + s_ScreenCoordX + 16*atoi(s_GameClass->s_IniFile->getValue("settings","distancelivemonstersX").c_str() ) &&
            xcc >= s_ScreenCoordX - 16*atoi(s_GameClass->s_IniFile->getValue("settings","distancelivemonstersX").c_str() ) &&
            ycc <= s_GameClass->s_DisplayStruct->s_ResolutionY + s_ScreenCoordY + 16*atoi(s_GameClass->s_IniFile->getValue("settings","distancelivemonstersY").c_str() ) &&
            ycc >= s_ScreenCoordY - 16*atoi(s_GameClass->s_IniFile->getValue("settings","distancelivemonstersY").c_str() ); t += stepcalculateshoot)
    {
        if(atoi( s_GameClass->s_IniFile->getValue("settings", "maxdistanceshoot").c_str() ) != -1 && t > atoi( s_GameClass->s_IniFile->getValue("settings", "maxdistanceshoot").c_str() )) break;
        double xc = double(coordfireX) + t*cos(anglefirerad);
        double yc = double(coordfireY) - t*sin(anglefirerad);
        xcc = xc;
        ycc = yc;
        int tileX = roundNumber(xcc, 16, -1)/16;
        int tileY = roundNumber(ycc, 16, -1)/16;
        if(s_GameClass->s_Data->s_Level->getTileType(tileX, tileY) == IMPASSABLE)
        {
            //int SizeXLev = atoi( ( s_GameClass->s_Data->s_Level->s_Params->getValue("info", "sizeX") ).c_str() );
            //s_GameClass->s_Data->s_Level->s_Fields["FieldTiles"][tileY*SizeXLev + tileX] = 0;
            statedave = "traceshoot";
            int tt;
            for(tt = t; testCollision(xcc, ycc, tileX*16, tileY*16, s_GameClass->s_Data->s_Dave->s_Collisions[statedave][getFrame(statedave)], Square(0,0,15,15)); tt--)
            {
                xc = double(coordfireX) + tt*cos(anglefirerad);
                yc = double(coordfireY) - tt*sin(anglefirerad);
                xcc = xc;
                ycc = yc;
            }
            xc = double(coordfireX) + (tt + 4)*cos(anglefirerad);
            yc = double(coordfireY) - (tt + 4)*sin(anglefirerad);
            xcc = xc;
            ycc = yc;
            s_GameClass->s_FactoryTmpImgs->addImage(s_GameClass->s_Data->s_Dave->s_Bitmaps[statedave][getFrame(statedave)],
                                                        s_GameClass->s_Data->s_Dave->s_CacheImages[statedave][getFrame(statedave)],
                                                        xcc, ycc, 4, 0, 0, "traceshoot");
            return;
        }
        map<int, CreatureMonster*>::iterator iter;
        for ( iter = s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters.begin(); iter != s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters.end(); iter++)
            if(iter->second->s_CoordX <= s_GameClass->s_DisplayStruct->s_ResolutionX + s_ScreenCoordX + 16*atoi(s_GameClass->s_IniFile->getValue("settings","distancelivemonstersX").c_str() ) &&
                iter->second->s_CoordX >= s_ScreenCoordX - 16*atoi(s_GameClass->s_IniFile->getValue("settings","distancelivemonstersX").c_str() ) &&
                iter->second->s_CoordY <= s_GameClass->s_DisplayStruct->s_ResolutionY + s_ScreenCoordY + 16*atoi(s_GameClass->s_IniFile->getValue("settings","distancelivemonstersY").c_str() ) &&
                iter->second->s_CoordY >= s_ScreenCoordY - 16*atoi(s_GameClass->s_IniFile->getValue("settings","distancelivemonstersY").c_str() ))
                        if(iter->second->s_DeleteNow == false && iter->second->s_CurrentLives != -2)
                        {
                            statedave = "traceshoot";
                            if(testCollision(xcc, ycc, iter->second->s_CoordX, iter->second->s_CoordY, s_GameClass->s_Data->s_Dave->s_Collisions[statedave][getFrame(statedave)], s_GameClass->s_Data->s_Monsters->s_Collisions[iter->second->s_Number - 1][iter->second->s_State][iter->second->getFrame()]))
                            {
                                if(iter->second->s_CurrentLives > 0)
                                {
                                    iter->second->s_CurrentLives--;
                                    info_for_spec_func infofsf;
                                    infofsf.damage = 1;
                                    s_GameClass->s_AI->runSpecialFunction(iter->second, TSF_ON_DAMAGE, infofsf);
                                }
                                if(iter->second->s_CurrentLives == 0) iter->second->kill(0);
                                statedave = "traceshoot";
                                s_GameClass->s_FactoryTmpImgs->addImage(s_GameClass->s_Data->s_Dave->s_Bitmaps[statedave][getFrame(statedave)],
                                                        s_GameClass->s_Data->s_Dave->s_CacheImages[statedave][getFrame(statedave)],
                                                        xcc, ycc, 4, 0, 0, "traceshoot");
                                return;
                            }
                        }
    }
}

void Creature::calculateDoKey()
{
    if( !(s_KeysState->s_KeyShoot) )
    {
        s_ControlShootPressed = false;
    }
    if(s_ShootNow > 0) return;
    bool command = false;
    string where;
    if( testSetStates("leftupshoot rightupshoot leftdownshoot rightdownshoot rightrun leftrun rightstand leftstand recharge") && (s_KeysState->s_KeyLeft) && (s_KeysState->s_KeyUp) && command == false)
    {
        s_State = "leftupshoot";
    }
    else if( testSetStates("leftupshoot rightupshoot leftdownshoot rightdownshoot rightrun leftrun rightstand leftstand recharge") && (s_KeysState->s_KeyLeft) && (s_KeysState->s_KeyDown) && command == false)
    {
        s_State = "leftdownshoot";
    }
    else if( testSetStates("leftupshoot rightupshoot leftdownshoot rightdownshoot rightrun leftrun rightstand leftstand recharge") && (s_KeysState->s_KeyRight) && (s_KeysState->s_KeyUp) && command == false)
    {
        s_State = "rightupshoot";
    }
    else if( testSetStates("leftupshoot rightupshoot leftdownshoot rightdownshoot rightrun leftrun rightstand leftstand recharge") && (s_KeysState->s_KeyRight) && (s_KeysState->s_KeyDown) && command == false)
    {
        s_State = "rightdownshoot";
    }
    else if( testSetStates("leftjumpdown leftjumpstand leftjumpup leftrun leftstand leftupshoot recharge rightdownshoot rightjumpdown rightjumpstand rightjumpup rightrun rightstand rightupshoot leftdownshoot") && (s_KeysState->s_KeyLeft) && command == false) //стрелка влево
    {
        step("left");
    }
    else if( testSetStates("leftjumpdown leftjumpstand leftjumpup leftrun leftstand leftupshoot recharge rightdownshoot rightjumpdown rightjumpstand rightjumpup rightrun rightstand rightupshoot leftdownshoot") && (s_KeysState->s_KeyRight) && command == false) //стрелка вправо
    {
        step("right");
    }
    else if( testSetStates("leftupshoot rightupshoot leftdownshoot rightdownshoot rightrun leftrun rightstand leftstand recharge") && (s_KeysState->s_KeyUp) && command == false) //стрелка вверх
    {
        if(!testOpenDoor())
        {
            s_ControlJumpPressed = false;
            if(s_State == "recharge") s_State = s_DopState;
            if(s_State.find("right") == 0) where = "right";
            else where = "left";
            s_State = where + "upshoot";
        }
    }
    else if( testSetStates("leftupshoot rightupshoot leftdownshoot rightdownshoot rightrun leftrun rightstand leftstand recharge") && (s_KeysState->s_KeyDown) && command == false) //стрелка вниз
    {
        if(s_State == "recharge") s_State = s_DopState;
        if(s_State.find("right") == 0) where = "right";
        else where = "left";
        s_State = where + "downshoot";
    }
    if(testSetStates("leftupshoot rightupshoot leftdownshoot rightdownshoot rightrun leftrun rightstand leftstand recharge") && (s_KeysState->s_KeyShoot) && s_ControlShootPressed == false)
    {
        s_ControlShootPressed = true;
        if(s_State == "recharge") s_State = s_DopState;
        s_ShootNow = atoi( s_GameClass->s_IniFile->getValue("settings", "timeshoot").c_str() );
        if(testSetStates("rightrun rightstand")) s_State = "rightshoot";
        else if(testSetStates("leftrun leftstand")) s_State = "leftshoot";
        else s_ShootNow = 2*atoi( s_GameClass->s_IniFile->getValue("settings", "timeshoot").c_str() )/3 + 1;
    }
    if( testSetStates("recharge rightjumpup leftjumpup leftupshoot rightupshoot leftdownshoot rightdownshoot rightrun leftrun rightstand leftstand recharge") && (s_KeysState->s_KeyJump) )
    {
        if(s_State.find("jumpup") != string::npos && s_ControlJumpPressed == true && s_NumberOfTilesJump != atoi( s_GameClass->s_IniFile->getValue("settings", "maxnumberoftilesforjump").c_str() ) )
        {
            s_JumpStep += atoi( s_GameClass->s_IniFile->getValue("settings", "jumpstep").c_str() );
            s_NumberOfTilesJump++;
        }
        else if( (s_State == "recharge" || s_State.find("stand") != string::npos || s_State.find("run") != string::npos || s_State.find("upshoot") != string::npos) && s_ControlJumpPressed == false )
        {
            s_ControlJumpPressed = true;
            s_JumpStep = atoi( s_GameClass->s_IniFile->getValue("settings", "jumpstep").c_str() );
            s_NumberOfTilesJump = 1;
            if(s_State == "recharge") s_State = s_DopState;
            if(s_State.find("right") == 0) where = "right";
            else where = "left";
            s_State = where + "jumpup";
        }
    }
    if( testSetStates("rightdownshoot leftdownshoot") && (s_KeysState->s_KeyJump) )
    {
        s_CoordY += 8;
        correctionPhys(s_CoordY - 8, 1);
    }
    if( !(s_KeysState->s_KeyJump) )
    {
        s_ControlJumpPressed = false;
    }
    if( !(s_KeysState->s_KeyShoot) )
    {
        s_ControlShootPressed = false;
    }
    if( !(s_KeysState->s_KeyLeft) && (s_State == "leftrun") )
    {
        s_State = "leftstand";
    }
    if( !(s_KeysState->s_KeyRight) && (s_State == "rightrun") )
    {
        s_State = "rightstand";
    }
    if(s_ShootNow == 0)
    {
        if( !(s_KeysState->s_KeyLeft) && !(s_KeysState->s_KeyUp) && (s_State == "leftupshoot") )
        {
            s_State = "leftstand";
        }
        if( !(s_KeysState->s_KeyLeft) && !(s_KeysState->s_KeyDown) && (s_State == "leftdownshoot") )
        {
            s_State = "leftstand";
        }
        if( !(s_KeysState->s_KeyRight) && !(s_KeysState->s_KeyUp) && (s_State == "rightupshoot") )
        {
            s_State = "rightstand";
        }
        if( !(s_KeysState->s_KeyRight) && !(s_KeysState->s_KeyDown) && (s_State == "rightdownshoot") )
        {
            s_State = "rightstand";
        }
    }
}

//...

PostParsingStruct* Creature::getKeys()
{
    PostParsingStruct* cpps = new PostParsingStruct;
    cpps->setValue("Keys", "s_KeyLeft", WorkFunction::ConvertFunction::itos( (int)s_KeysState->s_KeyLeft) );
    cpps->setValue("Keys", "s_KeyRight", WorkFunction::ConvertFunction::itos( (int)s_KeysState->s_KeyRight) );
    cpps->setValue("Keys", "s_KeyUp", WorkFunction::ConvertFunction::itos( (int)s_KeysState->s_KeyUp) );
    cpps->setValue("Keys", "s_KeyDown", WorkFunction::ConvertFunction::itos( (int)s_KeysState->s_KeyDown) );
    cpps->setValue("Keys", "s_KeyShoot", WorkFunction::ConvertFunction::itos( (int)s_KeysState->s_KeyShoot) );
    cpps->setValue("Keys", "s_KeyJump", WorkFunction::ConvertFunction::itos( (int)s_KeysState->s_KeyJump) );
    return cpps;
}

void Creature::setKeys(PostParsingStruct* cpps)
{
    s_KeysState->s_KeyLeft = (bool)atoi( cpps->getValue("Keys", "s_KeyLeft").c_str() );
    s_KeysState->s_KeyRight = (bool)atoi( cpps->getValue("Keys", "s_KeyRight").c_str() );
    s_KeysState->s_KeyUp = (bool)atoi( cpps->getValue("Keys", "s_KeyUp").c_str() );
    s_KeysState->s_KeyDown = (bool)atoi( cpps->getValue("Keys", "s_KeyDown").c_str() );
    s_KeysState->s_KeyShoot = (bool)atoi( cpps->getValue("Keys", "s_KeyShoot").c_str() );
    s_KeysState->s_KeyJump = (bool)atoi( cpps->getValue("Keys", "s_KeyJump").c_str() );
}

void Creature::mergeDave(Creature* cr)
{
        s_CurrentPoints = cr->s_CurrentPoints;
        s_CurrentHealth = cr->s_CurrentHealth;
        s_MaxHealth = cr->s_MaxHealth;
        s_Cartridges = cr->s_Cartridges;
        s_MaxCartridges = cr->s_MaxCartridges;
        s_CoordX = cr->s_CoordX;
        s_CoordY = cr->s_CoordY;
        s_State = cr->s_State;
        s_NumberOfAction = cr->s_NumberOfAction;
        s_JumpStep = cr->s_JumpStep;
        s_ControlJumpPressed = cr->s_ControlJumpPressed;
        s_ControlShootPressed = cr->s_ControlShootPressed;
        s_NumberOfTilesJump = cr->s_NumberOfTilesJump;
        s_FreezeJump = cr->s_FreezeJump;
        s_Acceleration = cr->s_Acceleration;
        s_TimeDoorOpen = cr->s_TimeDoorOpen;
        s_StateBeforeOpenDoor = cr->s_StateBeforeOpenDoor;
        s_HowDoorOpen = cr->s_HowDoorOpen;
        s_OldAnSt = cr->s_OldAnSt;
        s_OldNumberOfAction = cr->s_OldNumberOfAction;
        s_AdditionalNumberOfAction = cr->s_AdditionalNumberOfAction;
        s_ShootNow = cr->s_ShootNow;
        s_DopState = cr->s_DopState;
        s_ScreenCoordX = cr->s_ScreenCoordX;
        s_ScreenCoordY = cr->s_ScreenCoordY;
        s_NickName = cr->s_NickName;
}
