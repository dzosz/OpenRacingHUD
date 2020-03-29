#include "pluginselectorwindow.hpp"

#include <iostream>

PluginSelectorWindow::PluginSelectorWindow()
{
    this->setTitle(QString("F1 2019 Overlay"));
    wheelSlip = new WheelSlipWindow;
}

bool PluginSelectorWindow::event(QEvent *ev)
{
    auto ret = QWindow::event(ev);
    if (ev->type() == QEvent::Close && wheelSlip)
    {
        wheelSlip->close();
        wheelSlip = nullptr;
    }
    return ret;
}
