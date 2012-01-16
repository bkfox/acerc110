#ifndef     PROJECTOR_HPP
#define     PROJECTOR_HPP

#include "screenshot.hpp"

extern "C" {
    #include <libusb-1.0/libusb.h>
    #include <jpeglib.h>
    #include <jerror.h>
}

#include <iostream>
namespace am7x01 {

    /*
     *  For pointers, the rule is: allocator frees his allocation
     *
     */
    struct Projector {
            bool scale;
            uint32_t window;
            int  width, height, bpp, bpl, resolution;
            J_COLOR_SPACE color;

            /*  For the whole screen set width and height OR window to 0
             *  if scale = true, height and width will be the dimension to be scaled to
             *  otherwise, it is the size of the source
             */
            Projector (const Power power = HIGH, const bool scale = false,
                       const int width = 800, const int height = 480, const uint32_t window = 0);

            ~Projector ();

            void update ();
            void setPower (const Power);

            /*  Projector doesn't free IScreenshot after use */
            void assign (IScreenshot *);

            /* ! bytes per pixel */
            void setBpp (int, J_COLOR_SPACE = JCS_RGB);

        private:
            unsigned char* buffer;
            uint64_t       bufferSize;

            libusb_device_handle *dev;
            IScreenshot *shooter;

            header iHeader;

            void send(const void *buffer, const unsigned int len);

            /*  The function suppose that src has been allocated following (width * height * bpp);
             *  compress doesn't free src and dst; src must be at 3 bytes per pixel in RGB.
             */
            void compress (unsigned char *src);
    };

}

#endif

