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
#include "transform.hpp"

namespace am7x01 {

Transformer::Transformer () : next(0) {
}


void Transformer::append(Transformer* t) {
    if(next)
        next->append(t);
    next = t;
}


//--------------------------------------------------------------------------
Scale::Scale (uint32_t w, uint32_t h) :
    outW{w}, outH{h} {
    buffer = new unsigned char[w*h*4];
}


Scale::~Scale () {
    if(buffer)
        delete[] buffer;
}


/*  Use the Nearest Neighbour algorithm
 */
Image Scale::transform (const Image& src) {
    if(src.width == outW && src.height == outH)
        return transformer_next(src);

    // working var
    double dx = (double) src.width / outW,
           dy = (double) src.height / outH;
    int y, x, v, line;
    int table[outW];
    unsigned char *offset = buffer;

    for(x = 0; x < outW; x++)
        table[x] = (int) ((double)dx * x) * src.channels;

    for(y = 0; y < outH; y++) {
        line = (int)((double)dy * y) * src.bpl;
        for(x = 0; x < outW; x++) {
            v = line + table[x];
            *offset = src.data[v];
            *(offset+1) = src.data[v+1];
            *(offset+2) = src.data[v+2];
            offset+=src.channels;
        }
    }

    Image image = src;
    image.width = outW;
    image.height = outH;
    image.bpl = outW * image.channels;
    image.size = outH * image.bpl;
    image.data = buffer;
    return image;
}

}

