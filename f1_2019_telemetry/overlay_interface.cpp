#include "pluginselectorwindow.hpp"
#include "wheelslipwindow.hpp"

#include <QGuiApplication>
#include <thread>

#include <iostream>

static PluginSelectorWindow* window = nullptr;

#ifdef __cplusplus
extern "C"
{
#endif

    int start(int argc, char* argv[])
    {
        QGuiApplication a(argc, argv);

        QSurfaceFormat format;
        format.setRenderableType(QSurfaceFormat::OpenGL);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setVersion(3, 3);

        auto w = new PluginSelectorWindow();
        w->wheelSlip->setFormat(format);

        w->show();
        w->wheelSlip->show();

        std::atomic_thread_fence(std::memory_order_release);
        window = w;

        return a.exec();
    }

    void overlay_start()
    {
        static bool initiated = false;
        if (!initiated)
        {
            initiated = true;
            std::thread background{[&] { start(0, 0); }};
            background.detach();

            while (!window)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                std::atomic_thread_fence(std::memory_order_acquire);
            }
            std::atomic_thread_fence(std::memory_order_acquire);
        }
    }
    void overlay_update_wheel_slip(double rl, double rr, double fl, double fr)
    {
        window->wheelSlip->updateSlip(rl, rr, fl, fr);
    }

#ifdef __cplusplus
}  // extern C
#endif
