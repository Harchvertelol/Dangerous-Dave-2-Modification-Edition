#include "Level.h"

#include "IniParser/ParserInfoFile.h"
#include "WorkFunctions.h"
#include "Defines.h"

#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace WorkFunctions;
using namespace ConvertFunctions;
using namespace ParserFunctions;
using namespace MathFunctions;

using namespace IniParser;

Level::Level():
    s_Params(0)
{}

Level::~Level()
{
    if(s_Params != 0) delete s_Params;
}

bool Level::convertLevel(string file_name)
{
    cout << "Start converting: " << file_name << ".info" << endl;
    map<int, int>* field;
    ParserInfoFile prs;
    s_Params = prs.getParsedFromFile(file_name + ".info");
    if(!s_Params) return false;
    string str, NameField;
    int size_x = atoi( s_Params->getValue("info", "sizeX").c_str() );
    int size_y = atoi( s_Params->getValue("info", "sizeY").c_str() );
    char buf[FILE_READ_SIZE_STR];
    ifstream f_read( (file_name + ".lev").c_str() );
    if(!f_read)
    {
        cerr<<"Error: Level: "<<file_name<<". Load file level."<<endl;
        return false;
    }
    f_read.getline(buf, FILE_READ_SIZE_STR);
    str = buf;
    NameField = getNameMainVariable(str);
    while(f_read)
    {
        if(NameField != "")
        {
            field = &s_Fields[NameField];
            for(int i = 0; i < size_y*size_x; i++)
            {
                f_read.getline(buf, FILE_READ_SIZE_STR);
                str = buf;
                i = WorkFunctions::ParserFunctions::splitMass(field, size_y*size_x, i, str, " ") - 1;
            }
            f_read.getline(buf, FILE_READ_SIZE_STR);
            str = buf;
        }
        else
        {
            f_read.getline(buf, FILE_READ_SIZE_STR);
            str = buf;
        }
        NameField = getNameMainVariable(str);
    }
    f_read.close();

    WorkFunctions::FileFunctions::createFolders("result", ";");
    ofstream f( ("result/" + file_name + ".lev").c_str() );

    //info

    f << "[info]" << endl;
    f << "sizeX=" << size_x << endl;
    f << "sizeY=" << size_y << endl;
    f << "name=" << endl;
    f << "type=" << s_Params->getValue("info", "type") << endl;
    f << "numberoftilesfields=1" << endl;
    f << "background=" << endl;
    f << "levelformat=0.1" << endl;
    f << endl;

    //Options
    f << "[options]" << endl;
    if(s_Params->isExists("options", "distancelivemonstersX")) f << "distancelivemonstersX" << "=" << s_Params->getValue("options", "distancelivemonstersX") << endl;
    if(s_Params->isExists("options", "distancelivemonstersY")) f << "distancelivemonstersY" << "=" << s_Params->getValue("options", "distancelivemonstersY") << endl;
    if(s_Params->isExists("options", "distanceactivatemonstersX")) f << "distanceactivatemonstersX" << "=" << s_Params->getValue("options", "distanceactivatemonstersX") << endl;
    if(s_Params->isExists("options", "distanceactivatemonstersY")) f << "distanceactivatemonstersY" << "=" << s_Params->getValue("options", "distanceactivatemonstersY") << endl;
    f << endl;

    //Players
    f << "[Players]" << endl;
    int numberofdaves = atoi( s_Params->getValue("daves", "numberofdaves").c_str() );
    for(int i = 1; i <= numberofdaves; i++) f << "player" << i << "=" << atoi( s_Params->getValue("daves", "dave" + itos(i) + "X").c_str() ) * 16 << ";" << atoi( s_Params->getValue("daves", "dave" + itos(i) + "Y").c_str() ) * 16 << ";" << endl;
    f << endl;

    //Bonuses
    f << "[Bonuses]" << endl;
    for(int i = 0; i < size_y; i++)
        for(int j = 0; j < size_x; j++)
            if(s_Fields["FieldBonuses"][i * size_x + j] != 0) f << j << ";" << i << "=" << s_Fields["FieldBonuses"][i * size_x + j] << endl;
    f << endl;

    //BonusDoors
    f << "[BonusDoors]" << endl;
    for(int i = 0; i < size_y; i++)
        for(int j = 0; j < size_x; j++)
            if(s_Fields["FieldBonusDoors"][i * size_x + j] != 0) f << j << ";" << i << "=" << s_Fields["FieldBonusDoors"][i * size_x + j] << endl;
    f << endl;

    //Doors
    f << "[Doors]" << endl;
    for(int i = 0; i < size_y; i++)
        for(int j = 0; j < size_x; j++)
            if(s_Fields["FieldDoors"][i * size_x + j] != 0)
            {
                int state = 1;
                if(s_Fields["FieldDoors"][i * size_x + j] < 0) state = -1;
                int x = abs(s_Fields["FieldDoors"][i * size_x + j]) % size_x;
                int y = (abs(s_Fields["FieldDoors"][i * size_x + j]) - x) / size_x;
                f << j << ";" << i << "=" << state << ";" << x << ";" << y << ";" << endl;
            }
    f << endl;

    //ExitLevelDoors
    f << "[ExitLevelDoors]" << endl;
    for(int i = 0; i < size_y; i++)
        for(int j = 0; j < size_x; j++)
            if(s_Fields["FieldExitLevelDoors"][i * size_x + j] != 0) f << j << ";" << i << "=" << s_Fields["FieldExitLevelDoors"][i * size_x + j] << endl;
    f << endl;

    //Monsters
    f << "[Monsters]" << endl;
    int sch = 1;
    for(int i = 0; i < size_y; i++)
        for(int j = 0; j < size_x; j++)
            if(s_Fields["FieldMonsters"][i * size_x + j] != 0)
            {
                f << sch << "=" << s_Fields["FieldMonsters"][i * size_x + j] << ";" << j * 16 << ";" << i * 16 << ";{};" << endl;
                sch++;
            }
    f << endl;

    //SpawnPlayers
    f << "[SpawnPlayers]" << endl;
    sch = 1;
    for(int i = 0; i < size_y; i++)
        for(int j = 0; j < size_x; j++)
            if(s_Fields["FieldSpawnPlayers"][i * size_x + j] != 0)
            {
                f << sch << "=" << j * 16 << ";" << i * 16 << ";" << endl;
                sch++;
            }
    f << endl;

    //TilesParams
    f << "[TilesParams]" << endl;
    f << endl;

    //Field_Tiles1
    f << "[Field_Tiles1]" << endl;
    sch = 1;
    for(int i = 0; i < size_y; i++)
    {
        f << "l" << sch << "=";
        for(int j = 0; j < size_x; j++)
        {
            f << s_Fields["FieldTiles"][i * size_x + j];
            if(j < size_x - 1) f << " ";
        }
        f << endl;
        sch++;
    }
    f << endl;

    return true;
}
