#include "File.h"

#include "../Defines.h"

using namespace std;

CustomFileSystem::File::File():
    s_FileName(""),
    s_Length(0),
    s_Buffer(0)
{
    //...
}

CustomFileSystem::File::File(char* data, const size_t length, const std::string& filename, bool copy_buffer):
    s_FileName(filename),
    s_Length(length)
{
    if(copy_buffer)
    {
        s_Buffer = new char[length];
        for(size_t i = 0; i < length; i++) s_Buffer[i] = data[i];
    }
    else s_Buffer = data;
}

CustomFileSystem::File::~File()
{
    if(s_Buffer != 0) delete[] s_Buffer;
}

string CustomFileSystem::File::getFileName()
{
    return s_FileName;
}

void CustomFileSystem::File::setFileName(string filename)
{
    s_FileName = filename;
}

size_t CustomFileSystem::File::getFileSize()
{
    return s_Length;
}

char* CustomFileSystem::File::getFileBuffer()
{
    return s_Buffer;
}

std::string CustomFileSystem::File::getFileFolder()
{
    size_t found = s_FileName.find_last_of(STRING_CONSTANTS::DEFAULT_FOLDER_SPLITTER);
    if(found != string::npos) return s_FileName.substr(0, found);
    return "";
}
