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
#include "am7xxx.hpp"
#include "projector.hpp"


#include <iostream>
#include <time.h>

#include <stdexcept>

namespace am7x01 {
using namespace std;

Projector::Projector (const Power power, const Zoom zoom, Transformer *t):
    transformer(t),
    buffer(0), bufferSize(0), dev(0), shooter(0),
    header(htole32(IMAGE), sizeof(imageHeader), 0) {

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
    setZoom(zoom);
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


void Projector::setZoom (const Zoom zoom) {
    static dataHeader data(htole32(ZOOM), sizeof(zoomHeader), 0);

    switch(zoom) {
        case NONE:          data.sub.zoom = {0, 0}; break;
        case HORIZONTAL:    data.sub.zoom = {0, 1}; break;
        case BOTH:          data.sub.zoom = {1, 0}; break;
        case TEST:          data.sub.zoom = {1, 1}; break;
    }

    send(&data, sizeof(data));
}


void Projector::setPower (const Power power) {
    static dataHeader data(htole32(POWER), sizeof(powerHeader), 0);

    switch(power) {
        case OFF:   data.sub.power = { 0, 0, 0 };   break;
        case LOW:   data.sub.power = { 0, 0, 1 };   break;
        case MID:   data.sub.power = { 0, 1, 0 };   break;
        case HIGH:  data.sub.power = { 0, 1, 1 };   break;
        case TURBO: data.sub.power = { 1, 0, 0 };   break;
    }

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

    header.sub.image.format = 0x01;
    header.sub.image.size = htole32(compress(img));
    if(header.sub.image.size > AM7X01_MAX_SIZE)
        throw runtime_error("JPEG file size is too big");

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
    cinfo.in_color_space = JCS_EXT_BGRX; //src.color;

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

