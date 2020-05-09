#ifndef GAMEINFO_H
#define GAMEINFO_H
#include <map>
#include "canvas.hpp"
#include "CreatureDave.h"
#include "FactoryMonsters.h"

using namespace cnv;

class Game;

class GameInfo
{
    public:
        GameInfo(Game*);
        ~GameInfo();
        Game* s_GameClass;
        int s_CurrentLevel;
        int s_GameState;
        int s_CurrentLives;
        std::map<int, CreatureDave*> s_Daves;
        FactoryMonsters* s_FactoryMonsters;
        int s_ScreenCoordX;
        int s_ScreenCoordY;
        int s_DopScreenCoordX;
        int s_DopScreenCoordY;
        int s_DopFrame;
        bool s_Stop;
        int s_DeathType;
        int s_KeyLeft;
        int s_KeyRight;
        int s_KeyUp;
        int s_KeyDown;
        int s_KeyShoot;
        int s_KeyJump;
        unsigned int s_OldAnSt;
        CreatureDave* s_MyDave;
        bool s_CheatGod;
        bool s_IsAIOn;
        void live();
        void deathDave(int);
        void playDeath();
        void drawDeathFrame(std::map<int, Bitmap* >*, std::map<int, Bitmap* >*, int);
        void correctionScreen(CreatureDave*);
};
#endif
