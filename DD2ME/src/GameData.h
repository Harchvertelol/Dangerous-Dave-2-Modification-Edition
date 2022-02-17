#ifndef GAMEDATA_H
#define GAMEDATA_H
#include "Level.h"
#include "Textures.h"
#include "Bonuses.h"
#include "Screens.h"
#include "Dave.h"
#include "Monsters.h"
#include "Sounds.h"
#include "Music.h"
#include "GuiData.h"

class Game;

class GameData
{
    public:
        GameData(Game*);
        ~GameData();
        bool s_ModSetted;
        std::string s_NameMod;
        std::string PathToTexturePack;
        std::string PathToMonsterPack;
        std::string PathToBonusPack;
        std::string PathToScreenPack;
        std::string PathToSoundPack;
        std::string PathToMusicPack;
        std::string PathToLevelPack;
        std::string PathToDavePack;
        std::string PathToGuiPack;
        IniParser::PostParsingStruct* s_ModInfo;
        IniParser::PostParsingStruct* s_ModSettings;
        IniParser::PostParsingStruct* s_LevelsInfo;
        Textures* s_Textures;
        Level* s_Level;
        Bonuses* s_Bonuses;
        Screens* s_Screens;
        Dave* s_Dave;
        Monsters* s_Monsters;
        Sounds* s_Sounds;
        Music* s_Music;
        GuiData* s_GuiData;
        Game* s_GameClass;
        bool loadData(IniParser::PostParsingStruct*);
        bool drawScreenState0();
        bool drawScreenState1();
        bool drawScreenState2();
        bool drawScreenState3();
        void deleteAllGDIObjects();
        int compareGameVersions(std::string, std::string);
};
#endif
