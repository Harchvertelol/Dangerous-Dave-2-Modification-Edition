#include "GameData.h"

#include "Game.h"
#include "IniParser/ParserInfoFile.h"
#include "WorkFunctions.h"
#include "Defines.h"

using namespace IniParser;

using namespace std;

GameData::GameData(Game* gameclass):
    s_ModSetted(false),
    s_NameMod(""),
    PathToTexturePack(""),
    PathToMonsterPack(""),
    PathToBonusPack(""),
    PathToScreenPack(""),
    PathToSoundPack(""),
    PathToMusicPack(""),
    PathToLevelPack(""),
    PathToPlayerPack(""),
    PathToGuiPack(""),
    s_ModInfo(0),
    s_ModSettings(0),
    s_LevelsInfo(0),
    s_GameClass(gameclass)
{
    s_Level = new Level(gameclass);
    s_Textures = new Textures(gameclass);
    s_Bonuses = new Bonuses(gameclass);
    s_Screens = new Screens(gameclass);
    s_Player = new Player(gameclass);
    s_Monsters = new Monsters(gameclass);
    s_Sounds = new Sounds(gameclass);
    s_Music = new Music(gameclass);
    s_GuiData = new GuiData(gameclass);
    s_Backgrounds = new Backgrounds(gameclass);
}

GameData::~GameData()
{
    if(s_Level != 0) delete s_Level;
    if(s_LevelsInfo != 0) delete s_LevelsInfo;
    if(s_Textures != 0) delete s_Textures;
    if(s_ModInfo != 0) delete s_ModInfo;
    if(s_Bonuses != 0) delete s_Bonuses;
    if(s_Screens != 0) delete s_Screens;
    if(s_Player != 0) delete s_Player;
    if(s_Monsters != 0) delete s_Monsters;
    if(s_Sounds != 0) delete s_Sounds;
    if(s_Music != 0) delete s_Music;
    if(s_GuiData != 0) delete s_GuiData;
    if(s_Backgrounds != 0) delete s_Backgrounds;
}

void GameData::deleteAllGDIObjects()
{
    s_Textures->deleteAllGDIObjects();
    s_Bonuses->deleteAllGDIObjects();
    s_Screens->deleteAllGDIObjects();
    s_Player->deleteAllGDIObjects();
    s_Monsters->deleteAllGDIObjects();
}

int GameData::compareGameVersions(string first, string second)
{
    int first_level_version, second_level_version; // pre-beta = -2, beta = -1, pre-release = 0, release = 1
    map<int, int> first_mas, second_mas;

    int tmp_level_version = -2;
    string work_str = first;
    unsigned int str_vers_cut = work_str.find("pb");
    if(str_vers_cut == string::npos)
    {
        str_vers_cut = work_str.find("b");
        tmp_level_version = -1;
    }
    if(str_vers_cut == string::npos)
    {
        str_vers_cut = work_str.find("pr");
        tmp_level_version = 0;
    }
    if(str_vers_cut == string::npos)
    {
        str_vers_cut = work_str.size();
        tmp_level_version = 1;
    }
    work_str = work_str.substr(0, str_vers_cut);
    WorkFunctions::ParserFunctions::splitMass(&first_mas, 0, 0, work_str, ".");
    first_level_version = tmp_level_version;

    tmp_level_version = -2;
    work_str = second;
    str_vers_cut = work_str.find("pb");
    if(str_vers_cut == string::npos)
    {
        str_vers_cut = work_str.find("b");
        tmp_level_version = -1;
    }
    if(str_vers_cut == string::npos)
    {
        str_vers_cut = work_str.find("pr");
        tmp_level_version = 0;
    }
    if(str_vers_cut == string::npos)
    {
        str_vers_cut = work_str.size();
        tmp_level_version = 1;
    }
    work_str = work_str.substr(0, str_vers_cut);
    WorkFunctions::ParserFunctions::splitMass(&second_mas, 0, 0, work_str, ".");
    second_level_version = tmp_level_version;

    if(first_level_version > second_level_version) return 1;
    else if(first_level_version < second_level_version) return -1;

    map<int, int>* pointer_lower_mas = &second_mas;
    unsigned int max_size_mas = first_mas.size(), min_size_mas = second_mas.size();
    if(max_size_mas < second_mas.size())
    {
        min_size_mas = max_size_mas;
        max_size_mas = second_mas.size();
        pointer_lower_mas = &first_mas;
    }

    for(unsigned int i = 0; i < max_size_mas; i++)
    {
        if(i >= min_size_mas) (*pointer_lower_mas)[i] = 0;
        if(first_mas[i] > second_mas[i]) return 1;
        else if(first_mas[i] < second_mas[i]) return -1;
    }
    return 0;
}

