#ifndef     SCREENSHOT_XSHM_HPP
#define     SCREENSHOT_XSHM_HPP

#include "pimpl.hpp"
#include "screenshot.hpp"

namespace am7x01 {

class ScreenshotXShm : public IScreenshot {
        struct impl;
        Pimpl<impl> m;
    public:
        /**
         *  If panning < 0, set the width at the projector capabilities
         *  If panning == 0, set the width at the window/screen size
         *  If windowID == 0, use the fullscreen
         *  If pick == true, user select window using mouse
         */
        ScreenshotXShm (int panningWidth = -1, int panningHeight = -1, uint32_t windowID = 0,
                bool pick = false);
        ~ScreenshotXShm ();

        /**
         *  Update image data
         */
        virtual Image update ();
};

}

#endif

