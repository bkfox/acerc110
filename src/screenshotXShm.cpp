/*
Copyright (C) 2012 - bkfox

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
#include "pimpl_.hpp"
#include "screenshotXShm.hpp"
#include "projector.hpp"

#include <string>
#include <stdexcept>

#include <iostream>
#define B std::cout << __LINE__ << " " << __FUNCTION__ << std::endl;

extern "C" {
    #include <sys/shm.h>

    #include <xcb/xcb.h>
    #include <xcb/xproto.h>
    #include <xcb/xcb_image.h>
    #include <xcb/shm.h>
}


#ifndef XC_crosshair
    #define XC_crosshair 34
#endif


namespace am7x01 {
using namespace std;


struct ScreenshotXShm::impl {
    xcb_connection_t*       c;
    xcb_window_t            w;
    xcb_image_t*            xi;
    Image                   im;
    xcb_shm_segment_info_t  shm;

    uint32_t pick(uint32_t);
} ;


uint32_t ScreenshotXShm::impl::pick(uint32_t root) {
    cout << "Please click on a window to project it" << endl;

    xcb_cursor_t cursor;
    xcb_font_t   cursorFont;

    cursorFont = xcb_generate_id(c);
    xcb_open_font(c, cursorFont, 6, "cursor");

    cursor = xcb_generate_id(c);
    xcb_create_glyph_cursor(c, cursor, cursorFont, cursorFont,
            XC_crosshair, XC_crosshair + 1, 0, 0, 0, 0xffff, 0xffff, 0xffff);

    auto cookie = xcb_grab_pointer(c, false, root,
            XCB_EVENT_MASK_BUTTON_RELEASE,
            XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC,
            XCB_WINDOW_NONE, cursor, XCB_CURRENT_TIME);
    auto reply = xcb_grab_pointer_reply(c, cookie, NULL);

    uint32_t w = 0;

    if(reply) {
        xcb_generic_event_t * e;
        xcb_button_press_event_t * e_;

        while(1) {
            e = xcb_wait_for_event(c);
            if(e && (e->response_type & ~0x80) == XCB_BUTTON_RELEASE) {
                e_ = (xcb_button_press_event_t*)e;
                w = e_->child;
                break;
            }
        }

        delete reply;
        xcb_ungrab_pointer(c, XCB_CURRENT_TIME);
    }
    else
        throw runtime_error("Can't grab pointer");

    xcb_free_cursor(c, cursor);
    xcb_close_font(c, cursorFont);
    return w;
}


//------------------------------------------------------------------------------
ScreenshotXShm::ScreenshotXShm (int pW, int pH , uint32_t w, bool pick) {
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
        m->c = c;
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


        if(pick)
            w = m->pick(s->root);

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


