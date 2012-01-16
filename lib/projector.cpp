#include "am7xxx.hpp"
#include "projector.hpp"

#include <stdexcept>

namespace am7x01 {
    using namespace std;

    #define BUFFER_SIZE     AM7X01_MAX_SIZE*2

    Projector::Projector (const Power power, const bool Scale, const int Width,
        const int Height, const uint32_t Window):
        scale(Scale), window(Window),
        width(Width), height(Height), resolution(Width*Height),
        buffer(0), bufferSize(0), dev(0), shooter(0),
        iHeader(htole32(IMAGE), sizeof(imageHeader), 0, 0x3e, 0x10) {

        setBpp(3);

        // usb
        libusb_init(NULL);

        dev = libusb_open_device_with_vid_pid(NULL, AM7X01_VENDOR_ID, AM7X01_PRODUCT_ID);

        if(!dev)
            throw runtime_error("Cannot connect to usb device");

        libusb_set_configuration(dev, 1);
        libusb_claim_interface(dev, 0);

        // header
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

        if(buffer)
            delete[] buffer;
    }


    void Projector::assign (IScreenshot *s) {
        if(shooter)
            throw runtime_error("A IScreenshot interface has been yet assigned");

        shooter = s;
    }


    void Projector::setBpp (int b, J_COLOR_SPACE c) {
        bpp = b;
        color = c;
        bpl = width * bpp;
    }


    void Projector::update () {
        unsigned char *data;

        data = shooter->update();
        compress(data);

        iHeader.sub.image.size = htole32(bufferSize);
        if(bufferSize > AM7X01_MAX_SIZE)
            throw runtime_error("JPEG file size is too big");

        send(&iHeader, sizeof(iHeader));
        send(buffer, bufferSize);
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
        struct jpeg_compress_struct cinfo;
        struct jpeg_error_mgr jerr;

        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_compress(&cinfo);

        cinfo.image_width = width;
        cinfo.image_height = height;
        cinfo.input_components = bpp;
        cinfo.in_color_space = color;

        jpeg_mem_dest(&cinfo, &buffer, &bufferSize);

        jpeg_set_defaults(&cinfo);
        jpeg_start_compress(&cinfo, true);

        int n = height;
        while(n--) {
            jpeg_write_scanlines(&cinfo, &src, 1);
            src += bpl;
        }

        jpeg_finish_compress(&cinfo);
        jpeg_destroy_compress(&cinfo);
    }

}

