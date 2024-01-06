#include <fstream>

#include "IniParser/ParserInfoFile.h"
#include "WorkFunctions.h"
#include "CustomFileSystem/VirtualFolder.h"

#include <NoIrrlicht/NoIrrlicht.h>
#include <irrNet.h>

using namespace std;

using namespace CustomFileSystem;

using namespace IniParser;
using namespace WorkFunctions;
using namespace ConvertFunctions;
using namespace FileFunctions;
using namespace ParserFunctions;

int main()
{
    VirtualFolder vf;
    //if(!vf.addFolderAsRootFromFS("StandardDave")) cout << "Error adding folder!" << endl;
    vf.setCryptKey("1q2w3e4r5t6y7u8i");
    vf.setCryptedStatus(true);
    vf.setCompressedStatus(true);
    //vf.write("PackFolder.ar");
    vf.load("PackFolder.ar");
    vf.writeAllToFolderFS("UNPACK");
    return 0;
}