bool GameData::loadData(PostParsingStruct* s_IniFile)
{
    bool doTestLevelpackCompatibility = true;
    ParserInfoFile prs;
    PathToTexturePack = "PacksData/TexturePacks/" + s_IniFile->getValue("resources", "texturepack") + "/";
    PathToMonsterPack = "PacksData/MonsterPacks/" + s_IniFile->getValue("resources", "monsterpack") + "/";
    PathToBonusPack = "PacksData/BonusPacks/" + s_IniFile->getValue("resources", "bonuspack") + "/";
    PathToScreenPack = "PacksData/ScreenPacks/" + s_IniFile->getValue("resources", "screenpack") + "/";
    PathToSoundPack = "PacksData/SoundPacks/" + s_IniFile->getValue("resources", "soundpack") + "/";
    PathToMusicPack = "PacksData/MusicPacks/" + s_IniFile->getValue("resources", "musicpack") + "/";
    PathToLevelPack = "PacksData/LevelPacks/" + s_IniFile->getValue("resources", "levelpack") + "/";
    PathToPlayerPack = "PacksData/PlayerPacks/" + s_IniFile->getValue("resources", "playerpack") + "/";
    PathToGuiPack = "PacksData/GuiPacks/" + s_IniFile->getValue("resources", "guipack") + "/";
    PathToBackgroundsPack = "PacksData/BackgroundsPacks/" + s_IniFile->getValue("resources", "backgroundspack") + "/";
    if( s_IniFile->getValue("resources", "standard") == "true" ) s_IniFile->getMapVariables()["resources"]["modpack"] = "StandardDave";
    if( s_IniFile->getValue("resources", "modpack") != "")
    {
        s_NameMod = s_IniFile->getValue("resources", "modpack");
        s_ModInfo = prs.getParsedFromFile("ModPacks/" + s_NameMod + "/About/mod.info");
        s_ModSettings = prs.getParsedFromFile("ModPacks/" + s_NameMod + "/About/settings.ini");
        if(!s_ModInfo || !s_ModSettings) return false;

        int res_compare_game_versions = compareGameVersions(STRING_CONSTANTS::SC_GAME_VERSION, s_ModInfo->getValue("info", "gameversion"));
        string str_error = "";
        if(res_compare_game_versions == 1) str_error = "previously";
        else if(res_compare_game_versions == -1) str_error = "newer";
        if(str_error != "") s_GameClass->s_Logger->registerEvent(EVENT_TYPE_WARNING, "This modpack was added for " + str_error + " version of DD2:ME! (game version: " + STRING_CONSTANTS::SC_GAME_VERSION + ", modpack version: " + s_ModInfo->getValue("info", "gameversion") + ")");

        if(s_ModInfo->getValue("other", "inifile") != "")
        {
            //delete s_GameClass->s_IniFile;
            //s_GameClass->s_IniFile = prs.getParsedFromFile("ModPacks/" + s_NameMod + "/"+ s_ModInfo->getValue("other", "inifile"));
            prs.addParsedFromFile(s_GameClass->s_IniFile, "ModPacks/" + s_NameMod + "/"+ s_ModInfo->getValue("other", "inifile"));
            if(!s_GameClass->s_IniFile) return false;
            s_IniFile = s_GameClass->s_IniFile;
            //...
            s_GameClass->s_DisplayStruct->s_GameResolutionX = atoi( (s_GameClass->s_IniFile->getValue("video", "resolutionX") ).c_str() );
            s_GameClass->s_DisplayStruct->s_GameResolutionY = atoi( (s_GameClass->s_IniFile->getValue("video", "resolutionY") ).c_str() );
            s_GameClass->s_DisplayStruct->s_WindowResolutionX = atoi( (s_GameClass->s_IniFile->getValue("video", "windowresolutionX") ).c_str() );
            s_GameClass->s_DisplayStruct->s_WindowResolutionY = atoi( (s_GameClass->s_IniFile->getValue("video", "windowresolutionY") ).c_str() );
            s_GameClass->s_GameInfo->readKeys(s_GameClass->s_IniFile);
            if(s_GameClass->s_DisplayStruct->s_GameResolutionY <= 0 || s_GameClass->s_DisplayStruct->s_GameResolutionX <= 0)
            {
                s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Display resolution.", true);
                return false;
            }
            //delete s_GameClass->s_Window;
            //s_GameClass->s_Window = new Window("DD2 Remake: ME v0.1pre-beta", s_GameClass->s_DisplayStruct->s_GameResolutionX, s_GameClass->s_DisplayStruct->s_GameResolutionY, atoi( (s_GameClass->s_IniFile->getValue("video","scale") ).c_str() ));
            //s_GameClass->s_Window = new Window(nostart);
            //s_GameClass->s_Window->geometry(s_GameClass->s_DisplayStruct->s_GameResolutionX, s_GameClass->s_DisplayStruct->s_GameResolutionY, atoi( (s_GameClass->s_IniFile->getValue("video","scale") ).c_str() ));
            //s_GameClass->s_Window->title(STRING_CONSTANTS::SC_TITLE_WINDOW);
            s_GameClass->createWindow();
            //...
        }
        if(s_IniFile->getValue("resources", "pooling") == "false")
        {
            doTestLevelpackCompatibility = false;
            s_ModSetted = true;
            PathToTexturePack = "ModPacks/" + s_NameMod + "/Textures/";
            PathToMonsterPack = "ModPacks/" + s_NameMod + "/Monsters/";
            PathToBonusPack = "ModPacks/" + s_NameMod + "/Bonuses/";
            PathToScreenPack = "ModPacks/" + s_NameMod + "/Screens/";
            PathToSoundPack = "ModPacks/" + s_NameMod + "/Sounds/";
            PathToMusicPack = "ModPacks/" + s_NameMod + "/Music/";
            PathToLevelPack = "ModPacks/" + s_NameMod + "/Levels/";
            PathToPlayerPack = "ModPacks/" + s_NameMod + "/Player/";
            PathToGuiPack = "ModPacks/" + s_NameMod + "/Gui/";
            PathToBackgroundsPack = "ModPacks/" + s_NameMod + "/Backgrounds/";
        }
        else
        {
            if(s_IniFile->getValue("resources", "texturepack") == "") PathToTexturePack = "ModPacks/" + s_NameMod + "/Textures/";
            if(s_IniFile->getValue("resources", "monsterpack") == "") PathToMonsterPack = "ModPacks/" + s_NameMod + "/Monsters/";
            if(s_IniFile->getValue("resources", "bonuspack") == "") PathToBonusPack = "ModPacks/" + s_NameMod + "/Bonuses/";
            if(s_IniFile->getValue("resources", "screenpack") == "") PathToScreenPack = "ModPacks/" + s_NameMod + "/Screens/";
            if(s_IniFile->getValue("resources", "soundpack") == "") PathToSoundPack = "ModPacks/" + s_NameMod + "/Sounds/";
            if(s_IniFile->getValue("resources", "musicpack") == "") PathToMusicPack = "ModPacks/" + s_NameMod + "/Music/";
            if(s_IniFile->getValue("resources", "levelpack") == "") { PathToLevelPack = "ModPacks/" + s_NameMod + "/Levels/"; doTestLevelpackCompatibility = false; }
            if(s_IniFile->getValue("resources", "playerpack") == "") PathToPlayerPack = "ModPacks/" + s_NameMod + "/Player/";
            if(s_IniFile->getValue("resources", "guipack") == "") PathToGuiPack = "ModPacks/" + s_NameMod + "/Gui/";
            if(s_IniFile->getValue("resources", "backgroundspack") == "") PathToBackgroundsPack = "ModPacks/" + s_NameMod + "/Backgrounds/";
        }
    }
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Loading levels information...");
    s_LevelsInfo = prs.getParsedFromFile(PathToLevelPack + "levels.dat");
    if(!s_LevelsInfo) return false;
    if(!s_LevelsInfo->isExists("info", "levelsformat") || atof(s_LevelsInfo->getValue("info", "levelsformat").c_str()) - NUMBER_CONSTANTS::NC_LEVEL_FORMAT_VERSION > NUMBER_CONSTANTS::NC_EPS) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_WARNING, "Incorrect levelpack format version!");
    if(doTestLevelpackCompatibility)
        if(!s_LevelsInfo->isExists("info", "modpack") || s_LevelsInfo->getValue("info", "modpack") != s_NameMod) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_WARNING, "It is impossible to check the compatibility of the levelpack with the modpack!");
    if(s_LevelsInfo->isExists("options", "startinglevel")) s_GameClass->s_GameInfo->s_CurrentLevel = atoi(s_LevelsInfo->getValue("options", "startinglevel").c_str());
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Levels information is loaded.");
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Loading bonuses...");
    if( !s_Bonuses->load(PathToBonusPack) ) return false;
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Bonuses is loaded.");
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Loading screens...");
    if( !s_Screens->load(PathToScreenPack) ) return false;
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Screens is loaded.");
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Loading textures...");
    if( !s_Textures->load(PathToTexturePack) ) return false;
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Textures is loaded.");
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Loading player...");
    if( !s_Player->load(PathToPlayerPack) ) return false;
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Player is loaded.");
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Loading monsters...");
    if( !s_Monsters->load(PathToMonsterPack) ) return false;
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Monsters is loaded.");
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Loading sounds...");
    if( !s_Sounds->load(PathToSoundPack) ) return false;
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Sounds is loaded.");
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Loading music...");
    if( !s_Music->load(PathToMusicPack) ) return false;
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Music is loaded.");
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Loading gui...");
    if( !s_GuiData->load(PathToGuiPack) ) return false;
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Gui is loaded.");
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Loading backgrounds...");
    if( !s_Backgrounds->load(PathToBackgroundsPack) ) return false;
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Backgrounds is loaded.");
    return true;
}

