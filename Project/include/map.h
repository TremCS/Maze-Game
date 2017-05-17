#ifndef MAP_H_
#define MAP_H_
#include <ngl/Camera.h>
#include <ngl/Image.h>
#include <ngl/ShaderLib.h>
#include <ngl/Transformation.h>

class map
{
    private:
        ngl::Camera* m_cam;
        std::unique_ptr<ngl::Image>m_image;
        ngl::Colour pixel;
        float lastX;
        float lastZ;
        GLuint m_textureWall;
        GLuint m_textureFloor;
        GLuint m_textureCeiling;

        ngl::Transformation tx;

        std::unique_ptr<ngl::Image>m_image_wall;
        std::unique_ptr<ngl::Image>m_image_floor;
        std::unique_ptr<ngl::Image>m_image_ceiling;

        ngl::Vec3 startPos;
        ngl::Mat4 m_mouseGlobalTX;
        ngl::ShaderLib *shader;

    public:
        map(ngl::Camera* _cam, const std::string &_fname, float _x, float _z);
        ~map();
        void flipMap();
        void drawFloor();
        void drawWalls(float _x, float _z);
        void drawCeiling();
        void draw();
        void loadMatricesToShader(int texUnit, ngl::Vec2 scaleModTex);
        ngl::Vec3 spawnPos();

        bool collision(ngl::Vec3 _pos, ngl::Vec3 _aim, float _rad);
        bool collision_enemies(ngl::Vec3 _pos, ngl::Vec3 _aim, float _rad);
        bool collision_win(ngl::Vec3 _pos, ngl::Vec3 _aim, float _rad);
        inline ngl::Image *getImage(){return m_image.get();}

};

#endif // MAP_H_
