# This Python file uses the following encoding: utf-8
import ctypes as c
import os

class F1Overlay:
    def __init__(self):
        # init library
        lib_path = os.path.join(os.path.dirname(os.path.abspath(__file__))) + "/libf1-overlay-lib.so"
        print lib_path
        self.lib = c.cdll.LoadLibrary(lib_path)

        self.lib.f1_overlay_start.argtypes=None
        self.lib.f1_overlay_start.restype=None

        self.lib.f1_overlay_update_slip.argtypes=[c.c_int, c.c_int, c.c_int, c.c_int]
        self.lib.f1_overlay_update_slip.restype=None

        self._start()

    def update(self, rl, rr, fl, fr):
        self.lib.f1_overlay_update_slip(rl, rr, fl, fr)

    def _start(self):
         self.lib.f1_overlay_start()

# if__name__ == "__main__":
#     pass
