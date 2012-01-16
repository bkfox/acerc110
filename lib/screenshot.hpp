#ifndef     SCREENSHOT_HPP
#define     SCREENSHOT_HPP

#include "am7xxx.hpp"

#include <string>

namespace am7x01 {
    struct Projector;   // projector.hpp

    struct IScreenshot {
        IScreenshot() {};
        IScreenshot(Projector *p) : parent(p) {};
        virtual unsigned char * update () = 0;

        Projector *parent;
    };
}

#endif

