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
        float lastX;
        float lastZ;
    public:
        map(ngl::Camera* _cam, const std::string &_fname, float _x, float _z);
        ~map();
        void loadMap();
        void flipMap();
        void updateMap();
        void draw();
//        int collision_x(float _x, float _z);
//        int collision_z(float _x, float _z);
        bool collision(ngl::Vec3 _pos, ngl::Vec3 _aim, float _rad);
        inline ngl::Image *getImage(){return m_image.get();}

};

#endif // MAP_H_
