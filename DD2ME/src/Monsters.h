#ifndef MONSTER_H
#define MONSTER_H
#include <map>
#include "Square.h"
#include "IniParser/PostParsingStruct.h"
#include <SFML/Graphics.hpp>

class Game;

class Monsters
{
    public:
        Monsters(Game*);
        ~Monsters();
        Game* s_GameClass;
        IniParser::PostParsingStruct* s_GlobMonstersInfo;
        std::map<int, IniParser::PostParsingStruct*> s_MonstersInfo;
        std::map<int, std::map<std::string, std::map<int, sf::Texture*> > > s_Bitmaps;
        std::map<int, std::map<std::string, std::map<int, sf::Sprite*> > > s_CacheImages;
        std::map<int, std::map<std::string, std::map<int, Square> > > s_Collisions;
        bool s_CacheCreated;
        bool load(std::string);
        bool loadMonster(std::string, int);
        void collisionAnalyze(int, std::string, int);
        bool createCache();
        void drawMonster(int, std::string, int, int, int, bool);
        void deleteAllGDIObjects();
        void createMaskTransparent(int, int, int);
};
#endif
