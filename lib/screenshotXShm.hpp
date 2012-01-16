#ifndef     SCREENSHOT_XSHM_HPP
#define     SCREENSHOT_XSHM_HPP

#include "screenshot.hpp"

#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>

namespace am7x01 {

    struct ScreenshotXShm : IScreenshot {
            // window, panning width and height
            // if panning < 0, set it at the projector capabilities
            ScreenshotXShm (uint32_t = 0, int = -1, int = -1);
            ~ScreenshotXShm ();

            virtual Image update ();

        private:
            Display *display;
            XImage  *xImage;
            Image    image;
            uint32_t win;

            XShmSegmentInfo shm;
    };
}

#endif

