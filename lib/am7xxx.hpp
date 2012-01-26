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
    };

    enum PacketType {
        INIT    = 1,
        IMAGE   = 2,
        POWER   = 4,
        UNKNOWN = 5,
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

    struct dataHeader {
        uint32_t    type;
        uint8_t     unknown0;
        uint8_t     len;
        uint8_t     unknown1;
        uint8_t     unknown2;

        union {
            imageHeader image;
            powerHeader power;
        } sub;

        dataHeader (uint32_t t, uint8_t l = 0, uint8_t u0 = 0, uint8_t u1 = 0, uint8_t u2 = 0) :
            type(t), unknown0(u0), len(l), unknown1(u1), unknown2(u2) {
        }
    };
}

#endif

