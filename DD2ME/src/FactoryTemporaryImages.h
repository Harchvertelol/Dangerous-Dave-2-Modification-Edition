#ifndef FACTORYTEMPORARYIMAGES_H
#define FACTORYTEMPORARYIMAGES_H
#include <map>
#include "TemporaryImageInfo.h"
#include "canvas.hpp"
using namespace std;
using namespace cnv;

class Game;

class FactoryTemporaryImages
{
    public:
        FactoryTemporaryImages(Game*);
        ~FactoryTemporaryImages();
        Game* s_GameClass;
        map<int, TemporaryImageInfo> s_Info;
        int s_MaxIndex;
        void addImage(Bitmap*,Bitmap*, int, int, int, int, int, string, bool onTop = false);
        void addImage(map<int, Bitmap*>*, map<int, Bitmap*>*, int, int, int, int, int, int, string, bool, bool onTop = false);
        void removeImage(int);
        void live();
        void drawAll(bool);
        void clear();
};
#endif
