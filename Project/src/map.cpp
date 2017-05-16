#include <iostream>
#include "map.h"
#include "mainDraw.h"
#include <ngl/ShaderLib.h>
#include <ngl/NGLInit.h>
#include <ngl/Material.h>
#include <ngl/Transformation.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Image.h>

map::map(ngl::Camera* _cam, const std::string &_fname, float _x, float _z)
{


    m_image.reset(new ngl::Image(_fname));

    loadMap();
    lastX = _x;
    lastZ = _z;
    ngl::Colour pixel(0.0f, 0.0f, 0.0f);
    ngl::Vec3 startPos(0.0f, 1.0f, 0.0f);
    m_cam = _cam;

}


void map::loadMap()
{

//    ngl::VAOPrimitives::instance()->draw("ground");
}

void map::updateMap()
{

}

void map::draw()
{
    ngl::VAOPrimitives::instance()->createTrianglePlane(
                "ground", 80, 80, 10, 10, ngl::Vec3::up());

    ngl::ShaderLib *shader= ngl::ShaderLib::instance();
    shader->use(ngl::nglColourShader);
    shader->setUniform("Colour",1.0f,0.0f, 0.0f,1.0f);




    (*shader)["Phong"]->use();

    ngl::Mat4 MV;
    ngl::Mat4 MVP;
    ngl::Mat3 normalMatrix;
    ngl::Mat4 M;
    M= ngl::Mat4(1.0f);
    MV=  M*m_cam->getViewMatrix();
    MVP= M*m_cam->getVPMatrix();
    normalMatrix=MV;
    normalMatrix.inverse();
    shader->setShaderParamFromMat4("MV",MV);
    shader->setShaderParamFromMat4("MVP",MVP);
    shader->setShaderParamFromMat3("normalMatrix",normalMatrix);
    shader->setShaderParamFromMat4("M",M);

    ngl::VAOPrimitives::instance()->draw("ground");

    //GLuint w=m_image->width();
   // GLuint h=m_image->height();
    ngl::Transformation tx;
    ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
    shader->use(ngl::nglColourShader);
    shader->setUniform("Colour",1.0f,0.0f, 0.0f,1.0f);



    auto toFloat=[=](float _r){return _r/255.0f;};

    //image mapping
    GLuint imageX=0;
    GLuint imageY=0;
    float halfX = (m_image->height()/2)+0.5;
    float halfZ = (m_image->width()/2)+0.5;

    for(float z=-halfZ;z<=halfZ; ++z)
    {
        for(float x=-halfX; x<=halfX; ++x)
        {
            pixel = m_image->getColour(imageX,imageY);

            if(!FCompare(pixel.m_r,255) && !FCompare(pixel.m_g,255) &&  !FCompare(pixel.m_b,255))
            {
            //std::cout<<m_image->getColour(x,z)<<'\n';
            tx.setPosition(-x,1,z);
            tx.setScale(1, 3, 1);
            shader->setUniform("MVP",tx.getMatrix()*
                               m_cam->getVPMatrix());
            shader->setUniform("Colour", toFloat(pixel.m_r), toFloat(pixel.m_g), toFloat(pixel.m_b), 1.0);


            prim->draw("cube");
            }//end if
            if(FCompare(pixel.m_g,255) && FCompare(pixel.m_b,0))
            {
                prim->draw("teapot");
            }

            ++imageX;

        }//end X
        imageX=0;
        ++imageY;
    }//endZ


//    ngl::VAOPrimitives::instance()->draw("teapot");

}


bool map::collision(ngl::Vec3 _pos, ngl::Vec3 _aim, float _rad)
{
    //Bottom left of image
    GLuint m_imageX = 0;
    GLuint m_imageY = 0;
    float m_halfX = ((float)m_image->height())/2;
    float m_halfZ = ((float)m_image->width())/2;

    //aim vector resize to map proportions
    _aim = _aim*_rad;
    ngl::Vec3 bound = _pos+_aim;

    ngl::Vec2 test = ngl::Vec2(-_pos.m_x,_pos.m_z);
    auto aimVec = ngl::Vec2(-_aim.m_x,_aim.m_z);
    aimVec.normalize();

    test += ngl::Vec2(40,40);
    test += aimVec*ngl::Vec2(_rad,_rad);

    m_imageX=floor(test.m_x);
    m_imageY=floor(test.m_y);
    auto px = m_image->getColour(m_imageX,m_imageY);


    if(!px.m_r){std::cout<<"can't go there\n";}

    return !px.m_r;


    //    test /= ngl::Vec2(2.0,2.0); //since each block is 2 wide?

    std::cout<<"eye: "<<floor(test.m_x)<<", "<<floor(test.m_y)<<"\n";




//    for(float z = -m_halfZ; z <= m_halfZ; z++)
//    {
//        for(float x = -m_halfX; x < m_halfX; x++)
//        {
//            //find pixel colour of current cell
//            pixel = m_image->getColour(m_imageX, m_imageZ);
//            if(bound.m_x <= (x+0.5) && bound.m_x >= (x-0.5))
//            {
//                std::cout<<"Collide!\n";
//            }
//            else
//            {
//                std::cout<<"No Collide!\n";
//            }
//            m_imageX++;
//        }
//    m_imageX = 0;
//    m_imageZ++;
//    }
//    return 0;

}


