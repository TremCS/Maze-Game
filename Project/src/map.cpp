#include <iostream>
#include "map.h"
#include "mainDraw.h"
#include <ngl/ShaderLib.h>
#include <ngl/NGLInit.h>
#include <ngl/Material.h>
#include <ngl/Transformation.h>
#include <ngl/VAOPrimitives.h>

map::map()
{
    ngl::VAOPrimitives::instance()->createTrianglePlane(
                "ground", 40, 40, 10, 10, ngl::Vec3::up());
    loadMap();
}

void map::loadMap()
{
    ngl::ShaderLib *shader = ngl::ShaderLib::instance();
    shader->use(ngl::nglColourShader);
    shader->setUniform("Colour", 0.3f, 0.3f, 0.3f, 1.0f);
    shader->setUniform("MVP", m_mouseGlobalTX*m_cam.getVPMatrix());
    ngl::VAOPrimitives::instance()->draw("ground");

}

void map::updateMap()
{
    ngl::VAOPrimitives::instance()->draw("ground");
}
