#include "GameData.h"

#include "Game.h"
#include "IniParser/ParserInfoFile.h"
#include "WorkFunctions.h"
#include "Defines.h"

using namespace IniParser;

GameData::GameData(Game* gameclass):
    s_ModSetted(false),
    s_NameMod(""),
    PathToTexturePack(""),
    PathToMonsterPack(""),
    PathToBonusPack(""),
    PathToScreenPack(""),
    PathToSoundPack(""),
    PathToLevelPack(""),
    PathToDavePack(""),
    s_ModInfo(0),
    s_ModSettings(0),
    s_LevelsInfo(0),
    s_GameClass(gameclass)
{
    s_Level = new Level(gameclass);
    s_Textures = new Textures(gameclass);
    s_Bonuses = new Bonuses(gameclass);
    s_Screens = new Screens(gameclass);
    s_Dave = new Dave(gameclass);
    s_Monsters = new Monsters(gameclass);
    s_Sounds = new Sounds(gameclass);
}

GameData::~GameData()
{
    if(s_Level != 0) delete s_Level;
    if(s_LevelsInfo != 0) delete s_LevelsInfo;
    if(s_Textures != 0) delete s_Textures;
    if(s_ModInfo != 0) delete s_ModInfo;
    if(s_Bonuses != 0) delete s_Bonuses;
    if(s_Screens != 0) delete s_Screens;
    if(s_Dave != 0) delete s_Dave;
    if(s_Monsters != 0) delete s_Monsters;
    if(s_Sounds != 0) delete s_Sounds;
}

void GameData::deleteAllGDIObjects()
{
    s_Textures->deleteAllGDIObjects();
    s_Bonuses->deleteAllGDIObjects();
    s_Screens->deleteAllGDIObjects();
    s_Dave->deleteAllGDIObjects();
    s_Monsters->deleteAllGDIObjects();
}

