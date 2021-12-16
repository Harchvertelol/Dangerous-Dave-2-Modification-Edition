#ifndef TEMPORARYIMAGEINFO_H
#define TEMPORARYIMAGEINFO_H
#include <iostream>
#include <SFML/Graphics.hpp>

class Game;

class TemporaryImageInfo
{
    public:
        TemporaryImageInfo();
        ~TemporaryImageInfo();
        int s_CoordX;
        int s_CoordY;
        int s_TimeLive;
        int s_ChangeX;
        int s_ChangeY;
        std::map<int, sf::Texture*>* s_Bitmaps;
        std::map<int, sf::Sprite*>* s_CacheBitmaps;
        sf::Texture* s_Bitmap;
        sf::Sprite* s_Cache;
        int s_NumberOfFrames;
        std::string s_Type;
        bool s_Animated;
        bool s_CacheCreated;
        bool s_ManyFrames;
        Game* s_GameClass;
        void draw();
        void live();
        bool onTop;
};
#endif
