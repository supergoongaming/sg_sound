#include <SupergoonSound/gnpch.h>
#include <SupergoonSound/sound/sound.h>

static bool shouldQuit = false;
static SDL_Event event;
static SDL_Renderer *renderer;

static int CreateSdlWindow()
{
    const char *windowName = "SoundTesting";
    int width = 640;
    int height = 480;
    SDL_Window *window = SDL_CreateWindow(windowName,
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          width, height,
                                          SDL_WINDOW_SHOWN);
    // if the window creation succeeded create our renderer
    if (window == NULL)
    {
        fprintf(stderr, "Window could not be created, Error: %s", SDL_GetError());
        return 0;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        fprintf(stderr, "Renderer could not be created, Error: %s", SDL_GetError());
    }
    fprintf(stdout, "Created window\nWidth: %d, Height: %d\n", width, height);
    return 0;
}

void loop_func()
{
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            shouldQuit = true; // Quit the loop if the window close button is clicked
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_q)
            {
                shouldQuit = true; // Quit the loop if 'q' key is pressed
            }
            break;
        default:
            break;
        }
    }
    UpdateSound();
    if (shouldQuit)
        return;
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0)
    {
        fprintf(stderr, "Could not Initialize SDL!\nError: %s", SDL_GetError());
        return 1;
    }
    CreateSdlWindow();
    InitializeSound();
    Bgm *mainBgm = LoadBgm("test.ogg", 20.397, 43.08);
    PlayBgm(mainBgm, 1.0);
    while (!shouldQuit)
    {
#ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(loop_func, 60, 1);
#else
        loop_func();
        SDL_Delay(16);
#endif
    }
    SDL_Quit();
}