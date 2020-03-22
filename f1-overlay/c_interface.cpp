#include <c_interface.h>

#include "wheelslipwindow.hpp"

#include <QGuiApplication>
#include <thread>

extern int              start(int argc, char* argv[]);
extern WheelSlipWindow* window;

void f1_overlay_start()
{
    static bool initiated = false;
    if (!initiated)
    {
        initiated = true;
        std::thread background{[&] { start(0, 0); }};
        background.detach();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));  // works every time!
    }
}
void f1_overlay_update_slip(int rl, int rr, int fl, int fr)
{
    window->updateSlip(rl, rr, fl, fr);
}
