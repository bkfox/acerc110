#ifndef     PROJECTOR_HPP
#define     PROJECTOR_HPP

#include "screenshot.hpp"
#include "transform.hpp"

extern "C" {
    #include <libusb-1.0/libusb.h>
    #include <jpeglib.h>
    #include <jerror.h>
}


namespace am7x01 {

    #define     PROJECTOR_WIDTH     800
    #define     PROJECTOR_HEIGHT    480

    /*
     *  For pointers, the rule is: allocator frees his allocations
     */
    struct Projector {
            Transformer *transformer;

            /*  For the whole screen set width and height OR window to 0
             *  if scale = true, height and width will be the dimension to be scaled to
             *  otherwise, it is the size of the source
             */
            Projector (const Power power = HIGH, Transformer* t = NULL);

            ~Projector ();

            void update ();
            void setPower (const Power);

            /*  Projector doesn't free IScreenshot after use */
            void assign (IScreenshot *);

        private:
            unsigned char* buffer;
            uint64_t       bufferSize;

            libusb_device_handle *dev;
            IScreenshot *shooter;

            dataHeader     header;

            void send(const void *buffer, const unsigned int len);

            /*  The function suppose that src has been allocated following (width * height * bpp);
             *  compress doesn't free src and dst; src must be at 3 bytes per pixel in RGB.
             */
            uint64_t compress (const Image&);
    };

}

#endif

