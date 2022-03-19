#ifndef BACKGROUNDS_H
#define BACKGROUNDS_H

#include <SFML/Graphics.hpp>

#include "IniParser/PostParsingStruct.h"

class Game;

class Backgrounds
{
    public:
        Backgrounds(Game*);
        ~Backgrounds();
        Game* s_GameClass;
        IniParser::PostParsingStruct* s_BackgroundsInfo;
        std::map<std::string, sf::Texture> s_BackgroundsList;
        std::map<std::string, sf::Sprite> s_CacheBackgroundsList;
        bool s_CacheCreated;
        bool load(std::string);
        bool createCache();
        void draw(std::string, int, int, float, float);
        int getSizeBackgroundX(std::string);
        int getSizeBackgroundY(std::string);
        bool isExists(std::string);
        void deleteAllGDIObjects();
        void createMaskTransparent(int, int, int);
};
#endif
