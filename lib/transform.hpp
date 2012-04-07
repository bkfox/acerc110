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

