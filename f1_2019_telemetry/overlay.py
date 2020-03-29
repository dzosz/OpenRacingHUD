#!/usr/bin/env python

import ctypes as c
import os
import platform
import time

class Overlay:
    def __init__(self):
        prefix, ext = ('', '.dll') if platform.system() == 'Windows' else ('lib', '.so')
        lib_file_name = "{}overlay{}".format(prefix, ext)
        lib_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), lib_file_name)
        self.lib = c.cdll.LoadLibrary(lib_path)

        self.lib.overlay_start.argtypes=None
        self.lib.overlay_start.restype=None

        self.lib.overlay_update_wheel_slip.argtypes=[c.c_double, c.c_double, c.c_double, c.c_double]
        self.lib.overlay_update_wheel_slip.restype=None

        self._start()

    def updateWheelSlip(self, rl, rr, fl, fr):
        self.lib.overlay_update_wheel_slip(rl, rr, fl, fr)

    def _start(self):
         self.lib.overlay_start()


if __name__ == '__main__':
    overlay = Overlay()
    a = 0

    while True:
        a += 0.01
        wheelSlip = [(a)%1, (a+0.01)%0.1, (a+0.0333)%0.3, (a+0.0431531)%0.06]

        overlay.updateWheelSlip(*wheelSlip)

        time.sleep(1) # only for example
