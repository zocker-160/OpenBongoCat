#include <iostream>
#include <cstring>

#include <QDebug>


#include <X11/extensions/XInput2.h>

#include "keylogger.h"


GlobalKeylogger::GlobalKeylogger(QObject* parent, bool logMouseClicks) : QThread(parent) {
    display = XOpenDisplay(nullptr);
    if (!display) {
        qDebug() << "Failed to open X display";
        initFailed = true;
        return;
    }

    int event, error;
    if (!XQueryExtension(display, "XInputExtension", &xi_opcode, &event, &error)) {
        qDebug() << "XInput2 not available";
        initFailed = true;
        return;
    }

    int major = 2, minor = 0;
    if (XIQueryVersion(display, &major, &minor) != Success) {
        qDebug() << "XInput2 not supported (need at least version 2.0)";
        initFailed = true;
        return;
    }

    Window root = DefaultRootWindow(display);

    unsigned char mask[(XI_LASTEVENT + 7)/8] = {0};
    memset(mask, 0, sizeof(mask));
    
    XISetMask(mask, XI_RawKeyPress);
    if (logMouseClicks) {
        XISetMask(mask, XI_RawButtonPress);
    }
    
    XIEventMask evmask;
    evmask.deviceid = XIAllMasterDevices;
    evmask.mask_len = sizeof(mask);
    evmask.mask = mask;

    XISelectEvents(display, root, &evmask, 1);
    XFlush(display);
}


GlobalKeylogger::~GlobalKeylogger() {
    XCloseDisplay(display);
}

bool GlobalKeylogger::hasFailed() {
    return initFailed;
}

void GlobalKeylogger::run() {
    qDebug() << "Listening for global key presses using XInput2...";

    while (!shouldStop) {
        if (!XPending(display)) {
            msleep(100);
            continue;
        }

        XEvent ev;
        XNextEvent(display, &ev);

        if (ev.xcookie.type == GenericEvent && ev.xcookie.extension == xi_opcode) {
            if (XGetEventData(display, &ev.xcookie)) {
                qDebug() << "XEvent: " << ev.xcookie.evtype;

                if (ev.xcookie.evtype == XI_RawKeyPress) {
                    qDebug() << "key pressed";
                    emit keyPressed();
                }
                else if (ev.xcookie.evtype == XI_RawButtonPress) {
                    XIRawEvent* raw = static_cast<XIRawEvent*>(ev.xcookie.data);
                    qDebug() << "Mouse pressed: " << raw->detail;

                    int btn = raw->detail;
                    //emit mousePressed(btn);
                    emit mousePressed();
                }

                XFreeEventData(display, &ev.xcookie);
            }
        }
    }
}

void GlobalKeylogger::stop() {
    shouldStop = true;
}
