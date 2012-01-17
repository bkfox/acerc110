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


    Image Scale::transform (const Image& src) {
        //dest[dx,dy] = src[dx*src_width/dest_width + dy*src_height/dest_height * bpl]
        //
        //  d[x,y] = [x + y * bpl]
        //
        if(src.width <= outW || src.height <= outH)
            return transformer_next(src);

        double dx = (double) src.width / outW,
               dy = (double) src.height / outH,
               co;

        int y = outH, x,
            v, line;

        unsigned char * offset = buffer;

        for(int y = 0; y < outH; y++) {
            co = 0;
            line = (int)((double)dy * y) * src.bpl;
            for(int x = 0; x < outW; x++) {
                v = line + (int) ((double)dx * x) * src.channels;
                *offset = src.data[v];
                *(offset+1) = src.data[v+1];
                *(offset+2) = src.data[v+2];
                offset+=src.channels;
                co += dx;
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

