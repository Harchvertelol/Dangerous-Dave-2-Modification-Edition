#ifndef LEVEL_H
#define LEVEL_H
#include <map>
#include <cstring>
#include "IniParser/PostParsingStruct.h"

class Level
{
    public:
        Level();
        ~Level();
        IniParser::PostParsingStruct* s_Params;
        std::map<std::string, std::map<int, int> > s_Fields;
        bool convertLevel(std::string);
};
#endif
