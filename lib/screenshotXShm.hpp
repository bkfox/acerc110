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

