#include <fstream>

#include "Sounds.h"

#include "IniParser/ParserInfoFile.h"
#include "WorkFunctions.h"

#include "Game.h"
#include "Defines.h"

using namespace IniParser;

using namespace std;

Sounds::Sounds(Game* gameclass):
    s_GameClass(gameclass),
    s_SoundsInfo(0),
    s_MaxIdTempSoundsList(0)
{
    //...
}

Sounds::~Sounds()
{
    if(s_SoundsInfo != 0) delete s_SoundsInfo;
    map<int, sf::Sound*>::iterator _iter__, _iter2__;
    for (_iter__ = s_TempSoundsList.begin(), _iter2__ = s_TempSoundsList.end(); _iter__ != _iter2__;)
    {
        if(_iter__->second != 0) delete _iter__->second;
        ++_iter__;
    }
}

bool Sounds::load(string PathToSoundPack)
{
    ParserInfoFile prs;
    s_SoundsInfo = prs.getParsedFromFile(PathToSoundPack + "sounds.info");
    if(!s_SoundsInfo) return false;
    map<string, map<string, string> > list_all_vars_sounds = s_SoundsInfo->getMapVariables();
    map<string, string> list_all_sounds = list_all_vars_sounds["sounds"];
    map<string, string>::iterator iter1, iter2;
    for (iter1 = list_all_sounds.begin(), iter2 = list_all_sounds.end(); iter1 != iter2;)
    {
        if(iter1->second != "" && s_SoundsBuffersList.find(iter1->second) == s_SoundsBuffersList.end())
        {
            if(!s_SoundsBuffersList[iter1->second].loadFromFile(PathToSoundPack + iter1->second))
            {
                s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error loading " + iter1->first + " with file " + iter1->second, true);
                return false;
            }
        }
        s_SoundsList[iter1->first].setBuffer(s_SoundsBuffersList[iter1->second]);
        s_SoundsList[iter1->first].setVolume(stof(s_GameClass->s_IniFile->getValue("audio", "soundsvolume")));
        iter1++;
    }
    return true;
}

void Sounds::clearInactiveTempSounds()
{
    map<int, sf::Sound*>::iterator it1, it2;
    for(it1 = s_TempSoundsList.begin(), it2 = s_TempSoundsList.end(); it1 != s_TempSoundsList.end();)
    {
        if(it1->second->getStatus() == sf::SoundSource::Stopped)
        {
            delete it1->second;
            s_TempSoundsList.erase(it1++);
        }
        else ++it1;
    }
}

int Sounds::play(string name, bool new_sound, bool restart, bool is_loop, int offset_ms)
{
    if(s_GameClass->s_IniFile->getValue("audio", "sounds") != "true") return 0;
    if(s_SoundsList.find(name) == s_SoundsList.end()) return 0;
    if(new_sound)
    {
        sf::Sound* newsound = new sf::Sound;
        newsound->setBuffer( *(s_SoundsList[name].getBuffer()) );
        newsound->setPlayingOffset(sf::milliseconds(offset_ms));
        newsound->setLoop(is_loop);
        s_TempSoundsList[s_MaxIdTempSoundsList] = newsound;
        s_TempSoundsList[s_MaxIdTempSoundsList]->play();
        s_MaxIdTempSoundsList++;
        return s_MaxIdTempSoundsList - 1;
    }
    if(!restart && s_SoundsList[name].getStatus() == sf::SoundSource::Status::Playing) return -2;
    if(offset_ms != 0) s_SoundsList[name].setPlayingOffset(sf::milliseconds(offset_ms));
    s_SoundsList[name].setLoop(is_loop);
    s_SoundsList[name].play();
    return -1;
}

bool Sounds::stop(string name)
{
    if(s_SoundsList.find(name) == s_SoundsList.end() || s_SoundsList[name].getStatus() == sf::SoundSource::Status::Stopped) return false;
    s_SoundsList[name].stop();
    return true;
}

bool Sounds::pause(string name)
{
    if(s_SoundsList.find(name) == s_SoundsList.end()) return false;
    s_SoundsList[name].pause();
    return true;
}
