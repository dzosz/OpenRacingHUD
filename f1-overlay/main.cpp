#include "wheelslipwindow.hpp"

#include <QGuiApplication>
#include <iostream>

WheelSlipWindow* window;
#ifdef __cplusplus
extern "C"
{
    int start(int argc, char* argv[]);
}
#endif

int main(int argc, char* argv[])
{
    return start(argc, argv);
}

int start(int argc, char* argv[])
{
    QGuiApplication a(argc, argv);
    QSurfaceFormat  format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3, 3);
    format.setAlphaBufferSize(8);

    static WheelSlipWindow w;
    window = &w;
    w.setFormat(format);
    w.resize(QSize(500, 600));
    w.show();
    return a.exec();
}
