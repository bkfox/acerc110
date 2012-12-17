#include "pimpl_.hpp"
#include "screenshotXShm.hpp"
#include "projector.hpp"

#include <string>
#include <stdexcept>

#include <iostream>
//#define B std::cout << __LINE__ << " " << __FUNCTION__ << std::endl;

extern "C" {
    #include <sys/shm.h>

    #include <xcb/xcb.h>
    #include <xcb/xproto.h>
    #include <xcb/xcb_image.h>
    #include <xcb/shm.h>
}


namespace am7x01 {
using namespace std;


struct ScreenshotXShm::impl {
    xcb_connection_t*       c;
    xcb_window_t            w;
    xcb_image_t*            xi;
    Image                   im;
    xcb_shm_segment_info_t  shm;
} ;


ScreenshotXShm::ScreenshotXShm (int pW, int pH , uint32_t w) {
    xcb_connection_t* c = 0;
    xcb_screen_t* s = 0;
    xcb_image_t* xi = 0;
    xcb_get_geometry_reply_t* geomR = 0;

    try {
        int screenN;
        const char* dn = getenv("DISPLAY");
        if(!dn)
            throw runtime_error("cannot get $DISPLAY. Is X started?");

        c = xcb_connect(dn, &screenN);
        if(!c)
            throw "cannot connect to X";

        auto setup = xcb_get_setup(c);
        auto it = xcb_setup_roots_iterator(setup);
        for(; it.rem; --screenN, xcb_screen_next(&it))
            if(!screenN) {
                s = it.data;
                break;
            }

        if(!s)
            throw runtime_error("cannot get screen information");

        if(!w) {
            w = s->root;

            if(!pW || pW > s->width_in_pixels)
                pW = s->width_in_pixels;
            else if(pW < 0)
                pW = PROJECTOR_WIDTH;
            if(!pH || pH > s->height_in_pixels)
                pH = s->height_in_pixels;
            else if(pH < 0)
                pH = PROJECTOR_HEIGHT;
        }
        else {
            xcb_generic_error_t *e;

            auto ck = xcb_get_geometry(c, w);
            geomR = xcb_get_geometry_reply(c, ck, &e);

            if(!geomR)
                throw runtime_error("cannot retrieve window information");

            // check panning values
            if(!pW || pW > s->width_in_pixels)
                pW = geomR->width;
            else if(pW < 0)
                pW = PROJECTOR_WIDTH;

            if(!pH || pH > s->height_in_pixels)
                pH = geomR->height;
            else if(pH < 0)
                pH = PROJECTOR_HEIGHT;

            free(geomR);
            geomR = 0;
        }

        //cf http://svn.enlightenment.org/svn/e/tags/ecore-1.1.0/src/lib/ecore_x/xcb/ecore_xcb_image.c
        // ximage
        auto fmt = xcb_setup_pixmap_formats(setup);
        auto fmtEnd = xcb_setup_pixmap_formats_length(setup) + fmt;
        for(; fmt != fmtEnd && fmt->depth != 32; ++fmt);

        xi = xcb_image_create(pW, pH, XCB_IMAGE_FORMAT_Z_PIXMAP,
                fmt->scanline_pad, fmt->depth, fmt->bits_per_pixel,
                setup->bitmap_format_scanline_unit,
                XCB_IMAGE_ORDER_MSB_FIRST, // setup->image_byte_order,
                XCB_IMAGE_ORDER_MSB_FIRST, // setup->bitmap_format_bit_order,
                0, 0, 0);

        // shm memory
        m->shm.shmid = shmget(IPC_PRIVATE, xi->stride * xi->height, IPC_CREAT | 0666);
        if(m->shm.shmid < 0)
            throw runtime_error("shmget() failed");

        m->shm.shmaddr = (unsigned char*) shmat(m->shm.shmid, 0, 0);
        if(m->shm.shmaddr == (unsigned char*) -1)
            throw runtime_error("shmat() failed");

        m->shm.shmseg = xcb_generate_id(c);
        xcb_shm_attach(c, m->shm.shmseg, m->shm.shmid, 0);

        // image
        Image im;
        im.data = m->shm.shmaddr;
        im.width = xi->width;
        im.height = xi->height;
        im.bpl = xi->stride;
        im.channels = 4;
        im.size = im.bpl * im.height;

        // m
        m->c = c;
        m->w = w;
        m->xi = xi;
        m->im = im;
    }
    catch(...) {
        if(c)
            xcb_disconnect(c);
        if(geomR)
            free(geomR);
        if(xi)
            xcb_image_destroy(xi);
        if(m->shm.shmaddr) {
            shmdt(m->shm.shmaddr);
            shmctl(m->shm.shmid, IPC_RMID, 0);
        }

        throw;
    }
}


ScreenshotXShm::~ScreenshotXShm () {
    if(m->c && m->shm.shmaddr) {
        xcb_shm_detach_checked(m->c, m->shm.shmseg);
        shmdt(m->shm.shmaddr);
        shmctl(m->shm.shmid, IPC_RMID, 0);
    }

    if(m->xi)
        xcb_image_destroy(m->xi);
    if(m->c)
        xcb_disconnect(m->c);
}


Image ScreenshotXShm::update () {
    xcb_generic_error_t* e = NULL;

    auto ck = xcb_shm_get_image(m->c, m->w, 0, 0, m->im.width, m->im.height, 0xFFFFFFFF,
        m->xi->format, m->shm.shmseg, 0);
    auto r = xcb_shm_get_image_reply(m->c, ck, &e);

    if(e) {
        cout << "ERROR type " << (int)e->response_type
             << ", code " << (int) e->error_code
             << ", sequence " << (int) e->sequence
             << endl;
    }

    return m->im;
}

}


