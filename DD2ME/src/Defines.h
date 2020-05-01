#ifndef DEFINES_H
#define DEFINES_H
enum WORK_CONSTANTS
{
    FILE_READ_SIZE_STR = 5000,
    TIME_SECOND_FPS = 1000,
    MAXIMAL_UNSIGNED_NUMBER = 4000000000u
};
namespace STRING_CONSTANTS
{
    const string SPLITTER_STR_VARIABLE = ";";
    const string MISSING_ID_SERVER = "-1";
    const string SC_TITLE_WINDOW = "DD2 Remake: ME v0.4 pre-beta";
}
enum TILEMASK
{
    EMPTY = 0, //пустота
    IMPASSABLE = 1, //непроходимый
    LADDER = 2, //лестница
    DEATH = 3, //ножи и т. д.
    EXITLEVEL = 4 //выход с уровня (не дверь)
};
#endif
