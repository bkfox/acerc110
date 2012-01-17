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

    ScreenshotXShm::ScreenshotXShm (int panW, int panH, uint32_t window) {
        display = XOpenDisplay(0);
        if(!display)
            throw runtime_error("XOpenDisplay(0) failed");

        if (!XShmQueryExtension(display))
            throw runtime_error("XShmExtension is not available");

        /* Configurations */
        if(window)
            throw runtime_error("Not supported for the moment");

        win = RootWindow(display, DefaultScreen(display));

        if(!panW)
            panW = DisplayWidth(display, DefaultScreen(display));
        else if(panW < 0)
            panW = PROJECTOR_WIDTH;

        if(!panH)
            panH = DisplayHeight(display, DefaultScreen(display));
        else if(panH < 0)
            panH = PROJECTOR_HEIGHT;

        /* Init XImage */
        printf("%d %d \n", panW, panH);
        xImage = XShmCreateImage(display, DefaultVisual(display, 0), 32, ZPixmap, NULL,
                    &shm, panW, panH);

        if(!xImage)
            throw runtime_error("XShmCreatexImage() failed");

        /* Init image struct */
        image.width = xImage->width;
        image.height = xImage->height;
        image.bpl = xImage->bytes_per_line;
        image.channels = 4;
        image.color = JCS_EXT_BGRX;
        image.size = image.bpl * image.height;

        /* Shared memory */
        shm.shmid = shmget(IPC_PRIVATE, xImage->bytes_per_line * xImage->height, IPC_CREAT | 0777);
        if(shm.shmid < 0)
            throw runtime_error("shmget() failed");

        shm.shmaddr = xImage->data = (char *) shmat(shm.shmid, 0, 0);
        image.data = (unsigned char *) xImage->data;
        if (shm.shmaddr == (char *) -1)
            throw runtime_error("shmat() failed");

        shm.readOnly = False;
        XShmAttach(display, &shm);
    }


    ScreenshotXShm::~ScreenshotXShm () {
        XShmDetach(display, &shm);
        shmdt(shm.shmaddr);

        if(xImage)
            XDestroyImage(xImage);
        if(display)
            XCloseDisplay(display);
    }


    Image ScreenshotXShm::update () {
        XShmGetImage(display, win, xImage, 0, 0, AllPlanes);
        return image;
    }
}


