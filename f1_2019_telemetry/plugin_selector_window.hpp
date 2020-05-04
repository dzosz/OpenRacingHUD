#ifndef PLUGINSELECTORWINDOW_H
#define PLUGINSELECTORWINDOW_H

#include <python_runner.hpp>

#include <QBackingStore>
#include <QMainWindow>
#include <QMap>
#include <QQmlApplicationEngine>
#include <map>

class QQuickWindow;
class QTimer;
class QTableWidget;
class QTableWidgetItem;

class PluginSelectorWindow : public QMainWindow
{
  public:
    PluginSelectorWindow();
    ~PluginSelectorWindow();

    // bool event(QEvent* ev) override;

  private:
    void pluginEvent(const QString& plugin, const QString& action);
    void gamePluginEvent(const QString& plugin, const QString& action);
    void refreshData();
    void findPlugins();
    void createGUI();

    QQmlApplicationEngine                engine;
    std::map<QString, QQuickWindow*>     qmlPlugins;
    std::map<QString, QObject*>          gameParsers;
    std::map<QString, QTableWidgetItem*> dataEntries;
    PythonRunner                         pyRunner;
    QObject*                             startedGameParser;
    QTimer*                              timer;
    QTableWidget*                        table;
};

#endif  // PLUGINSELECTORWINDOW_H
