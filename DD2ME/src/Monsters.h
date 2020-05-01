#ifndef MONSTER_H
#define MONSTER_H
#include <map>
#include "canvas.hpp"
#include "Square.h"
#include "IniParser/PostParsingStruct.h"
using namespace cnv;
using namespace std;

class Game;

class Monsters
{
    public:
        Monsters(Game*);
        ~Monsters();
        Game* s_GameClass;
        PostParsingStruct* s_GlobMonstersInfo;
        map<int, PostParsingStruct*> s_MonstersInfo;
        map<int, map<string, map<int, Bitmap*> > > s_Bitmaps;
        map<int, map<string, map<int, Bitmap*> > > s_CacheImages;
        map<int, map<string, map<int, Square> > > s_Collisions;
        bool s_CacheCreated;
        bool load(string);
        bool loadMonster(string,int);
        void collisionAnalyze(int,string,int);
        bool createCache();
        void drawMonster(int,string,int,int,int);
        void deleteAllGDIObjects();
        void createMaskTransparent(int,int,int);
};
#endif
