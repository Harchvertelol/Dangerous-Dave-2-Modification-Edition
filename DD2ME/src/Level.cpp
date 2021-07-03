#include "Level.h"

#include "IniParser/ParserInfoFile.h"
#include "WorkFunctions.h"
#include "Defines.h"
#include "Game.h"

#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace WorkFunctions;
using namespace ParserFunctions;
using namespace MathFunctions;

using namespace IniParser;

Level::Level(Game* gameclass):
    s_GameClass(gameclass),
    s_Params(0)
{}

Level::~Level()
{
    if(s_Params != 0) delete s_Params;
}

bool Level::loadLevel(string file_name)
{
    map<int, int>* field;
    ParserInfoFile prs;
    s_Params = prs.getParsedFromFile(file_name + ".info");
    if(!s_Params) return false;
    string str, NameField;
    int size_x = atoi( s_Params->getValue("info", "sizeX").c_str() );
    int size_y = atoi( s_Params->getValue("info", "sizeY").c_str() );
    char buf[FILE_READ_SIZE_STR];
    ifstream f( (file_name + ".lev").c_str() );
    if(!f)
    {
        cerr<<"Error: Level: "<<file_name<<". Load file level."<<endl;
        return false;
    }
    f.getline(buf, FILE_READ_SIZE_STR);
    str = buf;
    NameField = getNameMainVariable(str);
    while(f)
    {
        if(NameField != "")
        {
            field = &s_Fields[NameField];
            for(int i = 0; i < size_y*size_x; i++)
            {
                f.getline(buf, FILE_READ_SIZE_STR);
                str = buf;
                i = WorkFunctions::ParserFunctions::splitMass(field, size_y*size_x, i, str, " ") - 1;
            }
            f.getline(buf, FILE_READ_SIZE_STR);
            str = buf;
        }
        else
        {
            f.getline(buf, FILE_READ_SIZE_STR);
            str = buf;
        }
        NameField = getNameMainVariable(str);
    }
    for(int i = 0; i < size_y; i++)
        for(int j = 0; j < size_x; j++)
            if(s_Fields["FieldMonsters"][i*size_x + j] != 0)
                s_GameClass->s_GameInfo->s_FactoryMonsters->addMonsterImmediately(s_Fields["FieldMonsters"][i*size_x + j], 16*j + atoi( s_GameClass->s_Data->s_Monsters->s_MonstersInfo[s_Fields["FieldMonsters"][i*size_x + j] - 1]->getValue("other","outputshiftX").c_str() ), 16*i +  + atoi( s_GameClass->s_Data->s_Monsters->s_MonstersInfo[s_Fields["FieldMonsters"][i*size_x + j] - 1]->getValue("other","outputshiftY").c_str() ));
    CreatureDave* s_Dave = s_GameClass->s_GameInfo->s_MyDave;
    s_Dave->s_CoordX = 16*atoi( s_Params->getValue("daves", "dave1X").c_str() );
    s_Dave->s_CoordY = 16*atoi( s_Params->getValue("daves", "dave1Y").c_str() );
    return true;
}

void Level::draw()
{
    int ScrLX = s_GameClass->s_GameInfo->s_ScreenCoordX;
    int ScrLY = s_GameClass->s_GameInfo->s_ScreenCoordY;
    int ScrRX = ScrLX + s_GameClass->s_DisplayStruct->s_ResolutionX;
    int ScrRY = ScrLY + s_GameClass->s_DisplayStruct->s_ResolutionY;
    int DrawLevLX = roundNumber(ScrLX,16,-1)/16;
    int DrawLevLY = roundNumber(ScrLY,16,-1)/16;
    int DrawLevRX = roundNumber(ScrRX,16,1)/16;
    int DrawLevRY = roundNumber(ScrRY,16,1)/16;
    int SizeXLev = atoi( ( s_Params->getValue("info", "sizeX") ).c_str() );
    int SizeYLev = atoi( ( s_Params->getValue("info", "sizeY") ).c_str() );

    if(DrawLevLX < 0) DrawLevLX = 0;
    if(DrawLevRX < 0) DrawLevRX = 0;
    if(DrawLevLY < 0) DrawLevLY = 0;
    if(DrawLevRY < 0) DrawLevRY = 0;
    if(DrawLevLX > SizeXLev) DrawLevLX = SizeXLev;
    if(DrawLevRX > SizeXLev) DrawLevRX = SizeXLev;
    if(DrawLevLY > SizeYLev) DrawLevLY = SizeYLev;
    if(DrawLevRY > SizeYLev) DrawLevRY = SizeYLev;

    for(int i = DrawLevLY; i < DrawLevRY; i++)
        for(int j = DrawLevLX; j < DrawLevRX; j++)
        {
            s_GameClass->s_Data->s_Textures->drawTile(s_Fields["FieldTiles"][i*SizeXLev + j], (j - DrawLevLX)*16 - ScrLX%16, (i - DrawLevLY)*16 - ScrLY%16);
            s_GameClass->s_Data->s_Bonuses->drawBonus(s_Fields["FieldBonuses"][i*SizeXLev + j], (j - DrawLevLX)*16 - ScrLX%16, (i - DrawLevLY)*16 - ScrLY%16);
        }
}

int Level::getTileType(int x, int y)
{
    int tileid = getTileID(x, y);
    if(tileid == -1) return IMPASSABLE;
    int curtileid = s_GameClass->s_Data->s_Textures->getCurrentAnimationTileID(tileid);
    return s_GameClass->s_Data->s_Textures->s_MaskTiles[0][curtileid];
}

int Level::getTileID(int x, int y)
{
    int SizeXLev = atoi( ( s_Params->getValue("info", "sizeX") ).c_str() );
    int SizeYLev = atoi( ( s_Params->getValue("info", "sizeY") ).c_str() );
    if(y >= SizeYLev || x >= SizeXLev)
    {
        cout << "Error get tile ID! X or Y higher level." << endl;
        cout << "Level: " << s_GameClass->s_GameInfo->s_CurrentLevel << ", X = " << x << ", Y = " << y << ", SizeXLevel = " << SizeXLev << ", SizeYLevel = " << SizeYLev << "." << endl;
        return -1;
    }
    return s_Fields["FieldTiles"][y*SizeXLev + x];
}

bool Level::setTileID(int x, int y, int tileid)
{
    int SizeXLev = atoi( ( s_Params->getValue("info", "sizeX") ).c_str() );
    int SizeYLev = atoi( ( s_Params->getValue("info", "sizeY") ).c_str() );
    if(y >= SizeYLev || x >= SizeXLev)
    {
        cout << "Error set tile ID! X or Y higher level." << endl;
        cout << "Level: " << s_GameClass->s_GameInfo->s_CurrentLevel << ", X = " << x << ", Y = " << y << ", SizeXLevel = " << SizeXLev << ", SizeYLevel = " << SizeYLev << "." << endl;
        return false;
    }
    s_Fields["FieldTiles"][y*SizeXLev + x] = tileid;
    return true;
}
