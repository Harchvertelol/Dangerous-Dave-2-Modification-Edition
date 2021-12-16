#ifndef FACTORYTEMPORARYIMAGES_H
#define FACTORYTEMPORARYIMAGES_H
#include <map>
#include "TemporaryImageInfo.h"
#include <SFML/Graphics.hpp>

class Game;

class FactoryTemporaryImages
{
    public:
        FactoryTemporaryImages(Game*);
        ~FactoryTemporaryImages();
        Game* s_GameClass;
        std::map<int, TemporaryImageInfo> s_Info;
        int s_MaxIndex;
        void addImage(sf::Texture*, sf::Sprite*, int, int, int, int, int, std::string, bool onTop = false);
        void addImage(std::map<int, sf::Texture*>*, std::map<int, sf::Sprite*>*, int, int, int, int, int, int, std::string, bool, bool onTop = false);
        void removeImage(int);
        void live();
        void drawAll(bool);
        void clear();
};
#endif
