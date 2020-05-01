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
    EMPTY = 0, //�������
    IMPASSABLE = 1, //������������
    LADDER = 2, //��������
    DEATH = 3, //���� � �. �.
    EXITLEVEL = 4 //����� � ������ (�� �����)
};
#endif
