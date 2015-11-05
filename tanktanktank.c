#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <jansson.h>

#include "hashtbl.h"
#include "hint.h"
#include "jsonutils.h"

#include "tonic.h"

#include "extras.h"
#include "globals.h"
#include "player.h"
#include "room.h"

#include "obj.h"

#if INPUT == CONTROLLER
SDL_GameController *controller;
#endif




// UPDATE THIS




int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    Mix_Init(MIX_INIT_OGG);

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024); // 44.1k, 2 channels, 1kb chunk size
    SDL_Window *win = SDL_CreateWindow("DECL TEST", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, 0);
    SDL_Renderer *r = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    Mix_AllocateChannels(64);
	
    SDL_Event e;
    bool running = true;
    struct room *room = NULL;
#if INPUT == CONTROLLER
    controller = SDL_GameControllerOpen(0);
    SDL_EventState(SDL_CONTROLLERAXISMOTION, SDL_IGNORE); // we handle these ourselves and they clog the input queue
    SDL_EventState(SDL_CONTROLLERBUTTONUP, SDL_IGNORE);
    SDL_EventState(SDL_CONTROLLERBUTTONDOWN, SDL_IGNORE);
#endif

    register_constructors();
    register_updaters();
    register_guns();
    room = instantiate_room("level.json", r);
    
    unsigned int ticks = 0;
    unsigned int last_ticks = SDL_GetTicks();
    unsigned short frames = 0;
    char title[128];
    while(running)
    {
        if ((ticks = SDL_GetTicks()) >= last_ticks + 1000) {
            last_ticks = ticks;
            snprintf(title, 128, "fps: %d bullets: %lld", frames, numBullets);
            SDL_SetWindowTitle(win, title);
            frames = 0;
        }
        room_update(room);
        while (SDL_PollEvent(&e))
            if(e.type == SDL_QUIT)
                running = 0;

        SDL_RenderPresent(r);
        cpSpaceStep(room->space, 1.0/60.0);

        SDL_Delay(1000/60);
        frames++;
    }
    
    printf("CLOSING THE GAME\n");
    release_textures();
    release_sounds();
    release_songs();
#if INPUT == CONTROLLER
    SDL_GameControllerClose(controller);
#endif
    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(win);

    Mix_Quit();
    SDL_Quit();
}
