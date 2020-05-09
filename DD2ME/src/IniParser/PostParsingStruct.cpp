#include "PostParsingStruct.h"

#include <iostream>

using namespace std;

PostParsingStruct::PostParsingStruct()
{
    //...
}

PostParsingStruct::~PostParsingStruct()
{
    //...
}

string PostParsingStruct::getValue(string A, string B)
{
    map<string, map<string, string> >::iterator I = s_Variables.find(A);
    if ( I == s_Variables.end() )
    {
        cout << "Error: First value no setted. ( " << A << " )" << endl;
        return "";
    }
    map<string, string>::iterator II = s_Variables[A].find(B);
    if ( II == s_Variables[A].end() )
    {
        cout << "Error: Second value no setted. ( " << A << ", " << B << " )" <<endl;
        return "";
    }
    return s_Variables[A][B];
}

void PostParsingStruct::setValue(string A, string B, string C)
{
    s_Variables[A][B] = C;
}

void PostParsingStruct::addPostParsingStruct(PostParsingStruct* pps)
{
    map<string, map<string, string> >::iterator I;
    map<string, string>::iterator I_s;
    for(I = pps->s_Variables.begin(); I != pps->s_Variables.end(); I++)
    {
        for(I_s = I->second.begin(); I_s != I->second.end(); I_s++)
        {
            setValue(I->first, I_s->first, I_s->second);
        }
    }
}
