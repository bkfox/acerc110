#include "am7xxx.hpp"
#include "projector.hpp"

#include <stdexcept>

namespace am7x01 {
    using namespace std;

    #define BUFFER_SIZE     AM7X01_MAX_SIZE*2

    Projector::Projector (const Power power, Transformer *t):
        transformer(t),
        buffer(0), bufferSize(0), dev(0), shooter(0),
        iHeader(htole32(IMAGE), sizeof(imageHeader), 0, 0x3e, 0x10) {

        /*  We need to alloc buffer at first, otherwise looks like there is a
         *  memory leak in libjpeg-turbo with jpeg_dest_mem
         */
        buffer = new unsigned char [BUFFER_SIZE];

        // usb
        libusb_init(NULL);

        dev = libusb_open_device_with_vid_pid(NULL, AM7X01_VENDOR_ID, AM7X01_PRODUCT_ID);

        if(!dev)
            throw runtime_error("Cannot connect to usb device");

        libusb_set_configuration(dev, 1);
        libusb_claim_interface(dev, 0);

        // header
        iHeader.sub.image.format = htole32(0x01);
        iHeader.sub.image.width =  htole32(PROJECTOR_WIDTH);
        iHeader.sub.image.height = htole32(PROJECTOR_HEIGHT);

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
            shooter->parent = NULL;

        shooter = s;
        shooter->parent = this;
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


    void Projector::update () {
        Image img = shooter->update();

        if(transformer)
            img = transformer->transform(img);

        compress(img);

        iHeader.sub.image.size = htole32(compressedSize);
        if(compressedSize > AM7X01_MAX_SIZE)
            throw runtime_error("JPEG file size is too big");

        send(&iHeader, sizeof(iHeader));
        send(buffer, compressedSize);
    }


    void Projector::compress (const Image& src ) {
        struct jpeg_compress_struct cinfo;
        struct jpeg_error_mgr jerr;

        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_compress(&cinfo);

        cinfo.image_width = src.width;
        cinfo.image_height = src.height;
        cinfo.input_components = src.channels;
        cinfo.in_color_space = src.color;

        compressedSize = bufferSize;
        jpeg_mem_dest(&cinfo, &buffer, &compressedSize);

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

        if(compressedSize > bufferSize)
            bufferSize = compressedSize;
    }

}

