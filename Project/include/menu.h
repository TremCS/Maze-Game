#ifndef MENU_H_
#define MENU_H_
#include <ngl/Camera.h>
#include <ngl/Image.h>
#include <SDL_ttf.h>
#include <Text.h>


class menu
{
    private:


    public:
        menu();
        ~menu();
        void mainMenu();
        void gameOver();
        void gameWin();

};


#endif // MENU_H_
