#include <time.h>
#include <iostream>

#include "lib/projector.hpp"
#include "lib/screenshotXShm.hpp"

#ifndef  CLOCKS_PER_SEC
    #define CLOCKS_PER_SEC  1000000l
#endif

int main () {
    using namespace am7x01;

    Scale scale(PROJECTOR_WIDTH, PROJECTOR_HEIGHT);
    Projector proj(HIGH, &scale);
    ScreenshotXShm scr(0, 0);

    proj.assign(&scr);

    clock_t start, stop;

    #define N   50
    int frame = 0;
    while(1) {
        frame = N;
        start = clock();
        while(frame--)
            proj.update();
        stop = clock() - start;
        std::cout << (N * CLOCKS_PER_SEC / (float)stop) << " fps\n";
    }
}


