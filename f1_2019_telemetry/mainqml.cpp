#include <QApplication>

#include "plugin_selector_window.hpp"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    auto         win = new PluginSelectorWindow();  // has attr Qt::WA_DeleteOnClose
    win->show();
    return app.exec();
}
