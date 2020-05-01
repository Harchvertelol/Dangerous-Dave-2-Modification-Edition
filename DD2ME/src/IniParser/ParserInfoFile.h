#ifndef PARSERINFOFILE_H
#define PARSERINFOFILE_H
#include <iostream>
#include "PostParsingStruct.h"
using namespace std;
class ParserInfoFile
{
    public:
        ParserInfoFile();
        ~ParserInfoFile();
        PostParsingStruct* getParsedFromFile(string, bool log = true);
        PostParsingStruct* getParsedFromString(string, string);
        string convertPostParsingStructToString(PostParsingStruct*, string);
};
#endif
