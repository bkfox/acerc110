#ifndef     SCREENSHOT_HPP
#define     SCREENSHOT_HPP

#include "am7xxx.hpp"

extern "C" {
    #include <libusb-1.0/libusb.h>
    #include <stdio.h>
    #include <jpeglib.h>
}

namespace am7x01 {
    struct Projector;   // projector.hpp


    struct Pixel {
        unsigned char r, g, b;
    };

    struct PixelA {
        unsigned char r, g, b, a;
    };

    struct Image {
        uint32_t width;
        uint32_t height;
        char     channels;
        uint32_t bpl;
        J_COLOR_SPACE color;

        uint64_t size;
        unsigned char *data;
    };


    struct IScreenshot {
        virtual Image update () = 0;
        Projector *parent;
    };
}

#endif

