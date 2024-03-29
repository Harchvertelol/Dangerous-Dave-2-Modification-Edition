#include "PostParsingStruct.h"

#include <iostream>

using namespace std;

IniParser::PostParsingStruct::PostParsingStruct():
    s_FileName("")
{
    //...
}

IniParser::PostParsingStruct::~PostParsingStruct()
{
    map<string, map<string, string> >::iterator iter, iter2;
    for(iter = s_Variables.begin(), iter2 = s_Variables.end(); iter != iter2;)
    {
        s_Variables[iter->first].clear();
        ++iter;
    }
    s_Variables.clear();
}

string IniParser::PostParsingStruct::getValue(string A, string B, bool error_on_not_exists)
{
    map<string, map<string, string> >::iterator I = s_Variables.find(A);
    if ( I == s_Variables.end() )
    {
        if(error_on_not_exists)
        {
            string show_error_str = "Error: ";
            if(s_FileName != "") show_error_str += "File: " + s_FileName + ", ";
            show_error_str += "First value not setted. ( " + A + " )";
            cout << show_error_str << endl;
        }
        return "";
    }
    map<string, string>::iterator II = s_Variables[A].find(B);
    if ( II == s_Variables[A].end() )
    {
        if(error_on_not_exists)
        {
            string show_error_str = "Error: ";
            if(s_FileName != "") show_error_str += "File: " + s_FileName + ", ";
            show_error_str += "Second value not setted. ( " + A + ", " + B + " )";
            cout << show_error_str << endl;
        }
        return "";
    }
    return s_Variables[A][B];
}

void IniParser::PostParsingStruct::setValue(string A, string B, string C)
{
    s_Variables[A][B] = C;
}

map<string, map<string, string> >& IniParser::PostParsingStruct::getMapVariables()
{
    return s_Variables;
}

map<string, string>& IniParser::PostParsingStruct::getMapVariables(string A)
{
    return s_Variables[A];
}

void IniParser::PostParsingStruct::addPostParsingStruct(PostParsingStruct* pps)
{
    map<string, map<string, string> > ppsvariables = pps->getMapVariables();
    map<string, map<string, string> >::iterator I;
    map<string, string>::iterator I_s;
    for(I = ppsvariables.begin(); I != ppsvariables.end(); I++)
    {
        for(I_s = I->second.begin(); I_s != I->second.end(); I_s++)
        {
            setValue(I->first, I_s->first, I_s->second);
        }
    }
}

bool IniParser::PostParsingStruct::isExists(string A)
{
    map<string, map<string, string> >::iterator I = s_Variables.find(A);
    if ( I == s_Variables.end() ) return false;
    return true;
}

bool IniParser::PostParsingStruct::isExists(string A, string B)
{
    map<string, map<string, string> >::iterator I = s_Variables.find(A);
    if ( I == s_Variables.end() ) return false;
    map<string, string>::iterator II = s_Variables[A].find(B);
    if ( II == s_Variables[A].end() ) return false;
    return true;
}

void IniParser::PostParsingStruct::setFileName(string fname)
{
    s_FileName = fname;
}

void IniParser::PostParsingStruct::remove(string A, string B)
{
    s_Variables[A].erase(B);
}

void IniParser::PostParsingStruct::remove(string A)
{
    s_Variables.erase(A);
}

void IniParser::PostParsingStruct::clear()
{
    s_Variables.clear();
}

bool IniParser::PostParsingStruct::isEmpty()
{
    return s_Variables.empty();
}
