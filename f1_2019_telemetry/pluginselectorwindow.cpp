#include "pluginselectorwindow.hpp"
PluginSelectorWindow::PluginSelectorWindow()
{
    this->setTitle(QString("F1 2019 Overlay"));
}

bool PluginSelectorWindow::event(QEvent *ev)
{
    if (ev->type() == QEvent::Close)
    {
        wheelSlip.close();
    }

    return QWindow::event(ev);
}