bool GameData::drawScreenState0()
{
    s_Screens->drawScreen("flybackground", 0, 0);
    s_Screens->drawScreen("flylogo", s_GameClass->s_GameInfo->s_DopScreenCoordX, s_GameClass->s_GameInfo->s_DopScreenCoordY);
    return true;
}

bool GameData::drawScreenState1()
{
    int newX = s_GameClass->s_GameInfo->s_DopScreenCoordX;
    int coordY = s_GameClass->s_GameInfo->s_DopScreenCoordY;
    int dispX = s_GameClass->s_DisplayStruct->s_GameResolutionX;
    int number = 0;
    while(newX < 0)
    {
        newX += dispX;
        number++;
    }
    if(newX%dispX == 0) s_Screens->drawScreen("mainscreen", newX, coordY, number);
    else
    {
        s_Screens->drawScreen("mainscreen", newX - dispX, coordY, number - 1);
        s_Screens->drawScreen("mainscreen", newX, coordY, number);
    }
    return true;
}

bool GameData::drawScreenState2()
{
    int scr = -1;
    int hlvl = s_GameClass->s_GameInfo->s_CurrentLevel;
    while(hlvl > 0 && scr < atoi( s_Screens->s_ChangeLevelInfo->getValue("info", "numberofscreens").c_str() ) )
    {
        scr++;
        hlvl -= atoi( s_Screens->s_ChangeLevelInfo->getValue("screen_" + WorkFunctions::ConvertFunctions::itos(scr + 1), "numberoflevels").c_str() );
    }
    hlvl += atoi( s_Screens->s_ChangeLevelInfo->getValue("screen_" + WorkFunctions::ConvertFunctions::itos(scr + 1), "numberoflevels").c_str() );
    s_Screens->drawScreen("changelevelscreen", 0, 0, scr);
    int drawingBonus = atoi( s_Screens->s_ChangeLevelInfo->getValue("info", "numberofbonusforplayer").c_str() );
    int coordX = atoi( s_Screens->s_ChangeLevelInfo->getValue("screen_" + WorkFunctions::ConvertFunctions::itos(scr + 1), "x" + WorkFunctions::ConvertFunctions::itos(hlvl)).c_str() );
    int coordY = atoi( s_Screens->s_ChangeLevelInfo->getValue("screen_" + WorkFunctions::ConvertFunctions::itos(scr + 1), "y" + WorkFunctions::ConvertFunctions::itos(hlvl)).c_str() );
    s_Bonuses->drawBonus(drawingBonus, coordX, coordY, false);
    s_GameClass->s_Gui->drawGuiState2(scr);
    return true;
}

