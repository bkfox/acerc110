#include "am7xxx.hpp"
#include "projector.hpp"


#include <iostream>
#include <time.h>

#include <stdexcept>

namespace am7x01 {
using namespace std;

Projector::Projector (const Power power, Transformer *t):
    transformer(t),
    buffer(0), bufferSize(0), dev(0), shooter(0),
    header(htole32(IMAGE), sizeof(imageHeader), 0, 0x3e, 0x10) {

    // usb
    libusb_init(NULL);

    dev = libusb_open_device_with_vid_pid(NULL, AM7X01_VENDOR_ID, AM7X01_PRODUCT_ID);

    if(!dev)
        throw runtime_error("Cannot connect to usb device");

    libusb_set_configuration(dev, 1);
    libusb_claim_interface(dev, 0);

    // header
    header.sub.image.format = htole32(0x01);
    header.sub.image.width =  htole32(PROJECTOR_WIDTH);
    header.sub.image.height = htole32(PROJECTOR_HEIGHT);

    // init device
    dataHeader h(INIT);
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
        shooter->parent = NULL;

    shooter = s;
    shooter->parent = this;
}


void Projector::setPower (const Power power) {
    static dataHeader data(htole32(POWER), sizeof(powerHeader), 0, 0xff, 0xff);

    data.sub.power.low = 0;
    data.sub.power.mid = (power & LOW) ? 1 : 0;
    data.sub.power.high = (power & MID) ? 1 : 0;

    send(&data, sizeof(data));
}


void Projector::send (const void *buffer, const unsigned int len) {
    int l;

    if(libusb_bulk_transfer(dev, 1, (unsigned char*)buffer, len, &l, 0) || l != len)
        throw runtime_error("Cannot correctly send data through usb");
}



void Projector::update () {
    Image img = shooter->update();

    if(transformer)
        img = transformer->transform(img);

//#define YUV
#ifdef YUV
    {
        header.sub.image.size = htole32((double) (img.height * img.width * 12/8));

        if(header.sub.image.size > bufferSize) {
            if(buffer)
                delete[] buffer;
            buffer = new unsigned char[header.sub.image.size];
            bufferSize = header.sub.image.size;
            header.sub.image.format = htole32(0x02);
        }

        unsigned char *yPos = buffer,
                      *uvPos = buffer + (img.width * img.height),
                      *src = img.data;

        for (int i = 0; i < img.height; i++)
            for (int j = 0; j < img.width; j++) {
                #define B   *(src)
                #define G   *(src+1)
                #define R   *(src+2)

                *yPos = ((66 * R + 129 * G + 25 * B + 128) >> 8) + 16;

                yPos++;
                if(j%2==0 & i%2==0) {
                    *uvPos =     ((-38 * R - 74 * G + 112 * B + 128) >> 8) + 128;
                    *(uvPos+1) = ((112 * R - 94 * G - 18 * B + 128) >> 8) + 128;
                    uvPos += 2;
                }
            }
    }
#else
    header.sub.image.format = 0x01;
    header.sub.image.size = htole32(compress(img));
    if(header.sub.image.size > AM7X01_MAX_SIZE)
        throw runtime_error("JPEG file size is too big");
#endif

    send(&header, sizeof(header));
    send(buffer, header.sub.image.size);
}


uint64_t Projector::compress (const Image& src ) {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    cinfo.image_width = src.width;
    cinfo.image_height = src.height;
    cinfo.input_components = src.channels;
    cinfo.in_color_space = src.color;

    unsigned char *b = buffer;
    uint64_t size = bufferSize;
    jpeg_mem_dest(&cinfo, &b, &size);

    jpeg_set_defaults(&cinfo);
    jpeg_start_compress(&cinfo, true);

    int n = src.height;
    unsigned char *c = src.data;
    while(n--) {
        jpeg_write_scanlines(&cinfo, &c, 1);
        c += src.bpl;
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    if(b != buffer) {
        delete[] buffer;
        buffer = b;
        bufferSize = size;
    }
    return size;
}

}

