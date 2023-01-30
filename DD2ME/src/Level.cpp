#include "Level.h"

#include "IniParser/ParserInfoFile.h"
#include "WorkFunctions.h"
#include "Defines.h"
#include "Game.h"

#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace std;

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
    /*map<int, int>* field;
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
    CreaturePlayer* s_Player = s_GameClass->s_GameInfo->s_MyPlayer;
    s_Player->s_CoordX = 16*atoi( s_Params->getValue("daves", "dave1X").c_str() );
    s_Player->s_CoordY = 16*atoi( s_Params->getValue("daves", "dave1Y").c_str() );
    return true;*/

    //new format read
    map<int, int>* field;
    s_Fields.clear();
    if(!s_Params) delete s_Params;
    ParserInfoFile prs;
    prs.setCryptKey(STRING_CONSTANTS::SC_CRYPT_KEY_LEVELS);
    prs.setCryptedStatus(true);
    s_Params = prs.getParsedFromFile(file_name + ".lev");
    if(!s_Params)
    {
        cerr<<"Error: Level: "<<file_name<<". Load file level."<<endl;
        return false;
    }
    if(!s_Params->isExists("info", "levelformat") || atof(s_Params->getValue("info", "levelformat").c_str()) - NUMBER_CONSTANTS::NC_LEVEL_FORMAT_VERSION > NUMBER_CONSTANTS::NC_EPS) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_WARNING, "Incorrect level format version!");
    if(!s_Params->isExists("options", "numberphysictilesfield")) s_Params->setValue("options", "numberphysictilesfield", "1");
    if(!s_Params->isExists("options", "numberobjectstilesfield")) s_Params->setValue("options", "numberobjectstilesfield", s_Params->getValue("options", "numberphysictilesfield"));
    string str, NameField, nameblock, name, value;
    int size_x = atoi( s_Params->getValue("info", "sizeX").c_str() );
    int size_y = atoi( s_Params->getValue("info", "sizeY").c_str() );

    map<string, map<string, string> > tmpMap = s_Params->getMapVariables();

    map<string, map<string, string> >::iterator tmpiter1, tmpiter2;
    for (tmpiter1 = tmpMap.begin(), tmpiter2 = tmpMap.end(); tmpiter1 != tmpiter2;)
    {
        nameblock = tmpiter1->first;
        if(nameblock.find("Field_") == 0)
        {
            NameField = nameblock.substr(6);
            field = &s_Fields[NameField];
            for(int i = 0, sch = 1; i < size_y*size_x; i++, sch++)
            {
                str = tmpiter1->second["l" + ConvertFunctions::itos(sch)];
                i = WorkFunctions::ParserFunctions::splitMass(field, size_y*size_x, i, str, " ") - 1;
            }
            tmpMap.erase(tmpiter1++);
        }
        else ++tmpiter1;
    }

    s_NumberBackgroundsOfFields.clear();
    s_NumberBackgroundsOfFields.push_back(-1); //Чтобы нумерация началась с 1-цы, а не с 0
    int numberTF = getNumberTilesFields();
    bool stop_find_bckgr = false;
    for(int i = 0; i <= numberTF; i++)
    {
        stop_find_bckgr = false;
        int j = 1;
        while(!stop_find_bckgr)
        {
            if(!s_Params->isExists(getNameBackground(i + 1, j))) stop_find_bckgr = true;
            else j++;
        }
        s_NumberBackgroundsOfFields.push_back(j - 1);
    }

    map<string, string> tmpMap_subbl;
    map<string, string>::iterator tmpiter1_, tmpiter2_;

    //s_Fields.erase(STRING_CONSTANTS::NAME_FIELD_BONUSES);
    tmpMap_subbl = s_Params->getMapVariables("Bonuses");
    for (tmpiter1_ = tmpMap_subbl.begin(), tmpiter2_ = tmpMap_subbl.end(); tmpiter1_ != tmpiter2_;)
    {
        name = tmpiter1_->first;
        value = tmpiter1_->second;
        map<int, int> tmp_coord;
        ParserFunctions::splitMass(&tmp_coord, 2, 0, name, ";");
        s_Fields[STRING_CONSTANTS::NAME_FIELD_BONUSES][tmp_coord[0] + tmp_coord[1] * size_x] = atoi( value.c_str() );
        ++tmpiter1_;
    }
    s_Params->remove("Bonuses");

    //s_Fields.erase(STRING_CONSTANTS::NAME_FIELD_BONUSDOORS);
    tmpMap_subbl = s_Params->getMapVariables("BonusDoors");
    for (tmpiter1_ = tmpMap_subbl.begin(), tmpiter2_ = tmpMap_subbl.end(); tmpiter1_ != tmpiter2_;)
    {
        name = tmpiter1_->first;
        value = tmpiter1_->second;
        map<int, int> tmp_coord;
        ParserFunctions::splitMass(&tmp_coord, 2, 0, name, ";");
        s_Fields[STRING_CONSTANTS::NAME_FIELD_BONUSDOORS][tmp_coord[0] + tmp_coord[1] * size_x] = atoi( value.c_str() );
        ++tmpiter1_;
    }
    s_Params->remove("BonusDoors");

    //s_Fields.erase(STRING_CONSTANTS::NAME_FIELD_DOORS);
    tmpMap_subbl = s_Params->getMapVariables("Doors");
    for (tmpiter1_ = tmpMap_subbl.begin(), tmpiter2_ = tmpMap_subbl.end(); tmpiter1_ != tmpiter2_;)
    {
        name = tmpiter1_->first;
        value = tmpiter1_->second;
        map<int, int> tmp_coord;
        map<int, int> tmp_value;
        ParserFunctions::splitMass(&tmp_coord, 2, 0, name, ";");
        ParserFunctions::splitMass(&tmp_value, 3, 0, value, ";");
        s_Fields[STRING_CONSTANTS::NAME_FIELD_DOORS][tmp_coord[0] + tmp_coord[1] * size_x] = tmp_value[0] * (tmp_value[1] + tmp_value[2] * size_x);
        ++tmpiter1_;
    }
    s_Params->remove("Doors");

    //s_Fields.erase(STRING_CONSTANTS::NAME_FIELD_EXITLEVELDOORS);
    tmpMap_subbl = s_Params->getMapVariables("ExitLevelDoors");
    for (tmpiter1_ = tmpMap_subbl.begin(), tmpiter2_ = tmpMap_subbl.end(); tmpiter1_ != tmpiter2_;)
    {
        name = tmpiter1_->first;
        value = tmpiter1_->second;
        map<int, int> tmp_coord;
        ParserFunctions::splitMass(&tmp_coord, 2, 0, name, ";");
        s_Fields[STRING_CONSTANTS::NAME_FIELD_EXITLEVELDOORS][tmp_coord[0] + tmp_coord[1] * size_x] = atoi( value.c_str() );
        ++tmpiter1_;
    }
    s_Params->remove("ExitLevelDoors");

    string Tname, Tvalue;

    tmpMap_subbl = s_Params->getMapVariables("Monsters");
    for (tmpiter1_ = tmpMap_subbl.begin(), tmpiter2_ = tmpMap_subbl.end(); tmpiter1_ != tmpiter2_;)
    {
        value = tmpiter1_->second;
        map<int, string> tmp_first_split;
        ParserFunctions::splitMassString(&tmp_first_split, 2, 0, value, "{");
        tmp_first_split[1] = tmp_first_split[1].substr(0, tmp_first_split[1].size() - 2);

        map<int, string> tmp_mnst_params;
        int tmp_size_mas_mnst_params = ParserFunctions::splitMassString(&tmp_mnst_params, -1, 0, tmp_first_split[1], ";");

        map<int, int> tmp_gen;
        ParserFunctions::splitMass(&tmp_gen, 3, 0, tmp_first_split[0], ";");

        int tmp_mnst_id = s_GameClass->s_GameInfo->s_FactoryMonsters->addMonsterImmediately(tmp_gen[0],
            tmp_gen[1] + atoi( s_GameClass->s_Data->s_Monsters->s_MonstersInfo[tmp_gen[0] - 1]->getValue("other","outputshiftX").c_str() ),
            tmp_gen[2] + atoi( s_GameClass->s_Data->s_Monsters->s_MonstersInfo[tmp_gen[0] - 1]->getValue("other","outputshiftY").c_str() ));

        for(int ii = 0; ii < tmp_size_mas_mnst_params; ii++)
        {
            Tname = ParserFunctions::getNameSecondaryVariable(tmp_mnst_params[ii]);
            Tvalue = ParserFunctions::getValueSecondaryVariable(tmp_mnst_params[ii]);
            if(Tname != "")
            {
                if(Tname == "GS_lives") s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[tmp_mnst_id]->s_CurrentLives = atoi( Tvalue.c_str() );
                else s_GameClass->s_GameInfo->s_FactoryMonsters->s_Monsters[tmp_mnst_id]->s_AIMonsterValues[Tname] = Tvalue;
            }
        }

        ++tmpiter1_;
    }
    s_Params->remove("Monsters");

    s_TilesParams.clear();
    tmpMap_subbl = s_Params->getMapVariables("TilesParams");
    for (tmpiter1_ = tmpMap_subbl.begin(), tmpiter2_ = tmpMap_subbl.end(); tmpiter1_ != tmpiter2_;)
    {
        name = tmpiter1_->first;
        value = tmpiter1_->second;
        map<int, string> tmp_params;
        int tmp_mas_size_params = ParserFunctions::splitMassString(&tmp_params, -1, 0, value, ";");
        map<int, int> tmp_coord;
        ParserFunctions::splitMass(&tmp_coord, 2, 0, name, ";");
        for(int ii = 0; ii < tmp_mas_size_params; ii++)
        {
            Tname = ParserFunctions::getNameSecondaryVariable(tmp_params[ii]);
            Tvalue = ParserFunctions::getValueSecondaryVariable(tmp_params[ii]);
            if(Tname != "") setTileParameter(tmp_coord[0], tmp_coord[1], Tname, Tvalue);
        }
        ++tmpiter1_;
    }
    s_Params->remove("TilesParams");

    CreaturePlayer* s_Player = s_GameClass->s_GameInfo->s_MyPlayer;
    string player_coords = s_Params->getValue("Players", "player1");
    map<int, int> tmp_spl;
    ParserFunctions::splitMass(&tmp_spl, 2, 0, player_coords, ";");
    s_Player->s_CoordX = tmp_spl[0];
    s_Player->s_CoordY = tmp_spl[1];
    return true;
}

