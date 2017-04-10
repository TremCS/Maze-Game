#include "camera.h"
#include <math.h>
#include <iostream>

#define PI 3.141592654
#define TO_RADS 3.141592654 / 180

camera::camera(ngl::Camera* _cam)
{
    m_cam = _cam;

    m_rotation=(0.0f);

//    m_forward = false;
//    m_backward = false;
//    m_strafeLeft = false;
//    m_strafeRight = false;


    //TO_RADS = 3.141592654 / 180.0;
}

void camera::eventHandler(SDL_Event* _event)
{
    std::cout<<"camera handle event\n";


    m_rotation.m_x = _event->motion.x;

    std::cout<<m_rotation.m_x;
    m_cam->yaw(m_rotation.m_x);


//    double mouseX = _event->motion.x;
//    double mouseY = _event->motion.y;

    //lookAround(mouseX, mouseY);
}

void camera::lookAround(double mouseX, double mouseY)
{

//   // m_rot_x = (mouseX-m_centreX)*m_yawSens;
//   // m_rot_y = (mouseY-m_centreY)*m_pitchSens;

//    if(m_rot_x < -PI*0.5f) { m_rot_x = -PI*0.5f; }
//    if(m_rot_y < -PI*0.5f) { m_rot_y = -PI*0.5f; }

//    if (m_rot_x< 0.0f) m_rot_x = 0.0f;
//    if (m_rot_y > PI*2.0f) m_rot_y = PI*2.0f;

//    update(m_rot_x, m_rot_y);


}

double toRads(double degs)
{
   //return degs * TO_RADS;
}

void camera::update(double m_rot_x, double m_rot_y)
{
//    std::cout<<m_rot_x;
//    cam->normalisedYaw(m_rot_x);
//    cam->normalisedPitch(m_rot_y);
}
