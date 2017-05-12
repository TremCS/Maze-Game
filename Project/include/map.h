#ifndef MAP_H_
#define MAP_H_
#include <ngl/Camera.h>
#include <ngl/Image.h>

class map
{
    private:
        ngl::Camera* m_cam;
        std::unique_ptr<ngl::Image>m_image;
        ngl::Colour pixel;
    public:
        map(ngl::Camera* _cam, const std::string &_fname);
        ~map();
        void loadMap();
        void flipMap();
        void updateMap();
        void draw();
        int collision(float _x, float _z);
        inline ngl::Image *getImage(){return m_image.get();}

};

#endif // MAP_H_
