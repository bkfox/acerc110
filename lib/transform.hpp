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
#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include    "screenshot.hpp"

namespace am7x01 {

#define transformer_next(x)   ((next) ? next->transform(x) : x)

struct  Transformer {
    Transformer();
    void append(Transformer*);

    virtual Image transform (const Image& src) = 0;

    Transformer *next;
};


struct  Scale : Transformer {
    ~Scale ();

    Scale (uint32_t w, uint32_t h);
    Image transform (const Image& src);

    private:
        unsigned char  *buffer;
        uint32_t        outW,
                        outH;
};

} //namespace

#endif

