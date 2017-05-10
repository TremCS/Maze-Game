#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include "mainDraw.h"
#include "map.h"
#include <ngl/NGLInit.h>
#include <SDL_ttf.h>

/// @brief function to quit SDL with error message
/// @param[in] _msg the error message to send
void SDLErrorExit(const std::string &_msg);

/// @brief initialize SDL OpenGL context
SDL_GLContext createOpenGLContext( SDL_Window *window);

int mainMenu();

int main(int argc, char * argv[])
{
    // under windows we must use main with argc / v so jus flag unused for params
    NGL_UNUSED(argc);
    NGL_UNUSED(argv);
    // Initialize SDL's Video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
        // Or die on error
        SDLErrorExit("Unable to initialize SDL");
    }

    // now get the size of the display and create a window we need to init the video
    SDL_Rect rect;
    rect.w = 1280;
    rect.h = 720;

    bool quit=false;
    int menuQuit = 0;

    // now create our window
    SDL_Window *window=SDL_CreateWindow("SDLNGL",
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      rect.w,
                                      rect.h,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
                                     );
    // check to see if that worked or exit
    if (!window)
    {
        SDLErrorExit("Unable to create window");
    }

    // Create our opengl context and attach it to our window

    SDL_GLContext glContext=createOpenGLContext(window);
    if(!glContext)
    {
        SDLErrorExit("Problem creating OpenGL context ");
    }
    // make this our current GL context (we can have more than one window but in this case not)
    SDL_GL_MakeCurrent(window, glContext);
    /* This makes our buffer swap syncronized with the monitor's vertical refresh */
    SDL_GL_SetSwapInterval(1);
    // we need to initialise the NGL lib which will load all of the OpenGL functions, this must
    // be done once we have a valid GL context but before we call any GL commands. If we dont do
    // this everything will crash
    ngl::NGLInit::instance();

    menuQuit = mainMenu();

    if(menuQuit == 1)
    {
        quit = true;
    }

    // now clear the screen and swap whilst NGL inits (which may take time)
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(window);
    // flag to indicate if we need to exit

    // sdl event processing data structure
    SDL_Event event;
    // now we create an instance of our ngl class, this will init NGL and setup basic
    // opengl stuff ext. When this falls out of scope the dtor will be called and cleanup
    // our gl stuff
//    mainDraw ngl();

    mainDraw* scene = new mainDraw(rect.w, rect.h);

    // resize the ngl to set the screen size and camera stuff
    scene->resize(rect.w,rect.h);

//    SDL_ShowCursor(SDL_DISABLE);

    //GAME LOOP
    while(!quit)
    {
        while ( SDL_PollEvent(&event) )
        {
                //Quit loop kept separate from every other handler
                switch (event.type)
                {
                    //Exit button click
                    case SDL_QUIT : quit = true; break;

                      case SDL_KEYDOWN:
                      {
                          switch( event.key.keysym.sym )
                          {
                              //Escape key press
                              case SDLK_ESCAPE :  quit = true; break;
                              default : break;
                          } // End of key process
                      } // End of keydown

                  default : break;
                }//End of quit loop

        //EVENT HANDLING
        scene->handleEvent(event, rect.w, rect.h);
        }


        //UPDATE HANDLING
        scene->updateEvent();

        //DRAW HANDLING
        scene->draw();

        // swap the buffers and shit on my kangaroo boss
        SDL_GL_SwapWindow(window);


    }

    // now tidy up and exit SDL
    SDL_Quit();
    // whilst this code will never execute under windows we need to have a return from
    // SDL_Main!
    return EXIT_SUCCESS;
}


SDL_GLContext createOpenGLContext(SDL_Window *window)
{
  // Request an opengl 3.2 context first we setup our attributes, if you need any
  // more just add them here before the call to create the context
  // SDL doesn't have the ability to choose which profile at this time of writing,
  // but it should default to the core profile
  // for some reason we need this for mac but linux crashes on the latest nvidia drivers
  // under centos
  #ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  #else
    // Note you may have to change this depending upon the driver (Windows is fussy)
    // stick to 3.2 as the core base level for NGL works ok
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  #endif
  // set multi sampling else we get really bad graphics that alias
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);
  // Turn on double buffering with a 24bit Z buffer.
  // You may need to change this to 16 or 32 for your system
  // on mac up to 32 will work but under linux centos build only 16
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  // enable double buffering (should be on by default)
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  //
  return SDL_GL_CreateContext(window);

}

void SDLErrorExit(const std::string &_msg)
{
  std::cerr<<_msg<<"\n";
  std::cerr<<SDL_GetError()<<"\n";
  SDL_Quit();
  exit(EXIT_FAILURE);
}

int mainMenu()
{
    int menuDecision = 0;
    bool quit = 0;
    SDL_Event event;
    TTF_Init();
    SDL_Renderer *renderer;
    SDL_Color textColor = {255, 255, 255};
    SDL_Rect PlayRect = {640, 100, 100, 30};

    TTF_Font *font = NULL;
    font = TTF_OpenFont( "16_true_type_fonts/lazy.ttf", 28 );

    SDL_Surface *PlaySurface = TTF_RenderText_Solid(font, "Play", textColor);
    SDL_Texture *mTexture = SDL_CreateTextureFromSurface( renderer, PlaySurface );

    SDL_RenderCopy(renderer, mTexture, NULL, &PlayRect);

    while(!quit)
    {
        while ( SDL_PollEvent(&event) )
        {
                //Quit loop kept separate from every other handler
                switch (event.type)
                {
                    //Exit button click
                      case SDL_QUIT : quit = true; menuDecision = 1; break;

                      case SDL_KEYDOWN:
                      {
                          switch( event.key.keysym.sym )
                          {
                              //Escape key press
                              case SDLK_ESCAPE :  quit = true; menuDecision = 1; break;
                              default : break;
                          } // End of key process
                      } // End of keydown

                      case SDL_MOUSEBUTTONDOWN:
                      {
                            switch(event.button.button)
                            {
                                case SDL_BUTTON_LEFT:  quit = true; menuDecision = 2; break;
                            }
                      }
                  default : break;
                }//End of quit loop

        }

    }
    return menuDecision;
}
