#include "mainDraw.h"
#include <ngl/ShaderLib.h>
#include <ngl/NGLInit.h>
#include <ngl/Material.h>
#include <ngl/Transformation.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Image.h>
#include <math.h>
#include <Text.h>

#define PI 3.141593654
#define TO_RADS PI / 180

mainDraw::mainDraw(int _w, int _h)
{
        m_rotation=(0.0f);
        m_moving = false;
        spawn = false;
        instructions = false;
        start_time = 0;
        globalSec = 0;
        flip = 0;

        m_text.reset(new Text("fonts/arial.ttf", 100));
        m_text->setColour(1.0,0.0,0.0);

        m_text_small.reset(new Text("fonts/arial.ttf", 30));
        m_text_small->setColour(1.0,0.0,0.0);

//        m_xLast = _w/4;
//        m_yLast = _h/4;
        m_origX = _w/2;
        m_origY = _h/2;

        // now create our light this is done after the camera so we can pass the
        // transpose of the projection matrix to the light to do correct eye space
        // transformations



        m_game = MENU;
        m_cam= new ngl::Camera();
        setCameraStartup(m_cam, _w, _h);
        m_map_startup = new map(m_cam, fname, eye.m_x, eye.m_z);

        ngl::Mat4 iv=m_cam->getViewMatrix();
        iv.transpose();
        m_light = new ngl::Light(ngl::Vec3(-2,5,2),ngl::Colour(1,1,1,1),ngl::Colour(1,1,1,1),ngl::LightModes::POINTLIGHT );
        m_light->setTransform(iv);
        // load these values to the shader as well
        m_light->loadToShader("light");
}

mainDraw::~mainDraw()
{
    std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
    delete m_light;
    delete m_cam;
    delete m_mainmap;
    //delete m_mainmap;
}

void mainDraw::setCamera(ngl::Camera *m_cam, int _w, int _h)
{
    // FIRST PERSON CAMERA
    eye= ngl::Vec3(36.5f, 1.0f, 36.0f);//m_mainmap->spawnPos();  //this will be the player position
    aim = ngl::Vec3(0.0f,0.0f,1.0f);
    right=ngl::Vec3(0.0f,0.0f,0.0f);
    look=eye+aim; //this is the player position + the direction they're looking in
    up=ngl::Vec3(0.0f,1.0f,0.0f);


    m_cam->set(eye, look, up);

    m_cam->setShape(60,(float)_w/_h,0.01,1);

}

void mainDraw::setCameraStartup(ngl::Camera *m_cam, int _w, int _h)
{
    eye= ngl::Vec3(36.5f, 1.0f, 36.0f);//m_mainmap->spawnPos();  //this will be the player position
    aim = ngl::Vec3(0.0f,0.0f,-1.0f);
    right=ngl::Vec3(0.0f,0.0f,0.0f);
    look=eye+aim; //this is the player position + the direction they're looking in
    up=ngl::Vec3(0.0f,1.0f,0.0f);


    m_cam->set(eye, look, up);

    m_cam->setShape(60,(float)_w/_h,0.01,1);
}

void mainDraw::resize(int _w, int _h)
{
    m_text->setScreenSize(_w,_h);
    glViewport(0,0,_w,_h);
    // now set the camera size values as the screen size has changed
    m_cam->setShape(60,(float)_w/_h,0.01,1);
    //m_text->setTransform(_w, _h);
}

void mainDraw::handleEvent(SDL_Event& _event, int _w, int _h)
{
    std::cout<<static_cast<int>(m_game)<<"\n";

    if(m_game == 1)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        switch(_event.key.keysym.sym)
        {
            case SDLK_SPACE:
            {
                instructions = 1;
            }
        }

        switch(_event.button.button)
        {
            case SDL_BUTTON_LEFT:
            {
                delete m_map_startup;
                fname=std::string("maps/Maze.png");
                m_mainmap = new map(m_cam, fname, eye.m_x, eye.m_z);
                setCamera(m_cam, _w, _h);
                m_game = GAME;
                start_time = SDL_GetTicks();
                instructions = 0;
            }
        }
    }

    if(m_game == 2)
    {
        SDL_ShowCursor(SDL_DISABLE);
        lookAround(_event, _w, _h);
        moveAround(_event);
    }

    if(m_game == 3)
    {
        switch(_event.key.keysym.sym)
        {
            case SDLK_SPACE:
            {
                delete m_mainmap;
                m_map_startup = new map(m_cam, fname, eye.m_x, eye.m_z);
                setCameraStartup(m_cam, _w, _h);
                m_game = MENU;
            }
        }
    }

    if(m_game == 4)
    {
        switch(_event.key.keysym.sym)
        {
            case SDLK_SPACE:
            {
                delete m_mainmap;
                m_mainmap = new map(m_cam, fname, eye.m_x, eye.m_z);
                setCamera(m_cam, _w, _h);
                m_game = MENU;
            }
        }
    }


}

