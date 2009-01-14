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

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/vdpau/vdpau_x11.h"

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

#define DRIVER_LIB_FORMAT "libvdpau_%s.so"

VdpStatus vdp_device_create_x11(
    Display *             display,
    int                   screen,
    /* output parameters follow */
    VdpDevice *           device,
    VdpGetProcAddress * * get_proc_address
)
{
    char const * vdpau_driver;
    char *       vdpau_driver_lib;
    void *       backend_dll;
    char const * vdpau_trace;
    char const * func_name;

    VdpDeviceCreateX11 * vdp_imp_device_create_x11;

    /* FIXME: Determine driver name using an X extension */
    vdpau_driver = getenv("VDPAU_DRIVER");
    if (!vdpau_driver) {
        vdpau_driver = "nvidia";
    }

    vdpau_driver_lib = malloc(strlen(DRIVER_LIB_FORMAT) + strlen(vdpau_driver) + 1);
    if (!vdpau_driver_lib) {
        _VDP_ERROR_BREAKPOINT();
        return VDP_STATUS_RESOURCES;
    }
    sprintf(vdpau_driver_lib, DRIVER_LIB_FORMAT, vdpau_driver);

    backend_dll = dlopen(vdpau_driver_lib, RTLD_NOW | RTLD_GLOBAL);
    free(vdpau_driver_lib);
    if (!backend_dll) {
        _VDP_ERROR_BREAKPOINT();
        return VDP_STATUS_NO_IMPLEMENTATION;
    }

    vdpau_trace = getenv("VDPAU_TRACE");
    if (vdpau_trace && atoi(vdpau_trace)) {
        void *         trace_dll;
        SetDllHandle * set_dll_handle;

        trace_dll = dlopen("libvdpau_trace.so", RTLD_NOW | RTLD_GLOBAL);
        if (!trace_dll) {
            _VDP_ERROR_BREAKPOINT();
            return VDP_STATUS_NO_IMPLEMENTATION;
        }

        set_dll_handle = (SetDllHandle*)dlsym(
            trace_dll,
            "vdp_trace_set_backend_handle"
        );
        if (!set_dll_handle) {
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

