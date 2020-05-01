#ifndef POSTPARSINGSTRUCT_H
#define POSTPARSINGSTRUCT_H
#include <map>
#include <string>
using namespace std;
class PostParsingStruct
{
    public:
        PostParsingStruct();
        ~PostParsingStruct();
        map<string, map<string, string> > s_Variables;
        string getValue(string A, string B);
        void setValue(string A, string B, string C);
};
#endif
