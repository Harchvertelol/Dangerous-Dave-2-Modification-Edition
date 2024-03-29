#include <fstream>

#include "Music.h"

#include "IniParser/ParserInfoFile.h"
#include "WorkFunctions.h"

#include "Game.h"
#include "Defines.h"

using namespace IniParser;

using namespace std;

Music::Music(Game* gameclass):
    s_GameClass(gameclass),
    s_MusicInfo(0)
{
    //...
}

Music::~Music()
{
    if(s_MusicInfo != 0) delete s_MusicInfo;
}

bool Music::load(string PathToMusicPack)
{
    ParserInfoFile prs;
    s_MusicInfo = prs.getParsedFromFile(PathToMusicPack + "music.info");
    if(!s_MusicInfo) return false;
    map<string, map<string, string> > list_all_vars_music = s_MusicInfo->getMapVariables();
    map<string, string> list_all_music = list_all_vars_music["music"];
    map<string, string>::iterator iter1, iter2;
    for (iter1 = list_all_music.begin(), iter2 = list_all_music.end(); iter1 != iter2;)
    {
        if(iter1->second != "" && s_MusicList.find(iter1->second) == s_MusicList.end())
        {
            if(!s_MusicList[iter1->first].openFromFile(PathToMusicPack + iter1->second))
            {
                s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error loading " + iter1->first + " with file " + iter1->second, true);
                return false;
            }
            s_MusicList[iter1->first].setVolume(stof(s_GameClass->s_IniFile->getValue("audio", "musicvolume")));
        }
        iter1++;
    }
    return true;
}

bool Music::play(string name, bool restart, bool is_loop, int offset_ms)
{
    if(s_GameClass->s_IniFile->getValue("audio", "music") != "true") return 0;
    if(s_MusicList.find(name) == s_MusicList.end()) return false;
    if(!restart && s_MusicList[name].getStatus() == sf::SoundSource::Status::Playing) return false;
    if(offset_ms != 0) s_MusicList[name].setPlayingOffset(sf::milliseconds(offset_ms));
    s_MusicList[name].setLoop(is_loop);
    s_MusicList[name].play();
    return true;
}

bool Music::stop(string name)
{
    if(s_MusicList.find(name) == s_MusicList.end()) return false;
    s_MusicList[name].stop();
    return true;
}

bool Music::pause(string name)
{
    if(s_MusicList.find(name) == s_MusicList.end()) return false;
    s_MusicList[name].pause();
    return true;
}

void Music::stopAllMusic()
{
    map<string, sf::Music>::iterator _iter__, _iter2__;
    for (_iter__ = s_MusicList.begin(), _iter2__ = s_MusicList.end(); _iter__ != _iter2__;)
    {
        _iter__->second.stop();
        ++_iter__;
    }
}
