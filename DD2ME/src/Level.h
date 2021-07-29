#ifndef LEVEL_H
#define LEVEL_H
#include <map>
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
        std::map<std::string, std::map<int, int> > s_Fields;
        bool loadLevel(std::string);
        void draw();
        int getTileType(int, int);
        int getTileID(int, int);
        bool setTileID(int, int, int);
        std::string getTileParameter(int, int, std::string);
        bool setTileParameter(int, int, std::string, std::string);
};
#endif
