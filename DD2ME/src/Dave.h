#ifndef DAVE_H
#define DAVE_H
#include <map>
#include "canvas.hpp"
#include "Square.h"
#include "IniParser/PostParsingStruct.h"

class Game;

class Dave
{
    public:
        Dave(Game*);
        ~Dave();
        Game* s_GameClass;
        IniParser::PostParsingStruct* s_DaveInfo;
        std::map<std::string, std::map<int, Bitmap*> > s_Bitmaps;
        std::map<std::string, std::map<int, Bitmap*> > s_CacheImages;
        std::map<std::string, std::map<int, Square> > s_Collisions;
        bool s_CacheCreated;
        bool load(std::string);
        bool createCache();
        void drawDave(std::string,int,int,int);
        void drawBandolier(int,int,int);
        void collisionAnalyze(std::string,int);
        void deleteAllGDIObjects();
        void createMaskTransparent(int,int,int);
};
#endif
