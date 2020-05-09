#include <fstream>
#include <iostream>
#include "ParserInfoFile.h"
#include "PostParsingStruct.h"
#include "../WorkFunctions.h"
#include "../Defines.h"
using namespace std;
using namespace WorkFunctions;
using namespace ParserFunctions;
using namespace IniParser;

IniParser::ParserInfoFile::ParserInfoFile()
{
    //...
}

IniParser::ParserInfoFile::~ParserInfoFile()
{
    //...
}

bool IniParser::ParserInfoFile::writeParsedToFile(PostParsingStruct* pps, string filename, bool log)
{
    ofstream FileInfo(filename.c_str());
    if(!FileInfo) return false;
    map<string, map<string, string> >::iterator iter;
    for(iter = pps->getMapVariables().begin(); iter != pps->getMapVariables().end(); iter++)
    {
        map<string, string>::iterator iter1;
        FileInfo << "[" + iter->first + "]" << endl;
        for(iter1 = iter->second.begin(); iter1 != iter->second.end(); iter1++)
        {
            FileInfo << iter1->first + "=" + iter1->second << endl;
        }
        FileInfo << endl;
    }
    return true;
}

PostParsingStruct* IniParser::ParserInfoFile::getParsedFromFile(string s_FileName, bool log)
{
    char buf[FILE_READ_SIZE_STR];
    string str;
    string NameMainVariable;
    string NameSecondaryVariable;
    string ValueSecondaryVariable;
    PostParsingStruct* s_PrsStr = new PostParsingStruct;
    map<string, map<string, string> >* s_Variables = &s_PrsStr->getMapVariables();
    ifstream FileInfo(s_FileName.c_str());
    if(!FileInfo)
    {
        if(log == true) cerr<<"Error open file "<<s_FileName<<"!"<<endl;
        FileInfo.close();
        return 0;
    }
    FileInfo.getline(buf, FILE_READ_SIZE_STR);
    str = buf;
    NameMainVariable = getNameMainVariable(str);
    while(FileInfo)
    {
        if(NameMainVariable != "")
        {
            FileInfo.getline(buf, FILE_READ_SIZE_STR);
            str = buf;
            while(FileInfo && getNameMainVariable(str) == "")
            {
                NameSecondaryVariable = getNameSecondaryVariable(str);
                ValueSecondaryVariable = getValueSecondaryVariable(str);
                if(NameSecondaryVariable != "")
                {
                    (*s_Variables)[NameMainVariable][NameSecondaryVariable] = ValueSecondaryVariable;
                }
                FileInfo.getline(buf, FILE_READ_SIZE_STR);
                str = buf;
            }
        }
        else
        {
            FileInfo.getline(buf, FILE_READ_SIZE_STR);
            str = buf;
        }
        NameMainVariable = getNameMainVariable(str);
    }
    FileInfo.close();
    return s_PrsStr;
}

PostParsingStruct* IniParser::ParserInfoFile::getParsedFromString(string str_s, string splitter)
{
    map<int, string> str_mas;
	int numberofstr = WorkFunctions::ParserFunctions::splitMassString(&str_mas, -1, 0, str_s, splitter);
    string str;
    string NameMainVariable;
    string NameSecondaryVariable;
    string ValueSecondaryVariable;
    PostParsingStruct* s_PrsStr = new PostParsingStruct;
    map<string, map<string, string> >* s_Variables = &s_PrsStr->getMapVariables();
    str = str_mas[0];
    NameMainVariable = getNameMainVariable(str);
    for(int i = 0; i < numberofstr;)
    {
        if(NameMainVariable != "")
        {
            i++;
            str = str_mas[i];
            while(i < numberofstr && getNameMainVariable(str) == "")
            {
                NameSecondaryVariable = getNameSecondaryVariable(str);
                ValueSecondaryVariable = getValueSecondaryVariable(str);
                if(NameSecondaryVariable != "")
                {
                    (*s_Variables)[NameMainVariable][NameSecondaryVariable] = ValueSecondaryVariable;
                }
                str = str_mas[i];
                i++;
            }
            i--;
        }
        else
        {
            i++;
            str = str_mas[i];
        }
        NameMainVariable = getNameMainVariable(str);
    }
    return s_PrsStr;
}

string IniParser::ParserInfoFile::convertPostParsingStructToString(PostParsingStruct* pps, string splitter)
{
    string str = "";
    map<string, map<string, string> >::iterator iter;
    for(iter = pps->getMapVariables().begin(); iter != pps->getMapVariables().end(); iter++)
    {
        map<string, string>::iterator iter1;
        str += "[" + iter->first + "]" + splitter;
        for(iter1 = iter->second.begin(); iter1 != iter->second.end(); iter1++)
        {
            str += iter1->first + "=" + iter1->second + splitter;
        }
    }
    return str;
}
