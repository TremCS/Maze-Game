#ifndef MAP_H_
#define MAP_H_
#include <ngl/Camera.h>

class map
{
    private:
        ngl::Camera* m_cam;

    public:
        map(ngl::Camera* _cam);
        ~map();
        void loadMap();
        void flipMap();
        void updateMap();
        void draw();

};

#endif // MAP_H_