string Level::getNameBackground(int number_tiles_field, int number_background)
{
    return STRING_CONSTANTS::PREFIX_NAME_BACKGROUND + WorkFunctions::ConvertFunctions::itos(number_tiles_field) + "_" + WorkFunctions::ConvertFunctions::itos(number_background);
}

string Level::getNamePhysicTilesField()
{
    return STRING_CONSTANTS::PREFIX_NAME_FIELD_TILES + s_Params->getValue("options", "numberphysictilesfield");
}

string Level::getNameObjectsTilesField()
{
    return STRING_CONSTANTS::PREFIX_NAME_FIELD_TILES + s_Params->getValue("options", "numberobjectstilesfield");
}

string Level::getTileParameter(int x_tile, int y_tile, string name)
{
    int SizeXLev = atoi( ( s_Params->getValue("info", "sizeX") ).c_str() );
    int SizeYLev = atoi( ( s_Params->getValue("info", "sizeY") ).c_str() );
    if(y_tile >= SizeYLev || x_tile >= SizeXLev)
    {
        s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error get tile parameter! X or Y higher level. Level: " + WorkFunctions::ConvertFunctions::itos(s_GameClass->s_GameInfo->s_CurrentLevel) + ", X = " + WorkFunctions::ConvertFunctions::itos(x_tile) + ", Y = " + WorkFunctions::ConvertFunctions::itos(y_tile) + ", SizeXLevel = " + WorkFunctions::ConvertFunctions::itos(SizeXLev) + ", SizeYLevel = " + WorkFunctions::ConvertFunctions::itos(SizeYLev) + ".");
        return "";
    }
    return s_TilesParams[x_tile + y_tile * SizeXLev][name];
}

