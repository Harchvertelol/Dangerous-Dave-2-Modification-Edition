#ifndef GAMEINFO_H
#define GAMEINFO_H
#include <map>
#include "CreaturePlayer.h"
#include "FactoryMonsters.h"
#include <SFML/Graphics.hpp>

class Game;

class GameInfo
{
    public:
        GameInfo(Game*);
        ~GameInfo();
        Game* s_GameClass;
        int s_CurrentLevel;
        int s_ChangeLevelTo;
        int s_GameState;
        int s_CurrentLives;
        std::map<int, CreaturePlayer*> s_Players;
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
        int s_KeyLeftUp;
        int s_KeyRightUp;
        int s_KeyLeftDown;
        int s_KeyRightDown;
        int s_KeyShoot;
        int s_KeyJump;
        int s_KeySkip;
        int s_KeyConsole;
        int s_KeyInfo;
        unsigned int s_OldAnSt;
        CreaturePlayer* s_MyPlayer;
        int s_CurrentDistanceLiveMonstersX;
        int s_CurrentDistanceLiveMonstersY;
        int s_CurrentDistanceActivateMonstersX;
        int s_CurrentDistanceActivateMonstersY;
        bool s_ForcedChangeLevel;
        bool s_CheatGod;
        bool s_IsAIOn;
        bool s_IsGhostOn;
        bool s_IsInfoShow;
        void readKeys(IniParser::PostParsingStruct*);
        void live();
        bool deathPlayer(int);
        int getChangeLevelTo(std::string);
        void doChangeLevelOnGameOver();
        void playDeath();
        void drawDeathFrame(std::map<int, sf::Texture* >*, std::map<int, sf::Sprite* >*, int);
        void correctionScreen(CreaturePlayer*);
};
#endif
