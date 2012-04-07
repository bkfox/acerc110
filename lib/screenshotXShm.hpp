#ifndef     SCREENSHOT_XSHM_HPP
#define     SCREENSHOT_XSHM_HPP

#include "screenshot.hpp"

#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>

namespace am7x01 {

struct ScreenshotXShm : IScreenshot {
        // panning width and height, window
        // if panning < 0, set it at the projector capabilities
        // if panning = 0, fullcreen
        ScreenshotXShm (int = -1, int = -1, uint32_t = 0);
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