void mainDraw::handleEventStartup()
{
    if(m_game == 1)
    {
        m_map_startup->drawStart();
    }
}

void mainDraw::lookAround(SDL_Event& _event, int _w, int _h)
{
    if(_event.type == SDL_MOUSEMOTION)
    {
        //Keeps cursor in the centre of the window
        SDL_WarpMouseInWindow(NULL, m_origX, m_origY);

        //Calculates the value between the centre of the window and the cursor each frame
        x_del += (_event.motion.x-m_origX)*0.001;
        y_del += (_event.motion.y-m_origY)*-0.001;

        //Locks the vertical access to -90 < x < 90 (rounded down to stop screen flip at 90
        if(y_del >= 1.57)
        {
            y_del = 1.57;
        }

        if(y_del <= -1.57)
        {
            y_del = -1.57;
        }
    //    std::cout<< x_del << ", " << y_del <<"\n";
        //Converting radians to cartesian coordinates
        ang_x = cos(x_del)*cos(y_del);
        ang_y = sin(y_del);
        ang_z = sin(x_del)*cos(y_del);


        aim.m_x = ang_x;
        aim.m_z = ang_z;
        aim.m_y = ang_y;

        aim.normalize();

        //Calculating the look vector and setting the camera movement
        look=eye+aim; //this is the player position + the direction they're looking in
        m_cam->set(eye, look, up);
        m_light->setTransform(iv);

        //print lines for look debugging
//        std::cout<<"eye: "<<eye.m_x<<", "<<eye.m_y<<", "<<eye.m_z<<"\n";
//        std::cout<<"aim: "<<aim.m_x<<", "<<aim.m_y<<", "<<aim.m_z<<"\n";
//        std::cout<<"look: "<<look.m_x<<", "<<look.m_y<<", "<<look.m_z<<"\n";
//        std::cout<<"-------------------------\n";
    }
}

void mainDraw::moveAround(SDL_Event& _event)
{
    if(_event.type == SDL_KEYDOWN && _event.key.repeat == 0)
    {
        m_moving = true;
    }

    if(_event.type == SDL_KEYUP  && _event.key.repeat == 0)
    {
        m_moving = false;
    }   

    if(m_moving)
    {


        switch(_event.key.keysym.sym)
        {
        case SDLK_w:
            if(!m_mainmap->collision(eye, aim, 0.5))
            {
                eye.m_x += aim.m_x*0.1;
                eye.m_z += aim.m_z*0.1;
                look=eye+aim;
                m_cam->set(eye, look, up);
            }
            if(m_mainmap->collision_enemies(eye,aim,0.5, flip))
            {
                m_game = GAMEOVER;
            }
            if(m_mainmap->collision_win(eye,aim,0.5))
            {
                m_game = EXIT;
            }
            flip = m_mainmap->collision_sphere(eye,aim,0.5, globalSec);

            break;



        case SDLK_s:
            if(!m_mainmap->collision(eye, -aim, 0.5))
            {
                eye.m_x -= aim.m_x*0.1;
                eye.m_z -= aim.m_z*0.1;
                look=eye+aim;
                m_cam->set(eye, look, up);
            }
            if(m_mainmap->collision_enemies(eye,-aim,0.5, flip))
            {
                m_game = GAMEOVER;
            }
            if(m_mainmap->collision_win(eye,-aim,0.5))
            {
                m_game = EXIT;
            }
            flip = m_mainmap->collision_sphere(eye,-aim,0.5, globalSec);

            break;

        case SDLK_d:
            if(!m_mainmap->collision(eye, aim, 0.5))
            {
                right.cross(aim, up);
                eye += right*0.1;
                look=eye+aim;
                m_cam->set(eye, look, up);
            }
            if(m_mainmap->collision_enemies(eye,aim,0.5, flip))
            {
                m_game = GAMEOVER;
            }
            if(m_mainmap->collision_win(eye,aim,0.5))
            {
                m_game = EXIT;
            }
            flip = m_mainmap->collision_sphere(eye,aim,0.5, globalSec);
            break;



        case SDLK_a:
            if(!m_mainmap->collision(eye, aim, 0.5))
            {
                right.cross(aim, up);
                eye -= right*0.1;
                look=eye+aim;
                m_cam->set(eye, look, up);
            }
            if(m_mainmap->collision_enemies(eye,aim,0.5, flip))
            {
                m_game = GAMEOVER;
            }
            if(m_mainmap->collision_win(eye,aim,0.5))
            {
                m_game = EXIT;
            }
            flip = m_mainmap->collision_sphere(eye,aim,0.5, globalSec);
            break;

        }
//        std::cout<<"eye: "<<eye.m_x<<", "<<eye.m_y<<", "<<eye.m_z<<"\n";

    }
}