bool Level::setTileParameter(int x_tile, int y_tile, string name, string value)
{
    int SizeXLev = atoi( ( s_Params->getValue("info", "sizeX") ).c_str() );
    int SizeYLev = atoi( ( s_Params->getValue("info", "sizeY") ).c_str() );
    if(y_tile >= SizeYLev || x_tile >= SizeXLev)
    {
        s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error set tile parameter! X or Y higher level. Level: " + WorkFunctions::ConvertFunctions::itos(s_GameClass->s_GameInfo->s_CurrentLevel) + ", X = " + WorkFunctions::ConvertFunctions::itos(x_tile) + ", Y = " + WorkFunctions::ConvertFunctions::itos(y_tile) + ", SizeXLevel = " + WorkFunctions::ConvertFunctions::itos(SizeXLev) + ", SizeYLevel = " + WorkFunctions::ConvertFunctions::itos(SizeYLev) + ".");
        return false;
    }
    s_TilesParams[x_tile + y_tile * SizeXLev][name] = value;
    return true;
}

void Level::drawBackgrounds(int number)
{
    int number_of_bckgrs = s_NumberBackgroundsOfFields[number], shiftX = 0, shiftY = 0, countloopX = -1, countloopY = -1;
    string name_val_bg, scrollspeedXstr, scrollspeedYstr, name_image;
    float scrollspeedX = 1, scrollspeedY = 1, scaleX = 0, scaleY = 0;
    for(int i = 0; i < number_of_bckgrs; i++)
    {
        name_val_bg = getNameBackground(number, i + 1);
        name_image = s_Params->getValue(name_val_bg, "image");
        shiftX = atoi(s_Params->getValue(name_val_bg, "startShiftX").c_str());
        shiftY = atoi(s_Params->getValue(name_val_bg, "startShiftY").c_str());
        countloopX = atoi(s_Params->getValue(name_val_bg, "countLoopX").c_str());
        countloopY = atoi(s_Params->getValue(name_val_bg, "countLoopY").c_str());
        scaleX = atof(s_Params->getValue(name_val_bg, "scaleX").c_str());
        scaleY = atof(s_Params->getValue(name_val_bg, "scaleY").c_str());

        int shiftXspec = 16 * 2;
        int shiftYspec = 16 * 2;
        int SizeXLevPix = atoi( ( s_Params->getValue("info", "sizeX") ).c_str() ) * 16;
        int SizeYLevPix = atoi( ( s_Params->getValue("info", "sizeY") ).c_str() ) * 16;
        int sizebgX = SizeXLevPix;
        if(name_image != "") sizebgX = s_GameClass->s_Data->s_Backgrounds->getSizeBackgroundX(name_image);
        int sizebgY = SizeYLevPix;
        if(name_image != "") sizebgY = s_GameClass->s_Data->s_Backgrounds->getSizeBackgroundY(name_image);
        sizebgX *= scaleX;
        sizebgY *= scaleY;
        int ScrLX = s_GameClass->s_GameInfo->s_ScreenCoordX;
        int ScrLY = s_GameClass->s_GameInfo->s_ScreenCoordY;
        int ScrXSize = s_GameClass->s_DisplayStruct->s_GameResolutionX;
        int ScrYSize = s_GameClass->s_DisplayStruct->s_GameResolutionY;

        scrollspeedXstr = s_Params->getValue(name_val_bg, "scrollSpeedX");
        if(scrollspeedXstr == "adaptive")
        {
            scrollspeedX = (float)(sizebgX - ScrXSize) / (float)(SizeXLevPix - ScrXSize - 2 * shiftXspec);
        }
        else scrollspeedX = atof(scrollspeedXstr.c_str());
        scrollspeedYstr = s_Params->getValue(name_val_bg, "scrollSpeedY");
        if(scrollspeedYstr == "adaptive")
        {
            scrollspeedY = (float)(sizebgY - ScrYSize) / (float)(SizeYLevPix - ScrYSize - 2 * shiftYspec);
        }
        else scrollspeedY = atof(scrollspeedYstr.c_str());

        int startCoordBGX = shiftX - scrollspeedX * (ScrLX - shiftXspec);
        int startCoordBGY = shiftY - scrollspeedY * (ScrLY - shiftYspec);

        int tmpRoundXD = roundNumber(0 - startCoordBGX, sizebgX, -1);
        int tmpRoundXU = roundNumber(0 - startCoordBGX + ScrXSize, sizebgX, 1);
        int number_bgX = (tmpRoundXU - tmpRoundXD) / sizebgX;

        int tmpRoundYD = roundNumber(0 - startCoordBGY, sizebgY, -1);
        int tmpRoundYU = roundNumber(0 - startCoordBGY + ScrYSize, sizebgY, 1);
        int number_bgY = (tmpRoundYU - tmpRoundYD) / sizebgY;

        int number_minX = tmpRoundXD / sizebgX;
        int number_minY = tmpRoundYD / sizebgY;

        //if(countloopX != -1 && number_minX + number_bgX > countloopX) number_bgX = countloopX - number_minX;

        if(name_image != "")
        {
            //s_GameClass->s_Data->s_Backgrounds->draw(name_image, startCoordBGX, startCoordBGY, scaleX, scaleY);
            for(int i = 0; i < number_bgX; i++)
                for(int j = 0; j < number_bgY; j++)
                    if( (countloopX == -1 || (number_minX + i >= 0 && number_minX + i < countloopX)) &&
                       (countloopY == -1 || (number_minY + j >= 0 && number_minY + j < countloopY)) ) s_GameClass->s_Data->s_Backgrounds->draw(name_image, startCoordBGX + tmpRoundXD + i * sizebgX, startCoordBGY + tmpRoundYD + j * sizebgY, scaleX, scaleY);
        }
    }
}

