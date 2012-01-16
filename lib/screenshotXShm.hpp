#ifndef     SCREENSHOT_XSHM_HPP
#define     SCREENSHOT_XSHM_HPP

#include "screenshot.hpp"

#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>

namespace am7x01 {

    struct ScreenshotXShm : IScreenshot {
            ScreenshotXShm (Projector*);
            ~ScreenshotXShm ();

            virtual unsigned char * update ();

        private:
            Display *display;
            XImage *image;
            XShmSegmentInfo shm;
    };
}

#endif

