#ifndef BONUSES_H
#define BONUSES_H
#include <map>
#include "canvas.hpp"
#include "IniParser/PostParsingStruct.h"
#include "Square.h"

using namespace std;
using namespace cnv;

class Game;

class Bonuses
{
    public:
        Bonuses(Game*);
        ~Bonuses();
        Game* s_GameClass;
        PostParsingStruct* s_GlobBonusesInfo;
        map<int, PostParsingStruct*> s_BonusesInfo;
        map<int, map<int, Bitmap* > > s_BonusesBitmaps;
        map<int, map<int, Bitmap* > > s_BonusesCache;
        map<int, map<int, Square> > s_Collisions;
        bool s_CacheCreated;
        bool load(string);
        bool createCache();
        void drawBonus(int bonus,int x,int y,bool anim = true);
        void collisionAnalyze(int,int);
        void deleteAllGDIObjects();
        void createMaskTransparent(int,int,int);
};
#endif
