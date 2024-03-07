#include <SupergoonSound/gnpch.h>
#include <SupergoonSound/include/sound.h>
// EMSCRIPTEN
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


void loop_func(void)
{
    UpdateSound();
}

int main(void)
{
    int result = InitializeSound();
    printf("Result is %d\n", result);
    LoadBgm("test.ogg", 20.397, 43.08);
    result = PlayBgm(1.0);
    printf("Result is %d\n", result);
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(loop_func, 0, 1);
#else
    while (true)
    {
        loop_func();
    }
#endif
}