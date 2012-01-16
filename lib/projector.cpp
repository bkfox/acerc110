#include "am7xxx.hpp"
#include "projector.hpp"

//#define BP  std::cout << __FILE__ << " " << __LINE__ << std::endl;
//#include <iostream>

#include <stdexcept>

extern "C" {
   #include <jpeglib.h>
   #include <jerror.h>
}

namespace am7x01 {
    using namespace std;
    using namespace Magick;

    #define BUFFER_SIZE     AM7X01_MAX_SIZE*2

    Projector::Projector (const Power power, const bool Scale, const int Width,
        const int Height, const uint32_t Window):
        scale(Scale), window(Window),
        width(Width), height(Height), bpp(3), bpl(Width*bpp), size(bpl*Height),
        resolution(Width*Height),
        dev(0), shooter(0),
        iHeader(htole32(IMAGE), sizeof(imageHeader), 0, 0x3e, 0x10) {

        libusb_init(NULL);

        dev = libusb_open_device_with_vid_pid(NULL, AM7X01_VENDOR_ID, AM7X01_PRODUCT_ID);

        if(!dev)
            throw runtime_error("Cannot connect to usb device");

        libusb_set_configuration(dev, 1);
        libusb_claim_interface(dev, 0);


        image = Image(Geometry(width, height), "White");


        iHeader.sub.image.format = htole32(0x01);
        iHeader.sub.image.width =  htole32(width);
        iHeader.sub.image.height = htole32(height);

        // init device
        header h(INIT);
        send(&h, sizeof(h));

        setPower(power);
    }


    Projector::~Projector () {
        if(dev) {
            setPower(OFF);      //first, shutdown the projector
            libusb_close(dev);  //then close the device
        }
    }


    void Projector::assign (IScreenshot *s) {
        if(shooter)
            throw runtime_error("A IScreenshot interface has been yet assigned");

        shooter = s;
    }

/*
        static inline unsigned int in_80chars(unsigned int i)
        {
            return ((i+1) % (80/3));
        }

        static void dump(uint8_t *buffer, unsigned int len)
        {
            unsigned int i;

            if (buffer == NULL || len == 0)
                return;

            for (i = 0; i < len; i++) {
                printf("%02hhX%c", buffer[i], (in_80chars(i) && (i < len - 1)) ? ' ' : '\n');
            }
            fflush(stdout);
        }
*/

    void Projector::update () {
        unsigned char *data;

        data = shooter->update();
        compress(data);

        int size = blob.length();
        iHeader.sub.image.size = htole32(size);
        if(size > AM7X01_MAX_SIZE)
            throw runtime_error("JPEG file size is too big");

        send(&iHeader, sizeof(iHeader));
        send(blob.data(), size);
    }


    void Projector::setPower (const Power power) {
        static header data(htole32(POWER), sizeof(powerHeader), 0, 0xff, 0xff);

        data.sub.power.low = 0;
        data.sub.power.mid = (power & (LOW | HIGH)) ? 1 : 0;
        data.sub.power.high = (power & (MID | HIGH)) ? 1 : 0;

        send(&data, sizeof(data));
    }


    void Projector::send (const void *buffer, const unsigned int len) {
        int l;

        if(libusb_bulk_transfer(dev, 1, (unsigned char*)buffer, len, &l, 0) || l != len)
            throw runtime_error("Cannot correctly send data through usb");
    }


    void Projector::compress (unsigned char *src) {
        image.modifyImage();
        PixelPacket *pixel = image.getPixels(0, 0, width, height);

        unsigned char *c = src;
        int n = resolution;
        uint32_t v = 0;

        /*
         *  At this point, we suppose that the src has the dimension [width,height]
         *  (then same resolution), and that the image is encoded as RGB(A)
         */
        while(n--) {
            c += bpp;
            v = *((uint32_t*) c);

            pixel->red = (v >> 8) & 0xffff;
            pixel->green = (v) & 0xffff;
            pixel->blue = (v << 8) & 0xffff;
            pixel++;
        }

        image.syncPixels();
        image.magick("JPEG");
        image.write(&blob);
    }

}

