/*
Copyright (C) 2012 - Thomas Baquet 

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
#ifndef     SCREENSHOT_HPP
#define     SCREENSHOT_HPP

#include "am7xxx.hpp"

extern "C" {
    #include <stdio.h>
}

namespace am7x01 {

struct Projector; // projector.hpp

struct Image {
    unsigned char*  data;
    uint64_t        size;
    uint32_t        width;
    uint32_t        height;
    char            channels;
    uint32_t        bpl;
};


class IScreenshot {
    public:
        virtual Image update () = 0;
        Projector *parent;
};

}

#endif

