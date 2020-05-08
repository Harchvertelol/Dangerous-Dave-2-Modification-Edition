#ifndef CREATURE_H
#define CREATURE_H
#include <iostream>
#include "IniParser/PostParsingStruct.h"
using namespace std;

class Game;

class KeysState;

class Creature
{
    public:
        Creature(Game*);
        ~Creature();
        Game* s_GameClass;
        int s_CurrentPoints;
        int s_CurrentHealth;
        int s_MaxHealth;
        int s_Cartridges;
        int s_MaxCartridges;
        int s_CoordX;
        int s_CoordY;
        string s_State;
        unsigned int s_NumberOfAction;
        int s_JumpStep;
        bool s_ControlJumpPressed;
        bool s_ControlShootPressed;
        int s_NumberOfTilesJump;
        int s_FreezeJump;
        int s_Acceleration;
        int s_TimeDoorOpen;
        string s_StateBeforeOpenDoor;
        string s_HowDoorOpen;
        unsigned int s_OldAnSt;
        unsigned int s_OldNumberOfAction;
        unsigned int s_AdditionalNumberOfAction;
        int s_ShootNow;
        string s_DopState;
        KeysState* s_KeysState;
        int s_ScreenCoordX;
        int s_ScreenCoordY;
        string s_NickName;
        void live();
        bool correctionPhys(int,int);
        void step(string);
        void testGetBonuses();
        int testDeath();
        bool testChangeLevel();
        bool testOpenDoor();
        bool testSetStates(string);
        void testSmallPassage(int);
        int getFrame();
        int getFrame(string);
        void draw();
        void testShoot();
        void calculateDoKey();
        PostParsingStruct* getKeys();
        void setKeys(PostParsingStruct*);
        void mergeDave(Creature*);
};
#endif
