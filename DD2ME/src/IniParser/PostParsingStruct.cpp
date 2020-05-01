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
        cout<<"Error: First value no setted. ( "<<A<<" )"<<endl;
        return "";
    }
    map<string, string>::iterator II = s_Variables[A].find(B);
    if ( II == s_Variables[A].end() )
    {
        cout<<"Error: Second value no setted. ( "<<A<<", "<<B<<" )"<<endl;
        return "";
    }
    return s_Variables[A][B];
}

void PostParsingStruct::setValue(string A, string B, string C)
{
    s_Variables[A][B] = C;
}
