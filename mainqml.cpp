#include "plugin_selector_window.hpp"

#include <stdio.h>
#include <QApplication>
#include <QQmlExtensionPlugin>
#include <QtPlugin>

static void messageHandler(QtMsgType, const QMessageLogContext &, const QString &msg)
{
    printf(qUtf8Printable(msg));
    printf("\n");
}

#if defined(WINDOWS) || defined(__MINGW32__) || defined(__MINGW64__)
Q_IMPORT_PLUGIN(QtQuick2Plugin)
Q_IMPORT_PLUGIN(QtQuick2WindowPlugin)

/*
Q_IMPORT_PLUGIN(QtQuickControlsPlugin)
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
Q_IMPORT_PLUGIN(QXcbGlxIntegrationPlugin)
Q_IMPORT_PLUGIN(QEvdevKeyboardPlugin)
Q_IMPORT_PLUGIN(QEvdevMousePlugin)

#ifndef _WIN32
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
Q_IMPORT_PLUGIN(QXcbGlxIntegrationPlugin)
Q_IMPORT_PLUGIN(QEvdevKeyboardPlugin)
Q_IMPORT_PLUGIN(QEvdevMousePlugin)
#else
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
Q_IMPORT_PLUGIN(QtQuick2DialogsPrivatePlugin)
Q_IMPORT_PLUGIN(QmlFolderListModelPlugin)
#endif

Q_IMPORT_PLUGIN(QtQuickControlsPlugin)
Q_IMPORT_PLUGIN(QtQuick2DialogsPlugin)
Q_IMPORT_PLUGIN(QtQuickLayoutsPlugin)
Q_IMPORT_PLUGIN(QtQuick2WindowPlugin)
Q_IMPORT_PLUGIN(QmlSettingsPlugin)
Q_IMPORT_PLUGIN(QtQuick2PrivateWidgetsPlugin)

qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2Plugin().instance())
    ->registerTypes("QtQuick");
qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2WindowPlugin().instance())
    ->registerTypes("QtQuick.Window");
qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControls1Plugin().instance())
    ->registerTypes("QtQuick.Controls");
qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControls1Plugin().instance())
    ->registerTypes("QtQuick.Controls.Styles");
qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControls1Plugin().instance())
    ->registerTypes("QtQuick.Controls.Private");
qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControls2Plugin().instance())
    ->registerTypes("QtQuick.Controls");
*/
#endif

int main(int argc, char **argv)
{
#if defined(WINDOWS) || defined(__MINGW32__) || defined(__MINGW64__)
    qInstallMessageHandler(messageHandler);
#endif

        QApplication app(argc, argv);
    auto             win = new PluginSelectorWindow();  // has attr Qt::WA_DeleteOnClose
    win->show();
    return app.exec();
}
