#include <c_interface.h>

#include "helpers.h"
#include "wheelslipwindow.hpp"

#include <QGuiApplication>
#include <thread>

#ifdef __cplusplus
extern "C"
{
#endif
    int start(int argc, char* argv[]);
#ifdef __cplusplus
}
#endif

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
void f1_overlay_update_slip(double rl, double rr, double fl, double fr)
{
    window->updateSlip(getPercentageWheelSlip(rl), getPercentageWheelSlip(rr),
                       getPercentageWheelSlip(fl), getPercentageWheelSlip(fr));
}
