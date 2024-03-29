#include <fstream>
#include <iostream>
#include <regex>

#include "ParserInfoFile.h"
#include "PostParsingStruct.h"
#include "../WorkFunctions.h"
using namespace std;
using namespace WorkFunctions;
using namespace ParserFunctions;
using namespace IniParser;
using namespace FileFunctions;

IniParser::ParserInfoFile::ParserInfoFile():
    s_CryptKey(""),
    s_IsCrypted(false)
{
    //...
}

IniParser::ParserInfoFile::~ParserInfoFile()
{
    //...
}

bool IniParser::ParserInfoFile::writeParsedToFile(PostParsingStruct* pps, string filename, bool log)
{
    if(isCrypted())
    {
        ofstream FileInfo(filename.c_str(), ios::binary);
        if(!FileInfo) return false;
        string tmp_str = convertPostParsingStructToString(pps, "\r\n");
        string cr_str = cryptString(tmp_str, s_CryptKey);
        FileInfo << "cr" << cr_str << endl;
        FileInfo.close();
        return true;
    }
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

PostParsingStruct* IniParser::ParserInfoFile::getParsedFromFile(string s_FileName, PostParsingStruct* prs, bool log)
{
    //char buf[FILE_READ_SIZE_STR];
    string str;
    string NameMainVariable;
    string NameSecondaryVariable;
    string ValueSecondaryVariable;
    PostParsingStruct* s_PrsStr = 0;
    if(prs) s_PrsStr = prs;
    else
    {
        s_PrsStr = new PostParsingStruct;
        s_PrsStr->setFileName(s_FileName);
    }
    map<string, map<string, string> >* s_Variables = &s_PrsStr->getMapVariables();
    ifstream FileInfo(s_FileName.c_str());
    if(!FileInfo)
    {
        if(log == true) cerr<<"Error open file "<<s_FileName<<"!"<<endl;
        FileInfo.close();
        return 0;
    }
    //FileInfo.getline(buf, FILE_READ_SIZE_STR);
    //str = buf;
    getline(FileInfo, str);
    if(str.find("cr") == 0)
    {
        if(isCrypted())
        {
            FileInfo.close();
            string tmp_str = fileToString(s_FileName);
            s_PrsStr = getParsedFromString(tmp_str, "\n", s_PrsStr, true);
            return s_PrsStr;
        }
        else
        {
            cout << "Error: the file is encrypted, but the crypt mode is not enabled! File name: " << s_FileName << endl;
            if(prs == 0) delete s_PrsStr;
            return 0;
        }
    }
    NameMainVariable = getNameMainVariable(str);
    while(FileInfo)
    {
        if(NameMainVariable != "")
        {
            if((*s_Variables)[NameMainVariable].empty()){} // ������ �������, ���� �� �� ����������
            //FileInfo.getline(buf, FILE_READ_SIZE_STR);
            //str = buf;
            getline(FileInfo, str);
            while(FileInfo && getNameMainVariable(str) == "")
            {
                NameSecondaryVariable = getNameSecondaryVariable(str);
                ValueSecondaryVariable = getValueSecondaryVariable(str);
                if(NameSecondaryVariable != "")
                {
                    (*s_Variables)[NameMainVariable][NameSecondaryVariable] = ValueSecondaryVariable;
                }
                //FileInfo.getline(buf, FILE_READ_SIZE_STR);
                //str = buf;
                getline(FileInfo, str);
            }
        }
        else
        {
            //FileInfo.getline(buf, FILE_READ_SIZE_STR);
            //str = buf;
            getline(FileInfo, str);
        }
        NameMainVariable = getNameMainVariable(str);
    }
    FileInfo.close();
    return s_PrsStr;
}

void IniParser::ParserInfoFile::addParsedFromFile(PostParsingStruct* prs, string filename, bool log)
{
    getParsedFromFile(filename, prs, log);
}

void IniParser::ParserInfoFile::addParsedFromString(string str_s, string splitter, PostParsingStruct* prs)
{
    getParsedFromString(str_s, splitter, prs);
}

void IniParser::ParserInfoFile::addParsedFromPostParsingStruct(PostParsingStruct* pps_main, PostParsingStruct* pps_add)
{
    map<string, map<string, string> >::iterator iter;
    for(iter = pps_add->getMapVariables().begin(); iter != pps_add->getMapVariables().end(); iter++)
    {
        map<string, string>::iterator iter1;
        for(iter1 = iter->second.begin(); iter1 != iter->second.end(); iter1++) pps_main->setValue(iter->first, iter1->first, iter1->second);
    }
}

PostParsingStruct* IniParser::ParserInfoFile::getParsedFromString(string str_s, string splitter, PostParsingStruct* prs, bool fixbrbn)
{
    if(str_s.find("cr") == 0 && isCrypted())
    {
        str_s = str_s.substr(2);
        str_s = decryptString(str_s, s_CryptKey);
        if(fixbrbn) str_s = std::regex_replace(str_s, std::regex("\r\n"), "\n");
    }
    if(str_s.find("[") != 0)
    {
        splitter = str_s.substr(0, str_s.find("["));
        str_s = str_s.substr(str_s.find("["));
    }
    map<int, string> str_mas;
	int numberofstr = WorkFunctions::ParserFunctions::splitMassString(&str_mas, -1, 0, str_s, splitter);
    string str;
    string NameMainVariable;
    string NameSecondaryVariable;
    string ValueSecondaryVariable;
    PostParsingStruct* s_PrsStr = 0;
    if(prs) s_PrsStr = prs;
    else s_PrsStr = new PostParsingStruct;
    map<string, map<string, string> >* s_Variables = &s_PrsStr->getMapVariables();
    str = str_mas[0];
    NameMainVariable = getNameMainVariable(str);
    for(int i = 0; i < numberofstr;)
    {
        if(NameMainVariable != "")
        {
            if((*s_Variables)[NameMainVariable].empty()){} // ������ �������, ���� �� �� ����������
            i++;
            str = str_mas[i];
            bool was_in_cycle = false;
            while(i < numberofstr && getNameMainVariable(str) == "")
            {
                was_in_cycle = true;
                NameSecondaryVariable = getNameSecondaryVariable(str);
                ValueSecondaryVariable = getValueSecondaryVariable(str);
                if(NameSecondaryVariable != "")
                {
                    (*s_Variables)[NameMainVariable][NameSecondaryVariable] = ValueSecondaryVariable;
                }
                str = str_mas[i];
                i++;
            }
            if(was_in_cycle) i--;
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
    bool first_change_splitter = true;
    bool mode_change_splitter = false;
    string str = "";
    do
    {
        mode_change_splitter = false;
        str = "";
        map<string, map<string, string> >::iterator iter;
        for(iter = pps->getMapVariables().begin(); iter != pps->getMapVariables().end() && !mode_change_splitter; iter++)
        {
            if(iter->first.find(splitter) != string::npos) mode_change_splitter = true;
            map<string, string>::iterator iter1;
            str += "[" + iter->first + "]" + splitter;
            for(iter1 = iter->second.begin(); iter1 != iter->second.end() && !mode_change_splitter; iter1++)
            {
                if(iter1->first.find(splitter) != string::npos || iter1->second.find(splitter) != string::npos) mode_change_splitter = true;
                str += iter1->first + "=" + iter1->second + splitter;
            }
            str += splitter;
        }
        if(mode_change_splitter)
        {
            if(first_change_splitter)
            {
                splitter = "";
                first_change_splitter = false;
            }
            auto getNextSplitter = [](string old_splitter) -> string
            {
                string symbArr = "!@#$%^&*+-|;,'";
                int* index_arr = new int[old_splitter.size() + 1];
                for(int i = 0; i < old_splitter.size(); i++)
                {
                    int nmb = symbArr.find(old_splitter[i]);
                    if(nmb == string::npos)
                    {
                        delete[] index_arr;
                        return string(1, symbArr[0]);
                    }
                    else index_arr[i] = nmb;
                }
                index_arr[old_splitter.size()] = -1;
                for(int i = 0; true; i++)
                {
                    index_arr[i]++;
                    if(index_arr[i] >= symbArr.size()) index_arr[i] = 0;
                    else break;
                }
                string new_splitter = "";
                for(int i = 0; i < old_splitter.size() + 1; i++)
                    if(index_arr[i] != -1)
                        new_splitter += string(1, symbArr[index_arr[i]]);
                delete index_arr;
                return new_splitter;
            };
            splitter = getNextSplitter(splitter);
        }
    }
    while(mode_change_splitter);
    if(!first_change_splitter) str = splitter + str;
    return str;
}

bool IniParser::ParserInfoFile::setCryptedStatus(bool crypted)
{
    if(crypted && s_CryptKey == "")
    {
        cout << "Error: setting crypted status true with empty crypt key!" << endl;
        return false;
    }
    s_IsCrypted = crypted;
    return true;
}

bool IniParser::ParserInfoFile::isCrypted()
{
    return s_IsCrypted;
}

void IniParser::ParserInfoFile::setCryptKey(string crypt_key)
{
    s_CryptKey = crypt_key;
    if(s_CryptKey == "") setCryptedStatus(false);
}

string IniParser::ParserInfoFile::getCryptKey()
{
    return s_CryptKey;
}
