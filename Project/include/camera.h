#ifndef CAMERA_H
#define CAMERA_H
#include <ngl/NGLInit.h>
#include <ngl/Camera.h>
#include <SDL.h>
#include <ngl/Vec3.h>


class camera {
    private:

       ngl::Vec3 m_rotation;

//        ngl::Vec3 m_pos;
//        ngl::Vec3 m_speed;

//        double TO_RADS;

//        bool m_forward;
//        bool m_backward;
//        bool m_strafeLeft;
//        bool m_strafeRight;

        ngl::Camera* m_cam;

    public:
        camera(ngl::Camera*);
        //~camera();

        void lookAround(double mouseX, double mouseY);

        void eventHandler(SDL_Event *_event);

        void update(double m_rot_x, double m_rot_y);

        void moveAround(int key, bool pressed);

        double toRads(const double &degs) const;


};

#endif // CAMERA_H
