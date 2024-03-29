#ifndef CREATUREPLAYER_H
#define CREATUREPLAYER_H
#include <iostream>
#include "IniParser/PostParsingStruct.h"

#include "Square.h"

class Game;

class KeysState;

class CreaturePlayer
{
    public:
        CreaturePlayer(Game*);
        ~CreaturePlayer();
        Game* s_GameClass;
        int s_CurrentPoints;
        int s_CurrentHealth;
        int s_MaxHealth;
        int s_Cartridges;
        int s_MaxCartridges;
        int s_CoordX;
        int s_CoordY;
        std::string s_State;
        unsigned int s_NumberOfAction;
        int s_JumpStep;
        bool s_ControlJumpPressed;
        bool s_ControlShootPressed;
        int s_NumberOfTilesJump;
        int s_FreezeJump;
        int s_Acceleration;
        int s_TimeDoorOpen;
        std::string s_StateBeforeOpenDoor;
        std::string s_HowDoorOpen;
        unsigned int s_OldAnSt;
        unsigned int s_OldNumberOfAction;
        unsigned int s_AdditionalNumberOfAction;
        int s_ShootNow;
        std::string s_DopState;
        KeysState* s_KeysState;
        int s_ScreenCoordX;
        int s_ScreenCoordY;
        std::string s_NickName;
        IniParser::PostParsingStruct* s_Values;
        bool s_IsControlled;
        void live(bool doKey = true);
        bool correctionPhys(int,int);
        void step(std::string);
        void testGetBonuses();
        void addAdditionalUpsFromPoints(int, int, int);
        void addPointsEffect(std::string, int, int);
        int testDeath();
        int testChangeLevel();
        bool testOpenDoor();
        bool testSetStates(std::string);
        void testSmallPassage(int);
        int getFrame();
        int getFrame(std::string);
        void draw();
        void testShoot();
        void calculateDoKey();
        Square getCollision();
        IniParser::PostParsingStruct* getKeys(std::string, IniParser::PostParsingStruct* cpps = 0);
        void setKeys(IniParser::PostParsingStruct*, std::string);
        void mergePlayer(CreaturePlayer*);
        IniParser::PostParsingStruct* getListOfVariables(std::string, IniParser::PostParsingStruct* dpps = 0);
        void setListOfVariables(IniParser::PostParsingStruct*, std::string);
};
#endif
