#include <iostream>
#include "map.h"
#include "mainDraw.h"
#include <ngl/ShaderLib.h>
#include <ngl/NGLInit.h>
#include <ngl/Material.h>
#include <ngl/Transformation.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Image.h>
#include <ngl/Texture.h>

map::map(ngl::Camera* _cam, const std::string &_fname, float _x, float _z)
{
    m_image.reset(new ngl::Image(_fname));
    flip = 0;
    lastSec = 0;

    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    // load a frag and vert shaders

    shader->createShaderProgram("TextureShader");

    startup_rotation = 0;

    shader->attachShader("TextureVertex",ngl::ShaderType::VERTEX);
    shader->attachShader("TextureFragment",ngl::ShaderType::FRAGMENT);
    shader->loadShaderSource("TextureVertex","shaders/TextureVert.glsl");
    shader->loadShaderSource("TextureFragment","shaders/TextureFrag.glsl");

    shader->compileShader("TextureVertex");
    shader->compileShader("TextureFragment");
    shader->attachShaderToProgram("TextureShader","TextureVertex");
    shader->attachShaderToProgram("TextureShader","TextureFragment");


    shader->linkProgramObject("TextureShader");
    shader->use("TextureShader");

    m_image_wall.reset(new ngl::Image("img/wall.png"));

    glGenTextures(1,&m_textureWall);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,m_textureWall);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,m_image_wall->getPixels());
    glGenerateMipmap(GL_TEXTURE_2D);

    m_image_floor.reset(new ngl::Image("img/floor.png"));

    glGenTextures(1,&m_textureFloor);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,m_textureFloor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,m_image_floor->getPixels());
    glGenerateMipmap(GL_TEXTURE_2D);

    m_image_ceiling.reset(new ngl::Image("img/ceiling.png"));

    glGenTextures(1,&m_textureCeiling);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,m_textureCeiling);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,m_image_ceiling->getPixels());
    glGenerateMipmap(GL_TEXTURE_2D);

    m_image_monster.reset(new ngl::Image("img/monster.png"));

    glGenTextures(1,&m_textureMonster);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D,m_textureMonster);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,m_image_monster->getPixels());
    glGenerateMipmap(GL_TEXTURE_2D);

    m_image_sphere.reset(new ngl::Image("img/sphere.png"));

    glGenTextures(1,&m_textureSphere);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D,m_textureSphere);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,m_image_sphere->getPixels());
    glGenerateMipmap(GL_TEXTURE_2D);

    lastX = _x;
    lastZ = _z;
    ngl::Colour pixel(0.0f, 0.0f, 0.0f);
    ngl::Vec3 startPos(0.0f, 1.0f, 0.0f);
    m_cam = _cam;

}

map::~map()
{

}


void map::loadMatricesToShader(int texUnit, ngl::Vec2 scaleModTex)
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    (*shader)["TextureShader"]->use();

    GLint pid = shader->getProgramID("TextureShader");

    ngl::Mat4 MVP=tx.getMatrix()*m_mouseGlobalTX*m_cam->getVPMatrix();

    shader->setRegisteredUniform("MVP",MVP);
    shader->setRegisteredUniform("scaleMod",scaleModTex);

    glUniform1i(glGetUniformLocation(pid, "tex"), //location of uniform
                           texUnit);

}




