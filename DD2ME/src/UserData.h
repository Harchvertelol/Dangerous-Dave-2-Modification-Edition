#ifndef USERDATA_H
#define USERDATA_H

#include "IniParser/PostParsingStruct.h"

class UserData
{
    public:
        UserData();
        ~UserData();
        IniParser::PostParsingStruct* s_UserInfo;
        std::string s_IdServerConnected;
};

#endif // USERDATA_H
