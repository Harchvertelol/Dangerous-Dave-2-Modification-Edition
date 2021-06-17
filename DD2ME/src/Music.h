#ifndef MUSIC_H
#define MUSIC_H

#include <SFML/Audio.hpp>

#include "IniParser/PostParsingStruct.h"

class Game;

class Music
{
    public:
        Music(Game*);
        ~Music();
        Game* s_GameClass;
        IniParser::PostParsingStruct* s_MusicInfo;
        std::map<std::string, sf::Music> s_MusicList;
        bool load(std::string);
        bool play(std::string, bool restart = false, bool is_loop = true, int offset_ms = 0);
        bool stop(std::string);
        bool pause(std::string);
        void stopAllMusic();
};
#endif