void Level::drawTilesField(int number)
{
    int ScrLX = s_GameClass->s_GameInfo->s_ScreenCoordX;
    int ScrLY = s_GameClass->s_GameInfo->s_ScreenCoordY;
    int ScrRX = ScrLX + s_GameClass->s_DisplayStruct->s_GameResolutionX;
    int ScrRY = ScrLY + s_GameClass->s_DisplayStruct->s_GameResolutionY;
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

    string namefield, nameobjectsfield = getNameObjectsTilesField();

    namefield = getNameTilesField(number);
    for(int i = DrawLevLY; i < DrawLevRY; i++)
        for(int j = DrawLevLX; j < DrawLevRX; j++)
        {
            s_GameClass->s_Data->s_Textures->drawTile(s_Fields[namefield][i*SizeXLev + j], (j - DrawLevLX)*16 - ScrLX%16, (i - DrawLevLY)*16 - ScrLY%16, j, i);
            if(namefield == nameobjectsfield) s_GameClass->s_Data->s_Bonuses->drawBonus(s_Fields[STRING_CONSTANTS::NAME_FIELD_BONUSES][i*SizeXLev + j], (j - DrawLevLX)*16 - ScrLX%16, (i - DrawLevLY)*16 - ScrLY%16);
        }
}

void Level::draw(bool before_objects_field)
{
    int numberoftilefields = getNumberTilesFields();

    int startI = 0, endI = numberoftilefields;
    if(before_objects_field) endI = getNumberObjectsTilesField();
    else startI = getNumberObjectsTilesField();

    for(int i = startI; i < endI; i++)
    {
        drawBackgrounds(i + 1);
        drawTilesField(i + 1);
    }
    if(!before_objects_field) drawBackgrounds(endI + 1);
}

