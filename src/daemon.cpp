/*
Copyright (C) 2012 - Thomas Baquet (aka lordblackfox)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <boost/program_options.hpp>

extern "C" {
    #include <time.h>
    #include <unistd.h>
}

#include "../lib/projector.hpp"
#include "../lib/screenshotXShm.hpp"

#ifndef  CLOCKS_PER_SEC
    #define CLOCKS_PER_SEC  1000000l
#endif

using namespace std;
using namespace boost;
using namespace am7x01;

namespace po = boost::program_options;

int main (int argc, char ** argv) {
    int panW{0}, panH{0}, power{3}, zoom{2};
    uint32_t wID{0}, framerate{30};

    po::options_description desc("Program options");
    desc.add_options()
        ("help", "help message")
        ("bench", "show fps")
        ("test", "show test picture")
        ("pick", "select a window using mouse click")
        ("power", po::value<int>(&power), "projector power (between 1 to 4)")
        ("framerate", po::value<uint32_t>(&framerate), "try to limit at a maximal framerate (default: 30). For unlimited framerate, set it at 0.")
        ("width", po::value<int>(&panW), "panning width (default: 0). If -1, use projector capabilities, 0 use the window/screen width")
        ("height", po::value<int>(&panH), "panning height (default: 0). If -1, use projector capabilities, 0 use the window/screen height")
        ("zoom", po::value<int>(&zoom), "zoom mode if projector must rescale image. 0: no, 1: horizontal, 2: horizontal and vertical (default)")
        ("window", po::value<uint32_t>(&wID), "window ID to project")
#ifndef C110_DISABLE_JPEG
        ("yuv", "Use YUV encoding instead of JPEG")
#endif
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    // help
    if(vm.count("help")) {
        cout << desc << endl;
        return 0;
    }

    // power
    if(power > 4)       power = 4;
    else if(power < 1)  power = 1;


    // zoom
    if(vm.count("test"))
        zoom = 4;
    else if(zoom < 0)   zoom = 0;
    else if(zoom > 2)   zoom = 2;

    // framerate
    __useconds_t us{0};
    if(framerate)
        us = 1000000 / framerate;

    // prepare main objects
    Scale scale(PROJECTOR_WIDTH, PROJECTOR_HEIGHT);
    Projector proj((Power)power, (Zoom)zoom, &scale, !vm.count("yuv"));
    ScreenshotXShm scr(panW, panH, wID, vm.count("pick"));

    // lets' go
    proj.assign(&scr);
    if(vm.count("bench")) {
        clock_t start, stop;

        #define N   50
        int frame = 0;
        while(1) {
            frame = N;
            start = clock();
            while(frame--) {
                proj.update();
                usleep(us);
            }
            stop = clock() - start;
            cout << (N * CLOCKS_PER_SEC / (float)stop) << " fps\n";
        }
    }
    else
        while(1) {
            proj.update();
            usleep(us);
        }

    return 0;
}


