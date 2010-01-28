/*
 * Copyright (c) 2008-2009 NVIDIA, Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <dlfcn.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vdpau/vdpau_x11.h>
#if DRI2
#include "mesa_dri2.h"
#include <X11/Xlib.h>
#endif

typedef void SetDllHandle(
    void * driver_dll_handle
);

#if DEBUG

static void _vdp_wrapper_error_breakpoint(char const * file, int line, char const * function)
{
    fprintf(stderr, "VDPAU wrapper: Error detected at %s:%d %s()\n", file, line, function);
}

#define _VDP_ERROR_BREAKPOINT() _vdp_wrapper_error_breakpoint(__FILE__, __LINE__, __FUNCTION__)

#else

#define _VDP_ERROR_BREAKPOINT()

#endif

#define DRIVER_LIB_FORMAT "%slibvdpau_%s.so%s"

static char * _vdp_get_driver_name_from_dri2(
    Display *             display,
    int                   screen
)
{
    char * driver_name = NULL;
#if DRI2
    Window root = RootWindow(display, screen);
    int event_base, error_base;
    int major, minor;
    char * device_name;

    if (!_vdp_DRI2QueryExtension(display, &event_base, &error_base)) {
        return NULL;
    }

    if (!_vdp_DRI2QueryVersion(display, &major, &minor) ||
            (major < 1 || (major == 1 && minor < 2))) {
        return NULL;
    }

    if (!_vdp_DRI2Connect(display, root, &driver_name, &device_name)) {
        return NULL;
    }

    XFree(device_name);
#endif /* DRI2 */
    return driver_name;
}

VdpStatus vdp_device_create_x11(
    Display *             display,
    int                   screen,
    /* output parameters follow */
    VdpDevice *           device,
    VdpGetProcAddress * * get_proc_address
)
{
    char const * vdpau_driver;
    char * vdpau_driver_dri2 = NULL;
    char         vdpau_driver_lib[PATH_MAX];
    void *       backend_dll;
    char const * vdpau_trace;
    char const * func_name;

    VdpDeviceCreateX11 * vdp_imp_device_create_x11;

    vdpau_driver = getenv("VDPAU_DRIVER");
    if (!vdpau_driver) {
        vdpau_driver = vdpau_driver_dri2 =
            _vdp_get_driver_name_from_dri2(display, screen);
    }
    if (!vdpau_driver) {
        vdpau_driver = "nvidia";
    }

    if (snprintf(vdpau_driver_lib, sizeof(vdpau_driver_lib), DRIVER_LIB_FORMAT,
                 VDPAU_MODULEDIR "/", vdpau_driver, ".1") >=
            sizeof(vdpau_driver_lib)) {
        fprintf(stderr, "Failed to construct driver path: path too long\n");
        if (vdpau_driver_dri2) {
            XFree(vdpau_driver_dri2);
            vdpau_driver_dri2 = NULL;
        }
        _VDP_ERROR_BREAKPOINT();
        return VDP_STATUS_NO_IMPLEMENTATION;
    }

    backend_dll = dlopen(vdpau_driver_lib, RTLD_NOW | RTLD_GLOBAL);
    if (!backend_dll) {
        /* Try again using the old path, which is guaranteed to fit in PATH_MAX
         * if the complete path fit above. */
        snprintf(vdpau_driver_lib, sizeof(vdpau_driver_lib), DRIVER_LIB_FORMAT,
                 "", vdpau_driver, "");
        backend_dll = dlopen(vdpau_driver_lib, RTLD_NOW | RTLD_GLOBAL);
    }

    if (vdpau_driver_dri2) {
        XFree(vdpau_driver_dri2);
        vdpau_driver_dri2 = NULL;
    }

    if (!backend_dll) {
        fprintf(stderr, "Failed to open VDPAU backend %s\n", dlerror());
        _VDP_ERROR_BREAKPOINT();
        return VDP_STATUS_NO_IMPLEMENTATION;
    }

    vdpau_trace = getenv("VDPAU_TRACE");
    if (vdpau_trace && atoi(vdpau_trace)) {
        void *         trace_dll;
        SetDllHandle * set_dll_handle;

        trace_dll = dlopen(VDPAU_MODULEDIR "/libvdpau_trace.so.1", RTLD_NOW | RTLD_GLOBAL);
        if (!trace_dll) {
            fprintf(stderr, "Failed to open VDPAU trace library %s\n", dlerror());
            _VDP_ERROR_BREAKPOINT();
            return VDP_STATUS_NO_IMPLEMENTATION;
        }

        set_dll_handle = (SetDllHandle*)dlsym(
            trace_dll,
            "vdp_trace_set_backend_handle"
        );
        if (!set_dll_handle) {
            fprintf(stderr, "%s\n", dlerror());
            _VDP_ERROR_BREAKPOINT();
            return VDP_STATUS_NO_IMPLEMENTATION;
        }

        set_dll_handle(backend_dll);

        backend_dll = trace_dll;

        func_name = "vdp_trace_device_create_x11";
    }
    else {
        func_name = "vdp_imp_device_create_x11";
    }

    vdp_imp_device_create_x11 = (VdpDeviceCreateX11*)dlsym(
        backend_dll,
        func_name
    );
    if (!vdp_imp_device_create_x11) {
        fprintf(stderr, "%s\n", dlerror());
        _VDP_ERROR_BREAKPOINT();
        return VDP_STATUS_NO_IMPLEMENTATION;
    }

    return vdp_imp_device_create_x11(
        display,
        screen,
        device,
        get_proc_address
    );
}
