#ifndef SOUNDS_H
#define SOUNDS_H
#include "IniParser/PostParsingStruct.h"

class Game;

class Sounds
{
    public:
        Sounds(Game*);
        ~Sounds();
        Game* s_GameClass;
        IniParser::PostParsingStruct* s_SoundsInfo;
        //std::map<int, sf::SoundBuffer> s_Sounds;
        bool load(std::string);
};
#endif
