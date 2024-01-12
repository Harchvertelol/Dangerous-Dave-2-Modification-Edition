#ifndef LEVEL_H
#define LEVEL_H
#include <map>
#include <vector>
#include <cstring>
#include "IniParser/PostParsingStruct.h"

class Game;

class Level
{
    public:
        Level(Game*);
        ~Level();
        Game* s_GameClass;
        IniParser::PostParsingStruct* s_Params;
        std::map<int, std::map<std::string, std::string> > s_TilesParams;
        std::vector<int> s_NumberBackgroundsOfFields;
        bool loadLevel(std::string);
        void draw(bool);
        void drawBackgrounds(int);
        void drawTilesField(int);
        std::string getNameTilesField(int);
        std::string getNamePhysicTilesField();
        std::string getNameObjectsTilesField();
        std::string getNameBackground(int, int);
        int getNumberTilesFields();
        int getNumberPhysicTilesField();
        int getNumberObjectsTilesField();
        int getTileType(int, int, int);
        int getTileID(int, int, int);
        std::string getTileParameter(int, int, std::string);
        int getFieldElement(std::string namefield, int index);
        std::map<std::string, std::map<int, int> >& getFieldsMap();
        std::map<int, int>& getFieldMap(std::string namefield);
        // Set functions
        bool setTileID(int, int, int, int, bool sendtonet = true, bool addtolisttileschanges = true);
        bool setTileIDByIndex(int, int, int, bool sendtonet = true, bool addtolisttileschanges = true);
        bool setTileParameter(int, int, std::string, std::string);
        bool openDoor(std::string type, int x_coord, int y_coord, bool sendtonet = true);
        bool setBonus(int x, int y, int type, bool sendtonet = true);
        std::map<std::string, std::map<int, int> > s_Fields;
        std::map<std::string, std::map<int, int> > s_ListTilesChanges;
};
#endif
