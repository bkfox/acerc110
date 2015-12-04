/*
Copyright (C) 2012 - bkfox

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
#ifndef     AM7X01_HPP
#define     AM7X01_HPP

extern "C" {
    #include <stdint.h>     //uintXX_t
    #include <endian.h>
}

#define     AM7X01_VENDOR_ID       0x1de1
#define     AM7X01_PRODUCT_ID      0xc101

#define     AM7X01_MAX_SIZE        250000

namespace am7x01 {

    enum Power {
        OFF     = 0,
        LOW     = 1,
        MID     = 2,
        HIGH    = 3,
        TURBO   = 4,
    };


    enum Zoom {
        NONE        = 0,
        HORIZONTAL  = 1,
        BOTH        = 2,
        TEST        = 4,
    };

    enum PacketType {
        INIT    = 1,
        IMAGE   = 2,
        POWER   = 4,
        ZOOM    = 5,
    };


    struct imageHeader {
        uint32_t format;
        uint32_t width;
        uint32_t height;
        uint32_t size;
    };

    struct powerHeader {
        uint32_t low;
        uint32_t mid;
        uint32_t high;
    };

    struct zoomHeader {
        uint32_t low;
        uint32_t mid;
    };

    struct dataHeader {
        uint32_t    type;
        uint8_t     direction;
        uint8_t     len;
        uint8_t     padding1;
        uint8_t     padding2;

        union {
            imageHeader image;
            powerHeader power;
            zoomHeader  zoom;
        } sub;

        dataHeader (uint32_t t, uint8_t l = 0, uint8_t d = 0) :
            type(t), direction(d), len(l) {
        }
    };
}

#endif