bool GameData::loadData(PostParsingStruct* s_IniFile)
{
    ParserInfoFile prs;
    PathToTexturePack = "PacksData/TexturePacks/" + s_IniFile->getValue("resources", "texturepack") + "/";
    PathToMonsterPack = "PacksData/MonsterPacks/" + s_IniFile->getValue("resources", "monsterpack") + "/";
    PathToBonusPack = "PacksData/BonusPacks/" + s_IniFile->getValue("resources", "bonuspack") + "/";
    PathToScreenPack = "PacksData/ScreenPacks/" + s_IniFile->getValue("resources", "screenpack") + "/";
    PathToSoundPack = "PacksData/SoundPacks/" + s_IniFile->getValue("resources", "soundpack") + "/";
    PathToLevelPack = "PacksData/LevelPacks/" + s_IniFile->getValue("resources", "levelpack") + "/";
    PathToDavePack = "PacksData/DavePacks/" + s_IniFile->getValue("resources", "davepack") + "/";
    if( s_IniFile->getValue("resources", "standard") == "true" ) s_IniFile->getMapVariables()["resources"]["modpack"] = "StandardDave";
    if( s_IniFile->getValue("resources", "modpack") != "")
    {
        s_NameMod = s_IniFile->getValue("resources", "modpack");
        s_ModInfo = prs.getParsedFromFile("ModPacks/" + s_NameMod + "/mod.info");
        s_ModSettings = prs.getParsedFromFile("ModPacks/" + s_NameMod + "/settings.ini");
        if(!s_ModInfo || !s_ModSettings) return false;
        if(s_ModInfo->getValue("other", "inifile") != "")
        {
            delete s_GameClass->s_IniFile;
            s_GameClass->s_IniFile = prs.getParsedFromFile("ModPacks/" + s_NameMod + "/"+ s_ModInfo->getValue("other", "inifile"));
            if(!s_GameClass->s_IniFile) return false;
            s_IniFile = s_GameClass->s_IniFile;
            //...
            s_GameClass->s_DisplayStruct->s_ResolutionX = atoi( (s_GameClass->s_IniFile->getValue("video","resolutionX") ).c_str() );
            s_GameClass->s_DisplayStruct->s_ResolutionY = atoi( (s_GameClass->s_IniFile->getValue("video","resolutionY") ).c_str() );
            s_GameClass->s_GameInfo->s_KeyDown = atoi( s_GameClass->s_IniFile->getValue("keys","down").c_str() );
            s_GameClass->s_GameInfo->s_KeyUp = atoi( s_GameClass->s_IniFile->getValue("keys","up").c_str() );
            s_GameClass->s_GameInfo->s_KeyRight = atoi( s_GameClass->s_IniFile->getValue("keys","right").c_str() );
            s_GameClass->s_GameInfo->s_KeyLeft = atoi( s_GameClass->s_IniFile->getValue("keys","left").c_str() );
            s_GameClass->s_GameInfo->s_KeyShoot = atoi( s_GameClass->s_IniFile->getValue("keys","shoot").c_str() );
            s_GameClass->s_GameInfo->s_KeyJump = atoi( s_GameClass->s_IniFile->getValue("keys","jump").c_str() );
            if(s_GameClass->s_DisplayStruct->s_ResolutionY <= 0 || s_GameClass->s_DisplayStruct->s_ResolutionX <= 0)
            {
                cout<<"Error: Display resolution."<<endl;
                return false;
            }
            delete s_GameClass->s_Window;
            //s_GameClass->s_Window = new Window("DD2 Remake: ME v0.1pre-beta", s_GameClass->s_DisplayStruct->s_ResolutionX, s_GameClass->s_DisplayStruct->s_ResolutionY, atoi( (s_GameClass->s_IniFile->getValue("video","scale") ).c_str() ));
            s_GameClass->s_Window = new Window(nostart);
            s_GameClass->s_Window->geometry(s_GameClass->s_DisplayStruct->s_ResolutionX, s_GameClass->s_DisplayStruct->s_ResolutionY, atoi( (s_GameClass->s_IniFile->getValue("video","scale") ).c_str() ));
            s_GameClass->s_Window->title(STRING_CONSTANTS::SC_TITLE_WINDOW);
            //...
        }
        if(s_IniFile->getValue("resources","pooling") == "false")
        {
            s_ModSetted = true;
            PathToTexturePack = "ModPacks/" + s_NameMod + "/Textures/";
            PathToMonsterPack = "ModPacks/" + s_NameMod + "/Monsters/";
            PathToBonusPack = "ModPacks/" + s_NameMod + "/Bonuses/";
            PathToScreenPack = "ModPacks/" + s_NameMod + "/Screens/";
            PathToSoundPack = "ModPacks/" + s_NameMod + "/Sounds/";
            PathToLevelPack = "ModPacks/" + s_NameMod + "/Levels/";
            PathToDavePack = "ModPacks/" + s_NameMod + "/Dave/";
        }
        else
        {
            if(s_IniFile->getValue("resources", "texturepack") == "") PathToTexturePack = "ModPacks/" + s_NameMod + "/Textures/";
            if(s_IniFile->getValue("resources", "monsterpack") == "") PathToMonsterPack = "ModPacks/" + s_NameMod + "/Monsters/";
            if(s_IniFile->getValue("resources", "bonuspack") == "") PathToBonusPack = "ModPacks/" + s_NameMod + "/Bonuses/";
            if(s_IniFile->getValue("resources", "screenpack") == "") PathToScreenPack = "ModPacks/" + s_NameMod + "/Screens/";
            if(s_IniFile->getValue("resources", "soundpack") == "") PathToSoundPack = "ModPacks/" + s_NameMod + "/Sounds/";
            if(s_IniFile->getValue("resources", "levelpack") == "") PathToLevelPack = "ModPacks/" + s_NameMod + "/Levels/";
            if(s_IniFile->getValue("resources", "davepack") == "") PathToDavePack = "ModPacks/" + s_NameMod + "/Dave/";
        }
    }
    s_LevelsInfo = prs.getParsedFromFile(PathToLevelPack + "levels.dat");
    if(!s_LevelsInfo) return false;
    if( !s_Bonuses->load(PathToBonusPack) ) return false;
    if( !s_Screens->load(PathToScreenPack) ) return false;
    if( !s_Textures->load(PathToTexturePack) ) return false;
    if( !s_Dave->load(PathToDavePack) ) return false;
    if( !s_Monsters->load(PathToMonsterPack) ) return false;
    if( !s_Sounds->load(PathToSoundPack) ) return false;
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
    int dispX = s_GameClass->s_DisplayStruct->s_ResolutionX;
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
    int drawingBonus = atoi( s_Screens->s_ChangeLevelInfo->getValue("info", "numberofbonusfordave").c_str() );
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
        s_Screens->drawScreen("endscreen", s_GameClass->s_DisplayStruct->s_ResolutionX/2 - s_Screens->s_StandardScreens["endscreen"]->width() / 2, s_GameClass->s_DisplayStruct->s_ResolutionY/2 - s_Screens->s_StandardScreens["endscreen"]->height() / 2, 0);
        s_GameClass->s_Window->draw(Label("Thank you for playing in DD2:ME", s_GameClass->s_DisplayStruct->s_ResolutionX/2 - 110, s_GameClass->s_DisplayStruct->s_ResolutionY/2 - 30, 20), Pen(0, 255, 0));
        s_GameClass->s_Window->draw(Label("" + STRING_CONSTANTS::SC_GAME_VERSION + "!", s_GameClass->s_DisplayStruct->s_ResolutionX/2 - 40, s_GameClass->s_DisplayStruct->s_ResolutionY/2, 20), Pen(0, 255, 0));
        s_GameClass->s_Window->draw(Label("You win!", s_GameClass->s_DisplayStruct->s_ResolutionX/2 - 20, s_GameClass->s_DisplayStruct->s_ResolutionY/2 + 30, 20), Pen(0, 255, 0));
        s_GameClass->s_Window->draw(Label("", s_GameClass->s_DisplayStruct->s_ResolutionX/2 - 20, s_GameClass->s_DisplayStruct->s_ResolutionY/2 + 30), Pen(0, 0, 0));
    }
    s_GameClass->s_Gui->drawGuiState3();
    return true;
}