bool GameData::drawScreenState3()
{
    if(s_GameClass->s_GameInfo->s_Stop == true && s_GameClass->s_GameInfo->s_DeathType == 0)
    {
        s_Screens->drawScreen("endscreen", s_GameClass->s_DisplayStruct->s_GameResolutionX/2 - s_Screens->s_StandardScreens["endscreen"]->getSize().x / 2, s_GameClass->s_DisplayStruct->s_GameResolutionY/2 - s_Screens->s_StandardScreens["endscreen"]->getSize().y / 2, 0);
        /*s_GameClass->s_Window->draw(Label("Thank you for playing in DD2:ME", s_GameClass->s_DisplayStruct->s_GameResolutionX/2 - 110, s_GameClass->s_DisplayStruct->s_GameResolutionY/2 - 30, 20), Pen(0, 255, 0));
        s_GameClass->s_Window->draw(Label("" + STRING_CONSTANTS::SC_GAME_VERSION + "!", s_GameClass->s_DisplayStruct->s_GameResolutionX/2 - 40, s_GameClass->s_DisplayStruct->s_GameResolutionY/2, 20), Pen(0, 255, 0));
        s_GameClass->s_Window->draw(Label("You win!", s_GameClass->s_DisplayStruct->s_GameResolutionX/2 - 20, s_GameClass->s_DisplayStruct->s_GameResolutionY/2 + 30, 20), Pen(0, 255, 0));
        s_GameClass->s_Window->draw(Label("", s_GameClass->s_DisplayStruct->s_GameResolutionX/2 - 20, s_GameClass->s_DisplayStruct->s_GameResolutionY/2 + 30), Pen(0, 0, 0));*/
    }
    s_GameClass->s_Gui->drawGuiState3();
    return true;
}
