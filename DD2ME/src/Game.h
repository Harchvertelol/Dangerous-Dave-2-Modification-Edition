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
#include "canvas.hpp"

class Game
{
    public:
        Game();
        ~Game();
        Window* s_Window;
        GameInfo* s_GameInfo;
        DisplayStruct* s_DisplayStruct;
        IniParser::PostParsingStruct* s_IniFile;
        GameData* s_Data;
        StateManager* s_StateManager;
        FactoryTemporaryImages* s_FactoryTmpImgs;
        AI* s_AI;
        Gui* s_Gui;
        NetClient* s_NetClient;
        unsigned int s_AnimationStep;
        unsigned int s_IdGameClass;
        unsigned int s_IdTimerAnimationStep;
        unsigned int s_IdTimerCreaturesAnimationStep;
        unsigned int s_IdTimerDrawStep;
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
        bool changeLevel(int, bool switchstate = true);
        bool changeNextLevel(bool switchstate = true);
        void onTimer(unsigned int);
        void configureForGame();
        IniParser::PostParsingStruct* getObjects();
        void setObjects(IniParser::PostParsingStruct*);
        void processAllEvents(int);
        bool insertDave(int, int, std::string);
        bool removeDave(int);
        void deleteAllGDIObjects();
};
#endif
