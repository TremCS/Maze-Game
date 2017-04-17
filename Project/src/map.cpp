#include <iostream>
#include "map.h"
#include "mainDraw.h"
#include <ngl/ShaderLib.h>
#include <ngl/NGLInit.h>
#include <ngl/Material.h>
#include <ngl/Transformation.h>
#include <ngl/VAOPrimitives.h>

map::map(ngl::Camera* _cam)
{
    ngl::VAOPrimitives::instance()->createTrianglePlane(
                "ground", 40, 40, 10, 10, ngl::Vec3::up());
    loadMap();

    m_cam = _cam;

    ngl::ShaderLib *shader = ngl::ShaderLib::instance();
    shader->use(ngl::nglColourShader);
    shader->setUniform("Colour", 0.3f, 0.3f, 0.3f, 1.0f);
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
//    ngl::ShaderLib *shader = ngl::ShaderLib::instance();
//    shader->setUniform("MVP", m_cam.getVPMatrix());



    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
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
}