int Level::getNumberObjectsTilesField()
{
    return atoi(s_Params->getValue("options", "numberobjectstilesfield").c_str());
}

int Level::getNumberPhysicTilesField()
{
    return atoi(s_Params->getValue("options", "numberphysictilesfield").c_str());
}

string Level::getNameTilesField(int number)
{
    return STRING_CONSTANTS::PREFIX_NAME_FIELD_TILES + WorkFunctions::ConvertFunctions::itos(number);
}

int Level::getNumberTilesFields()
{
    return atoi(s_Params->getValue("info", "numberoftilesfields").c_str());
}

int Level::getTileType(int x, int y, int numberfield)
{
    int tileid = getTileID(x, y, numberfield);
    if(tileid == -1) return EMPTY;
    int curtileid = s_GameClass->s_Data->s_Textures->getCurrentAnimationTileID(tileid, x, y);
    return s_GameClass->s_Data->s_Textures->s_MaskTiles[0][curtileid];
}

int Level::getTileID(int x, int y, int numberfield)
{
    int SizeXLev = atoi( ( s_Params->getValue("info", "sizeX") ).c_str() );
    int SizeYLev = atoi( ( s_Params->getValue("info", "sizeY") ).c_str() );
    if(y >= SizeYLev || x >= SizeXLev)
    {
        s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error get tile ID! X or Y higher level. Level: " + WorkFunctions::ConvertFunctions::itos(s_GameClass->s_GameInfo->s_CurrentLevel) + ", X = " + WorkFunctions::ConvertFunctions::itos(x) + ", Y = " + WorkFunctions::ConvertFunctions::itos(y) + ", SizeXLevel = " + WorkFunctions::ConvertFunctions::itos(SizeXLev) + ", SizeYLevel = " + WorkFunctions::ConvertFunctions::itos(SizeYLev) + ".");
        return -1;
    }
    return s_Fields[getNameTilesField(numberfield)][y*SizeXLev + x];
}

bool Level::setTileID(int x, int y, int numberfield, int tileid)
{
    int SizeXLev = atoi( ( s_Params->getValue("info", "sizeX") ).c_str() );
    int SizeYLev = atoi( ( s_Params->getValue("info", "sizeY") ).c_str() );
    if(y >= SizeYLev || x >= SizeXLev)
    {
        s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error set tile ID! X or Y higher level. Level: " + WorkFunctions::ConvertFunctions::itos(s_GameClass->s_GameInfo->s_CurrentLevel) + ", X = " + WorkFunctions::ConvertFunctions::itos(x) + ", Y = " + WorkFunctions::ConvertFunctions::itos(y) + ", SizeXLevel = " + WorkFunctions::ConvertFunctions::itos(SizeXLev) + ", SizeYLevel = " + WorkFunctions::ConvertFunctions::itos(SizeYLev) + ".");
        return false;
    }
    s_Fields[getNameTilesField(numberfield)][y*SizeXLev + x] = tileid;
    return true;
}
