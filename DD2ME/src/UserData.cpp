#include "UserData.h"
#include "Defines.h"

UserData::UserData():
    s_UserInfo(0),
    s_IdServerConnected(STRING_CONSTANTS::MISSING_ID_SERVER)
{
    //...
}

UserData::~UserData()
{
    if(s_UserInfo != 0) delete s_UserInfo;
}
