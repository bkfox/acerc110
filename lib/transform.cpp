#include "transform.hpp"

#include <iostream>
#define BKP std::cout << __LINE__ << " in " << __FILE__ << std::endl;

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
        outW(w), outH(h) {
        buffer = new unsigned char[w*h*4];
    }


    Scale::~Scale () {
        if(buffer)
            delete[] buffer;
    }


    /* Note:
     *      big → little : works correctly
     *      little → big : not tested yet
     */
    Image Scale::transform (const Image& src) {
        if(src.width == outW || src.height == outH)
            return transformer_next(src);

        // working var
        double dx = (double) src.width / outW,
               dy = (double) src.height / outH;
        int y, x, v, line;
        int table[outW];
        unsigned char *offset = buffer;

        /*  Using a lookup table; Seems that caching the values in RAM
         *  for next function calls cost more in access to value than
         *  recompute all the data in stack
         */
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