void mainDraw::updateEvent()
{
//    std::cout<<"update event\n";
}

void mainDraw::draw()
{
    if(m_game == 1)
    {
        if(!instructions)
        {
            m_text->setColour(1,1,1);

            m_text->renderText(150, 175, "Curse of the Teapots");

            m_text_small->setColour(1,1,1);

            m_text_small->renderText(400, 600, "Click to play, or Space for instructions");
        }
        else
        {
            m_text_small->setColour(1,1,1);

            m_text_small->renderText(30, 200, "Don't hit these guys if you're on the ceiling!");
            m_text_small->renderText(800, 230, "Or these guys on the floor!");
            m_text_small->renderText(500, 500, "Hit these to flip the map!");
            m_text_small->renderText(500, 600, "Good luck!  Click to play");
        }

    }
    if(m_game == 2)
    {
        // clear the screen and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        m_mainmap->draw(flip);
        time = SDL_GetTicks()-start_time;
        min = time/60000;
        sec = (time/1000)-(min*60);
        globalSec = (time/1000);
        m_text_small->setColour(1,1,1);
        if(min<=9)
        {
            m_text_small->renderText(20, 20, std::to_string(min));
        }
        else
        {
            m_text_small->renderText(2, 20, std::to_string(min));
        }
        if(sec<=9)
        {
            m_text_small->renderText(38, 20, ":0"+std::to_string(sec));
        }
        else
        {
            m_text_small->renderText(38, 20, ":"+std::to_string(sec));
        }
    }

    if(m_game == 3)
    {
        m_text->setColour(1,1,1);

        m_text->renderText(375, 200, "Game Over!");

        m_text_small->setColour(1,1,1);

        m_text_small->renderText(420, 500, "Press Space to restart or ESC to quit");
    }

    if(m_game == 4)
    {
        m_text->setColour(1,1,1);

        m_text->renderText(375, 200, "You're winner!");

        m_text->setColour(1,1,1);

        m_text->renderText(375, 400, "Time: "+std::to_string(min)+":"+std::to_string(sec));

        m_text_small->setColour(1,1,1);

        m_text_small->renderText(500, 600, "Press Space to restart or ESC to quit");
    }

}

void mainDraw::loadMatricesToShader()
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();

    ngl::Mat4 MV;
    ngl::Mat4 MVP;
    ngl::Mat3 normalMatrix;
    ngl::Mat4 M;
    M=m_mouseGlobalTX;
    MV=  M*m_cam->getViewMatrix();
    MVP= M*m_cam->getVPMatrix();
    normalMatrix=MV;
    normalMatrix.inverse();
    shader->setShaderParamFromMat4("MV",MV);
    shader->setShaderParamFromMat4("MVP",MVP);
    shader->setShaderParamFromMat3("normalMatrix",normalMatrix);
    shader->setShaderParamFromMat4("M",M);
}
