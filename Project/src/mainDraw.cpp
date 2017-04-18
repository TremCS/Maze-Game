#include "mainDraw.h"
#include <ngl/ShaderLib.h>
#include <ngl/NGLInit.h>
#include <ngl/Material.h>
#include <ngl/Transformation.h>
#include <ngl/VAOPrimitives.h>
#include <math.h>

const static float INCREMENT=0.01;
const static float ZOOM=0.05;
mainDraw::mainDraw()
{
        m_rotation=(0.0f);


        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
        // enable depth testing for drawing
        glEnable(GL_DEPTH_TEST);
        // now to load the shader and set the values
        // grab an instance of shader manager
        ngl::ShaderLib *shader=ngl::ShaderLib::instance();
        // we are creating a shader called Phong
        shader->createShaderProgram("Phong");
        // now we are going to create empty shaders for Frag and Vert
        shader->attachShader("PhongVertex",ngl::ShaderType::VERTEX);
        shader->attachShader("PhongFragment",ngl::ShaderType::FRAGMENT);
        // attach the source
        shader->loadShaderSource("PhongVertex","shaders/PhongVertex.glsl");
        shader->loadShaderSource("PhongFragment","shaders/PhongFragment.glsl");
        // compile the shaders
        shader->compileShader("PhongVertex");
        shader->compileShader("PhongFragment");
        // add them to the program
        shader->attachShaderToProgram("Phong","PhongVertex");
        shader->attachShaderToProgram("Phong","PhongFragment");
        // now bind the shader attributes for most NGL primitives we use the following
        // layout attribute 0 is the vertex data (x,y,z)
        shader->bindAttribute("Phong",0,"inVert");
        // attribute 1 is the UV data u,v (if present)
        shader->bindAttribute("Phong",1,"inUV");
        // attribute 2 are the normals x,y,z
        shader->bindAttribute("Phong",2,"inNormal");

        // now we have associated this data we can link the shader
        shader->linkProgramObject("Phong");
        // and make it active ready to load values
        (*shader)["Phong"]->use();
        // the shader will use the currently active material and light0 so set them
        ngl::Material m(ngl::STDMAT::GOLD);
        // load our material values to the shader into the structure material (see Vertex shader)
        m.loadToShader("material");

        setCamera();

        // now create our light this is done after the camera so we can pass the
        // transpose of the projection matrix to the light to do correct eye space
        // transformations
        ngl::Mat4 iv=m_cam->getViewMatrix();
        iv.transpose();
        m_light = new ngl::Light(ngl::Vec3(-2,5,2),ngl::Colour(1,1,1,1),ngl::Colour(1,1,1,1),ngl::LightModes::POINTLIGHT );
        m_light->setTransform(iv);
        // load these values to the shader as well
        m_light->loadToShader("light");

//        map m_mainmap(m_cam);
        m_mainmap = new map(m_cam);
}

void mainDraw::setCamera()
{
    // FIRST PERSON CAMERA
    eye=ngl::Vec3(1.0f,1.0f,1.0f);  //this will be the player position
    aim = ngl::Vec3(0.0f,0.0f,1.0f);
    right=ngl::Vec3(0.0f,0.0f,0.0f);
    look=eye+aim; //this is the player position + the direction they're looking in
    up=ngl::Vec3(0.0f,1.0f,0.0f);

    m_cam= new ngl::Camera();
    m_cam->set(eye, look, up);
    // set the shape using FOV 45 Aspect Ratio based on Width and Height
    // The final two are near and far clipping planes of 0.5 and 10
    m_cam->setShape(45,(float)720.0/576.0,0.05,350);
}

mainDraw::~mainDraw()
{
    std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
    delete m_light;
    delete m_cam;
    //delete m_mainmap;
}

void mainDraw::resize(int _w, int _h)
{
    glViewport(0,0,_w,_h);
    // now set the camera size values as the screen size has changed
    m_cam->setShape(90,(float)_w/_h,0.05,350);
}

void mainDraw::handleEvent(SDL_Event* _event, int _w, int _h)
{
    std::cout<<"handle event\n";



    if(_event->type == SDL_MOUSEMOTION){lookAround(_event, _w, _h);}


}

void mainDraw::lookAround(SDL_Event *_event, int _w, int _h)
{

    //std::cout<<x_orig;
    m_origX = _w/4;
    m_origY = _h/4;


    x_del += 0.0001*(m_origX-_event->motion.x);
    y_del += 0.0001*(m_origY-_event->motion.y);

    std::cout<<x_del<<' '<<y_del<<'\n';


//    std::cout<<_event->motion.xrel;
//    m_cam->yaw(m_rotation.m_x);
//    m_cam->normalisedPitch(m_rotation.m_y);
//    std::cout<<"looking\n";

//    aim = ngl::Vec3(0.0,0.0,0.0);

    //ang_x = x_del;
    //ang_z = x_del;
    //ang_y = y_del;

    ang_x = sin(y_del)*sin(x_del);
    ang_y = sin(y_del);
    ang_z = cos(y_del)*cos(x_del);

    //std::cout<<ang_x;

//    aim.m_x -= ang_x;
//    aim.m_z += ang_y;
//    aim.m_y -= ang_z;


    aim.m_x = ang_x;
    aim.m_z = ang_z;
    aim.m_y = ang_y;

//    right.m_x = sin(x_del - (3.14/2));
//    right.m_z = cos(x_del - (3.14/2));

    //up = ngl::Vec3.cross(right, aim);

    //up.cross(right, aim);

    //std::cout<<aim.m_x<<", "<<aim.m_y<<", "<<aim.m_z<<"\n";

    look=eye+aim; //this is the player position + the direction they're looking in

    m_cam->set(eye, look, up);

    m_xLast = x_del;
    m_yLast = y_del;

    _event->motion.x = m_origX;
    _event->motion.y = m_origY;

}


void mainDraw::updateEvent()
{
    std::cout<<"update event\n";
    m_mainmap->updateMap();
}

void mainDraw::draw()
{
    // clear the screen and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // grab an instance of the shader manager
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    (*shader)["Phong"]->use();

    // get the VBO instance and draw the built in teapot
    //ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
    // draw
    //loadMatricesToShader();
    //prim->draw("cube");

    m_mainmap->draw();
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
