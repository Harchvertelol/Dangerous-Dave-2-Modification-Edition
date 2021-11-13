#ifndef DEFINES_H
#define DEFINES_H

#include <string>

enum WORK_CONSTANTS
{
    TIME_SECOND_FPS = 1000,
    MAXIMAL_UNSIGNED_NUMBER = 4000000000u
};

namespace STRING_CONSTANTS
{
    const std::string SPLITTER_STR_VARIABLE = ";";
    const std::string MISSING_ID_SERVER = "-1";
    const std::string SC_TITLE_WINDOW = "DD2 Remake: ME v0.9 pre-beta";
    const std::string SC_GAME_VERSION = "pre-beta v0.9";
    const std::string DEFAULT_FOLDER_SPLITTER = "/";

    const std::string NAME_FIELD_TILES = "Tiles1";
    const std::string NAME_FIELD_BONUSES = "FieldBonuses";
    const std::string NAME_FIELD_BONUSDOORS = "FieldBonusDoors";
    const std::string NAME_FIELD_DOORS = "FieldDoors";
    const std::string NAME_FIELD_EXITLEVELDOORS = "FieldExitLevelDoors";
}

namespace NUMBER_CONSTANTS
{
    const float NC_EPS = 0.0000001;
    const float NC_GAME_VERSION = 0.9;
    const float NC_API_VERSION = 0.1;
    const float NC_LEVEL_FORMAT_VERSION = 0.1;
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
