#include "mainDraw.h"
#ifndef map_H_
#define map_H_

class map: public mainDraw
{
    private:

    public:
        map();
        ~map();
        void loadMap();
        void updateMap();
        void flipMap();

};

#endif map_H_
