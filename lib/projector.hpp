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
#ifndef     PROJECTOR_HPP
#define     PROJECTOR_HPP

extern "C" {
    #include <libusb-1.0/libusb.h>
}


#include "screenshot.hpp"
#include "transform.hpp"


namespace am7x01 {

#define     PROJECTOR_WIDTH     800
#define     PROJECTOR_HEIGHT    480

//#define     PROJECTOR_WIDTH     1280
//#define     PROJECTOR_HEIGHT    800

/*
 *  For pointers, the rule is: allocator frees his allocations
 */
struct Projector {
        Transformer *transformer;

        /*  For the whole screen set width and height OR window to 0
         *  if scale = true, height and width will be the dimension to be scaled to
         *  otherwise, it is the size of the source
         */
        Projector (const Power power = HIGH, const Zoom zoom = BOTH, Transformer* t = NULL);

        ~Projector ();

        void update ();
        void setPower (const Power);
        void setZoom (const Zoom zoom);

        /*  Projector doesn't free IScreenshot after use */
        void assign (IScreenshot *);

    private:
        unsigned char* buffer;
        uint64_t       bufferSize;

        libusb_device_handle *dev;
        IScreenshot *shooter;

        dataHeader     header;

        void send(const void *buffer, const unsigned int len);

#ifdef USE_JPEG
        /*  The function suppose that src has been allocated following (width * height * bpp);
         *  compress doesn't free src and dst; src must be at 3 bytes per pixel in RGB.
         */
        uint64_t compress (const Image&);
#endif
};

}

#endif

