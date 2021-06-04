#ifndef SOUNDS_H
#define SOUNDS_H

#include <SFML/Audio.hpp>

#include "IniParser/PostParsingStruct.h"

class Game;

class Sounds
{
    public:
        Sounds(Game*);
        ~Sounds();
        Game* s_GameClass;
        IniParser::PostParsingStruct* s_SoundsInfo;
        std::map<std::string, sf::SoundBuffer> s_SoundsBuffersList;
        std::map<std::string, sf::Sound> s_SoundsList;
        std::map<int, sf::Sound*> s_TempSoundsList;
        unsigned int s_MaxIdTempSoundsList;
        void clearInactiveTempSounds();
        bool load(std::string);
        int play(std::string, bool new_sound = false, bool restart = false, bool is_loop = false, int offset_ms = 0);
        bool stop(std::string);
        bool pause(std::string);
};
#endif
