#include "plugin_selector_window.hpp"
#include "wheelslipwindow.hpp"

#include "f1_data_receiver.hpp"

#include <QGuiApplication>
#include <functional>
#include <iostream>

#include <unistd.h>

using namespace std::placeholders;

/*
#ifdef __cplusplus
extern "C"
{
#endif
    int start(int argc, char* argv[]);
#ifdef __cplusplus
}
#endif
*/
int main(int argc, char* argv[])
{
    /*
    return start(argc, argv);
}

int start(int argc, char* argv[])
{
*/
    QGuiApplication a(argc, argv);
    // a.setQuitOnLastWindowClosed(true);

    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3, 3);

    // format.setAlphaBufferSize(8);

    PluginSelectorWindow* w = new PluginSelectorWindow();
    F1DataReceiver        receiver;

    receiver.setWheelCallback(
        std::bind(&WheelSlipWindow::updateSlip, w->wheelSlip, _1, _2, _3, _4));

    w->wheelSlip->setFormat(format);
    // w.resize(QSize(500, 600));
    receiver.start();

    w->show();
    w->wheelSlip->show();

    return a.exec();
}
