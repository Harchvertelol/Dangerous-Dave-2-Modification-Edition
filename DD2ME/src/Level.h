#ifndef LEVEL_H
#define LEVEL_H
#include <map>
#include <cstring>
#include "IniParser/PostParsingStruct.h"
using namespace std;

class Game;

class Level
{
    public:
        Level(Game*);
        ~Level();
        Game* s_GameClass;
        PostParsingStruct* s_Params;
        map<string, map<int,int> > s_Fields;
        bool loadLevel(string);
        void draw();
        int getTileType(int,int);
};
#endif
