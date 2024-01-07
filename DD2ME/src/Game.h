#ifndef GAME_H
#define GAME_H
#include "DisplayStruct.h"
#include "IniParser/PostParsingStruct.h"
#include "GameData.h"
#include "GameInfo.h"
#include "StateManager.h"
#include "FactoryTemporaryImages.h"
#include "NetClient/NetClient.h"
#include "AI.h"
#include "Gui.h"
#include "KeysState.h"
#include "Timers.h"
#include "Logger.h"
#include <SFML/Graphics.hpp>

class Game
{
    public:
        Game();
        ~Game();
        sf::RenderWindow* s_RenderWindow;
        sf::RenderTexture* s_RenderTexture;
        GameInfo* s_GameInfo;
        DisplayStruct* s_DisplayStruct;
        IniParser::PostParsingStruct* s_IniFile;
        GameData* s_Data;
        StateManager* s_StateManager;
        FactoryTemporaryImages* s_FactoryTmpImgs;
        AI* s_AI;
        Gui* s_Gui;
        NetClient* s_NetClient;
        Timers s_Timers;
        Logger* s_Logger;
        float s_GameRenderScale;
        unsigned int s_AnimationStep;
        unsigned int s_TileAnimationStep;
        unsigned int s_IdGameClass;
        unsigned int s_IdTimerAnimationStep;
        unsigned int s_IdTimerCreaturesAnimationStep;
        unsigned int s_IdTimerDrawStep;
        unsigned int s_IdTimerTilesAnimationStep;
        unsigned int s_IdTimerAIRunStep;
        // FPS
        unsigned int s_GameDrawFPS;
        unsigned int s_GameDrawFPSMaximal;
        unsigned int s_GameDrawFPSMinimal;
        unsigned int s_GameDrawFPSCalc;
        time_t s_TimeStartGameDrawFPS;
        unsigned int s_TechnicalFPS;
        unsigned int s_TechnicalFPSMaximal;
        unsigned int s_TechnicalFPSMinimal;
        unsigned int s_TechnicalFPSCalc;
        time_t s_TimeStartTechnicalFPS;
        // Flags
        bool s_AIWasRunned;
        //...
        void calculateGameDrawFPS();
        unsigned int getGameDrawFPS();
        void calculateTechnicalFPS();
        unsigned int getTechnicalFPS();
        void calculateFPS(unsigned int*, unsigned int*, time_t*, unsigned int*, unsigned int*);
        //...
        bool loadPack();
        void startGame(int);
        void play();
        void correctionAllScreens();
        void drawAll();
        bool changeLevel(int, bool switchstate = true, bool playmusic = true);
        bool changeNextLevel(bool switchstate = true);
        std::string getNameForSaveFolder();
        std::string getNameForSaveFile();
        void onTimer(unsigned int);
        void configureForGame();
        IniParser::PostParsingStruct* getObjects(bool notfullfornetmode, CreaturePlayer* cr_player = 0);
        void setObjects(IniParser::PostParsingStruct*, bool notfullfornetmode);
        void processAllEvents(int);
        bool insertPlayer(int, int, std::string, bool iscontrolled = true);
        bool removePlayer(int);
        void deleteAllGDIObjects();
        bool createWindow();
        bool readConfig();
};
#endif
