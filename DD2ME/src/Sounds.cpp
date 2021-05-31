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
    s_SoundsInfo(0)
{
    //...
}

Sounds::~Sounds()
{
    if(s_SoundsInfo != 0) delete s_SoundsInfo;
}

bool Sounds::load(string PathToSoundPack)
{
    ParserInfoFile prs;
    s_SoundsInfo = prs.getParsedFromFile(PathToSoundPack + "sounds.info");
    if(!s_SoundsInfo) return false;
    return true;
}
