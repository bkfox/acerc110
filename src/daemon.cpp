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
    int panW{0}, panH{0}, power{3};
    uint32_t wID{0}, framerate{0};

    po::options_description desc("General options");
    desc.add_options()
        ("help", "help message")
        ("bench", "show fps")
        ("width", po::value<int>(&panW), "panning width (default: 0). If -1, use projector capabilities, 0 use the window/screen width")
        ("height", po::value<int>(&panH), "panning height (default: 0). If -1, use projector capabilities, 0 use the window/screen height")
        ("power", po::value<int>(&power), "projector power (between 1, 2 or 3)")
        ("window", po::value<uint32_t>(&wID), "window ID to project")
        ("framerate", po::value<uint32_t>(&framerate), "limit at a maximal framerate (default: not limited)")
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
    if(power > 3) power = 3;
    else if(power < 1) power = 1;

    // framerate
    __useconds_t us{0};
    if(framerate)
        us = 1000000 / framerate;

    // prepare main objects
    Scale scale(PROJECTOR_WIDTH, PROJECTOR_HEIGHT);
    Projector proj((Power)power, &scale);
    ScreenshotXShm scr(panW, panH, wID);

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


