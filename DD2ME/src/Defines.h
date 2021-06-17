#ifndef DEFINES_H
#define DEFINES_H

#include <string>

enum WORK_CONSTANTS
{
    FILE_READ_SIZE_STR = 5000,
    TIME_SECOND_FPS = 1000,
    MAXIMAL_UNSIGNED_NUMBER = 4000000000u
};

namespace STRING_CONSTANTS
{
    const std::string SPLITTER_STR_VARIABLE = ";";
    const std::string MISSING_ID_SERVER = "-1";
    const std::string SC_TITLE_WINDOW = "DD2 Remake: ME v0.8 pre-beta";
    const std::string SC_GAME_VERSION = "pre-beta v0.8";
    const std::string DEFAULT_FOLDER_SPLITTER = "/";
}

namespace NUMBER_CONSTANTS
{
    const float NC_GAME_VERSION = 0.8;
    const float NC_API_VERSION = 0.1;
}

enum TILEMASK
{
    EMPTY = 0, //пустота
    IMPASSABLE = 1, //непроходимый
    LADDER = 2, //лестница
    DEATH = 3, //ножи и т. д.
    EXITLEVEL = 4 //выход с уровня (не дверь)
};

enum GAME_CONSTANTS
{
    GC_START_LIVES_NUMBER = 4
};
#endif
