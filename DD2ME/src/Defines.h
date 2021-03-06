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
    const std::string SC_TITLE_WINDOW = "DD2 Remake: ME v0.6 pre-beta";
    const std::string SC_GAME_VERSION = "pre-beta v0.6";
    const std::string DEFAULT_FOLDER_SPLITTER = "/";
}
enum TILEMASK
{
    EMPTY = 0, //�������
    IMPASSABLE = 1, //������������
    LADDER = 2, //��������
    DEATH = 3, //���� � �. �.
    EXITLEVEL = 4 //����� � ������ (�� �����)
};
#endif
