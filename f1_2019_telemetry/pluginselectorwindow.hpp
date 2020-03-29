#ifndef PLUGINSELECTORWINDOW_H
#define PLUGINSELECTORWINDOW_H

#include <QWindow>
#include "wheelslipwindow.hpp"

class PluginSelectorWindow : public QWindow
{
    Q_OBJECT

  public:
    PluginSelectorWindow();
    bool event(QEvent* ev) override;

    WheelSlipWindow* wheelSlip;
};

#endif  // PLUGINSELECTORWINDOW_H