void map::draw(bool flip)
{

    float troll_scale;
    float troll_pos;
    float pot_scale;
    float pot_pos;
    float cube_scale;
    if(flip)
    {
        troll_scale = -1;
        troll_pos = 1.5;
        pot_scale = 1;
        pot_pos = 0.4;
        cube_scale = -3;
    }
    else
    {
        troll_scale = 1;
        troll_pos = 0.6;
        pot_scale = -1;
        pot_pos = 1.5;
        cube_scale = 3;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(flip)
    {
        glBindTexture(GL_TEXTURE_2D,m_textureCeiling);

        ngl::VAOPrimitives::instance()->createTrianglePlane(
                    "ceiling", 80, 80, 10, 10, -ngl::Vec3::up());

        tx.setPosition(0,2,0);
        loadMatricesToShader(1, ngl::Vec2(10,1));

        ngl::VAOPrimitives::instance()->draw("ceiling");
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D,m_textureFloor);

        ngl::VAOPrimitives::instance()->createTrianglePlane(
                    "ground", 80, 80, 10, 10, ngl::Vec3::up());

        tx.setPosition(0,0,0);


        loadMatricesToShader(1, ngl::Vec2(10,10));

        ngl::VAOPrimitives::instance()->draw("ground");
    }



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

            if(!FCompare(pixel.m_r,255) && !FCompare(pixel.m_g,255) && !FCompare(pixel.m_b,255))
            {

                ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
                glBindTexture(GL_TEXTURE_2D,m_textureWall);

                tx.setPosition(-x,1,z);
                tx.setScale(1, cube_scale, 1);

                loadMatricesToShader(0, ngl::Vec2(1,5));
                prim->draw("cube");

            }
            if(FCompare(pixel.m_r, 0) && FCompare(pixel.m_g,255) && FCompare(pixel.m_b,0))
            {
                ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
                glBindTexture(GL_TEXTURE_2D,m_textureMonster);
                tx.setPosition(-x,troll_pos,z);
                tx.setScale(1, troll_scale, 1);
                loadMatricesToShader(3, ngl::Vec2(1,1));
                prim->draw("troll");
            }

            if(FCompare(pixel.m_r, 255) && FCompare(pixel.m_g,255) && FCompare(pixel.m_b,0))
            {
                ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
                glBindTexture(GL_TEXTURE_2D,m_textureMonster);
                tx.setPosition(-x,pot_pos,z);
                tx.setScale(1, pot_scale, 1);
                loadMatricesToShader(3, ngl::Vec2(1,1));
                prim->draw("teapot");
            }
            if(FCompare(pixel.m_r, 255) && FCompare(pixel.m_g,0) && FCompare(pixel.m_b,0))
            {
                ngl::VAOPrimitives::instance()->createSphere(
                            "sphere", 0.5, 10);
                glBindTexture(GL_TEXTURE_2D,m_textureSphere);
                tx.setPosition(-x,1,z);
                tx.setScale(1, 1, 1);
                loadMatricesToShader(4, ngl::Vec2(1,1));
                ngl::VAOPrimitives::instance()->draw("sphere");
            }

            ++imageX;

        }//end X
        imageX=0;
        ++imageY;
    }//endZ

    if(flip)
    {
        glBindTexture(GL_TEXTURE_2D,m_textureFloor);

        ngl::VAOPrimitives::instance()->createTrianglePlane(
                    "ground", 80, 80, 10, 10, ngl::Vec3::up());

        tx.setPosition(0,0,0);


        loadMatricesToShader(2, ngl::Vec2(10,10));

        ngl::VAOPrimitives::instance()->draw("ground");
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D,m_textureCeiling);
        ngl::VAOPrimitives::instance()->createTrianglePlane(
                    "ceiling", 80, 80, 10, 10, -ngl::Vec3::up());

        tx.setPosition(0,2,0);
        loadMatricesToShader(2, ngl::Vec2(10,1));

        ngl::VAOPrimitives::instance()->draw("ceiling");
    }




//    ngl::VAOPrimitives::instance()->draw("teapot");

}

void map::drawStart()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D,m_textureFloor);

    ngl::VAOPrimitives::instance()->createTrianglePlane(
                "ground", 10, 10, 10, 10, ngl::Vec3::up());

    tx.setPosition(36.5,0,37);
    tx.setRotation(0, 0, 0);


    loadMatricesToShader(1, ngl::Vec2(1,1));

    ngl::VAOPrimitives::instance()->draw("ground");

    for(float x = 30; x<43; x++ )
    {
        ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
        glBindTexture(GL_TEXTURE_2D,m_textureWall);

        tx.setPosition(x,1, 32.5);
        tx.setScale(1, 3, 1);
        tx.setRotation(0, 0, 0);

        loadMatricesToShader(0, ngl::Vec2(1,5));
        prim->draw("cube");
    }

    glBindTexture(GL_TEXTURE_2D,m_textureCeiling);
    ngl::VAOPrimitives::instance()->createTrianglePlane(
                "ceiling", 10, 10, 10, 10, -ngl::Vec3::up());

    tx.setPosition(36.5,2,37);
    tx.setRotation(0, 0, 0);
    loadMatricesToShader(2, ngl::Vec2(1,1));

    ngl::VAOPrimitives::instance()->draw("ceiling");

    ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
    glBindTexture(GL_TEXTURE_2D,m_textureMonster);
    tx.setPosition(34.5,0.5,34);
    tx.setScale(0.8, 0.8, 0.8);
    tx.setRotation(0, startup_rotation, 0);
    loadMatricesToShader(3, ngl::Vec2(1,1));
    prim->draw("teapot");


    glBindTexture(GL_TEXTURE_2D,m_textureMonster);
    tx.setPosition(37.7,0.8,35);
    tx.setScale(1, 1, 1);
    tx.setRotation(0, startup_rotation*1.3, 0);
    loadMatricesToShader(3, ngl::Vec2(1,1));
    prim->draw("troll");


    ngl::VAOPrimitives::instance()->createSphere(
                "sphere", 0.5, 10);
    glBindTexture(GL_TEXTURE_2D,m_textureSphere);
    tx.setPosition(36.5,0.8,35);
    tx.setScale(1, 1, 1);
    tx.setRotation(0, -startup_rotation, 0);
    loadMatricesToShader(4, ngl::Vec2(1,1));
    ngl::VAOPrimitives::instance()->draw("sphere");


    startup_rotation = startup_rotation + 0.5;


}

