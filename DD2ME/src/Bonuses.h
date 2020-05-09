#ifndef BONUSES_H
#define BONUSES_H
#include <map>
#include "canvas.hpp"
#include "IniParser/PostParsingStruct.h"
#include "Square.h"

class Game;

class Bonuses
{
    public:
        Bonuses(Game*);
        ~Bonuses();
        Game* s_GameClass;
        IniParser::PostParsingStruct* s_GlobBonusesInfo;
        std::map<int, IniParser::PostParsingStruct*> s_BonusesInfo;
        std::map<int, std::map<int, Bitmap* > > s_BonusesBitmaps;
        std::map<int, std::map<int, Bitmap* > > s_BonusesCache;
        std::map<int, std::map<int, Square> > s_Collisions;
        bool s_CacheCreated;
        bool load(std::string);
        bool createCache();
        void drawBonus(int bonus, int x, int y, bool anim = true);
        void collisionAnalyze(int, int);
        void deleteAllGDIObjects();
        void createMaskTransparent(int, int, int);
};
#endif
