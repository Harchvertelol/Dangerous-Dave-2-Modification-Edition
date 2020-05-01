#ifndef GAMEDATA_H
#define GAMEDATA_H
#include "Level.h"
#include "Textures.h"
#include "Bonuses.h"
#include "Screens.h"
#include "Dave.h"
#include "Monsters.h"

class Game;

class GameData
{
    public:
        GameData(Game*);
        ~GameData();
        bool s_ModSetted;
        string s_NameMod;
        string PathToTexturePack;
        string PathToMonsterPack;
        string PathToBonusPack;
        string PathToScreenPack;
        string PathToSoundPack;
        string PathToLevelPack;
        string PathToDavePack;
        PostParsingStruct* s_ModInfo;
        PostParsingStruct* s_LevelsInfo;
        Textures* s_Textures;
        Level* s_Level;
        Bonuses* s_Bonuses;
        Screens* s_Screens;
        Dave* s_Dave;
        Monsters* s_Monsters;
        Game* s_GameClass;
        bool loadData(PostParsingStruct*);
        bool drawScreenState0();
        bool drawScreenState1();
        bool drawScreenState2();
        bool drawScreenState3();
        void deleteAllGDIObjects();
};
#endif