bool map::collision(ngl::Vec3 _pos, ngl::Vec3 _aim, float _rad)
{
    //Bottom left of image
    bool collision = 0;
    GLuint m_imageX = 0;
    GLuint m_imageY = 0;


    //aim vector resize to map proportions
    _aim = _aim*_rad;

    ngl::Vec2 test = ngl::Vec2(-_pos.m_x,_pos.m_z);
    auto aimVec = ngl::Vec2(-_aim.m_x,_aim.m_z);
    aimVec.normalize();

    test += ngl::Vec2(40,40);
    test += aimVec*ngl::Vec2(_rad,_rad);

    m_imageX=floor(test.m_x);
    m_imageY=floor(test.m_y);
    auto px = m_image->getColour(m_imageX,m_imageY);


    if(FCompare(px.m_r,0) && FCompare(px.m_g,0) && FCompare(px.m_b,0)){std::cout<<"can't go there\n"; collision = 1;}

    return collision;
//    return 0; //Turns off collision, for testing

}

bool map::collision_enemies(ngl::Vec3 _pos, ngl::Vec3 _aim, float _rad, bool flip)
{
    //Bottom left of image
    bool dead = 0;
    GLuint m_imageX = 0;
    GLuint m_imageY = 0;

    //aim vector resize to map proportions
    _aim = _aim*_rad;

    ngl::Vec2 test = ngl::Vec2(-_pos.m_x,_pos.m_z);
    auto aimVec = ngl::Vec2(-_aim.m_x,_aim.m_z);
    aimVec.normalize();

    test += ngl::Vec2(40,40);
    test += aimVec*ngl::Vec2(_rad,_rad);

    m_imageX=floor(test.m_x);
    m_imageY=floor(test.m_y);
    auto px = m_image->getColour(m_imageX,m_imageY);

    if(flip)
    {
        if(FCompare(px.m_r, 255) && FCompare(px.m_g, 255) && FCompare(px.m_b, 0)){std::cout<<"dead!"; dead = 1;}
    }
    else
    {
        if(FCompare(px.m_r, 0) && FCompare(px.m_g, 255) && FCompare(px.m_b, 0)){std::cout<<"dead!"; dead = 1;}
    }

    return dead;
    return 0; //Turns off collision, debug

}

bool map::collision_win(ngl::Vec3 _pos, ngl::Vec3 _aim, float _rad)
{
    //Bottom left of image
    bool win = 0;
    GLuint m_imageX = 0;
    GLuint m_imageY = 0;

    //aim vector resize to map proportions
    _aim = _aim*_rad;

    ngl::Vec2 test = ngl::Vec2(-_pos.m_x,_pos.m_z);
    auto aimVec = ngl::Vec2(-_aim.m_x,_aim.m_z);
    aimVec.normalize();

    test += ngl::Vec2(40,40);
    test += aimVec*ngl::Vec2(_rad,_rad);

    m_imageX=floor(test.m_x);
    m_imageY=floor(test.m_y);
    auto px = m_image->getColour(m_imageX,m_imageY);


    if(FCompare(px.m_r, 0) && FCompare(px.m_b, 255)){std::cout<<"win!"; win = 1;}

    return win;
    return 0; //Turns off collision, debug

}

bool map::collision_sphere(ngl::Vec3 _pos, ngl::Vec3 _aim, float _rad, int sec)
{
    //Bottom left of image
    GLuint m_imageX = 0;
    GLuint m_imageY = 0;

    //aim vector resize to map proportions
    _aim = _aim*_rad;

    ngl::Vec2 test = ngl::Vec2(-_pos.m_x,_pos.m_z);
    auto aimVec = ngl::Vec2(-_aim.m_x,_aim.m_z);
    aimVec.normalize();

    test += ngl::Vec2(40,40);
    test += aimVec*ngl::Vec2(_rad,_rad);

    m_imageX=floor(test.m_x);
    m_imageY=floor(test.m_y);
    auto px = m_image->getColour(m_imageX,m_imageY);

    if(sec-lastSec > 5)
    {
        if(FCompare(px.m_r, 255) && FCompare(px.m_b, 0) && FCompare(px.m_g,0)){std::cout<<"flip!"; flip = !flip;}
        lastSec = sec;
    }

    return flip;
    return 0; //Turns off collision, debug

}
