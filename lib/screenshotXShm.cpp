#include "screenshotXShm.hpp"
#include "projector.hpp"

#include <stdexcept>

extern "C" {
    #include <sys/shm.h>
    #include <X11/Xutil.h>

    #include <jpeglib.h>
    #include <jerror.h>
}

namespace am7x01 {
    using namespace std;

    ScreenshotXShm::ScreenshotXShm (Projector *Parent) :
        display(NULL) {
        parent = Parent;

        display = XOpenDisplay(0);
        if(!display)
            throw runtime_error("XOpenDisplay(0) failed");

        if (!XShmQueryExtension(display))
            throw runtime_error("XShmExtension is not available");

        if(!parent->window)
            parent->window = RootWindow(display, DefaultScreen(display));
        //for the moment, now way to scale
        /* if(!parent->width)
            parent->width = DisplayWidth(display, DefaultScreen(display));
        if(!parent->height)
            parent->height = DisplayHeight(display, DefaultScreen(display)); */


        image = XShmCreateImage(display, DefaultVisual(display, 0), 32, ZPixmap, NULL,
                    &shm, parent->width, parent->height);
        parent->setBpp(image->bits_per_pixel / 8, JCS_EXT_BGRX);

        if(!image)
            throw runtime_error("XShmCreateImage() failed");

        shm.shmid = shmget(IPC_PRIVATE, image->bytes_per_line * image->height, IPC_CREAT | 0777);
        if(shm.shmid < 0)
            throw runtime_error("shmget() failed");

        shm.shmaddr = image->data = (char *) shmat(shm.shmid, 0, 0);
        if (shm.shmaddr == (char *) -1)
            throw runtime_error("shmat() failed");

        shm.readOnly = False;
        XShmAttach(display, &shm);
    }


    ScreenshotXShm::~ScreenshotXShm () {
        XShmDetach(display, &shm);
        shmdt(shm.shmaddr);

        if(image)
            XDestroyImage(image);
        if(display)
            XCloseDisplay(display);
    }


    unsigned char* ScreenshotXShm::update () {
        XShmGetImage(display, parent->window, image, 0, 0, AllPlanes);
        return (unsigned char*) image->data;
    }
}


