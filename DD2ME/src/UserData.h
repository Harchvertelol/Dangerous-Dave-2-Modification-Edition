#ifndef USERDATA_H
#define USERDATA_H

#include "IniParser/PostParsingStruct.h"

class UserData
{
    public:
        UserData();
        ~UserData();
        PostParsingStruct* s_UserInfo;
        string s_IdServerConnected;
};

#endif // USERDATA_H
