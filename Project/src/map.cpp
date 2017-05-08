#include <iostream>
#include "map.h"
#include "mainDraw.h"
#include <ngl/ShaderLib.h>
#include <ngl/NGLInit.h>
#include <ngl/Material.h>
#include <ngl/Transformation.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Image.h>

map::map(ngl::Camera* _cam, const std::string &_fname)
{
    m_image = new ngl::Image(_fname));

    loadMap();

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
    GLuint w=m_image->width();
    GLuint h=m_image->height();
    ngl::Transformation tx;
    ngl::VAOPrimitives::instance()->createCylinder(
                "cube", 2, 5, 4, 4);

    ngl::ShaderLib *shader= ngl::ShaderLib::instance();
    shader->use(ngl::nglColourShader);
    shader->setUniform("Colour",1.0f,0.0f, 0.0f,1.0f);

    ngl::Colour pixel;

    auto toFloat=[=](float _r){return _r/255.0f;};

    //image mapping
    GLuint imageX=0;
    GLuint imageY=0;
    float halfZ=h/2;
    float halfX=w/2;

    for(float z=-halfZ;z<halfZ; ++z)
    {
        for(float x=-halfX; x<halfX; ++x)
        {
            std::cout<<"colour"<<'\n';
            pixel = m_image->getColour(imageX,imageY);

            if(!FCompare(pixel.m_r,255) || !FCompare(pixel.m_g,255) ||  !FCompare(pixel.m_b,255))
            {

            tx.setPosition(-x,0,z);
            shader->setUniform("MVP",tx.getMatrix()*m_cam->getVPMatrix());
            shader->setUniform("Colour", toFloat(pixel.m_r), toFloat(pixel.m_g), toFloat(pixel.m_b), 1.0);
            ngl::VAOPrimitives::instance()->draw("cube");



            }//end if
            ++imageX;

        }//end X
        imageX=0;
        ++imageY;
    }//endZ
    ngl::VAOPrimitives::instance()->createTrianglePlane(
                "ground", 40, 40, 10, 10, ngl::Vec3::up());


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

//    ngl::VAOPrimitives::instance()->draw("teapot");

}
