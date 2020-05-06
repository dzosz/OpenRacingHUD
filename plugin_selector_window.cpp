#include "plugin_selector_window.hpp"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QGroupBox>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QMetaObject>
#include <QPushButton>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQuickWindow>
#include <QString>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <algorithm>

#include <iostream>

static const size_t UPDATES_PER_SEC = 60;

namespace
{
void jsonToString(QString& stringified, const QJsonValue& value)
{
    switch (value.type())
    {
        case QJsonValue::Double:
            stringified += QString::number(value.toDouble());
            break;
        case QJsonValue::String:
            stringified += value.toString();
            break;
        case QJsonValue::Bool:
            stringified += value.toBool() ? "true" : "false";
            break;
        case QJsonValue::Array:
        {
            auto arr = value.toArray();
            std::accumulate(arr.begin(), arr.end(), stringified, [&](auto&&, auto&& b) {
                if (!stringified.isEmpty())
                {
                    stringified += ", ";
                }
                jsonToString(stringified, b);
                return stringified;
            });

            break;
        }
        case QJsonValue::Object:
        {
            auto obj = value.toObject();
            for (auto it = obj.begin(); it != obj.end(); ++it)
            {
                if (!stringified.isEmpty())
                {
                    stringified += ", ";
                }
                stringified += it.key() + ": {";
                jsonToString(stringified, it.value());
                stringified += "}";
            }
            break;
        }
    }
}
}  // namespace

PluginSelectorWindow::PluginSelectorWindow()
    : startedGameParser{nullptr}, timer{nullptr}, table{nullptr}
{
    this->setWindowTitle("Plugin Selector");
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->resize(800, 900);
    engine.rootContext()->setContextProperty("pythonExecutor", &pyRunner);

    this->findPlugins();
    this->createGUI();

    timer = new QTimer();
    this->connect(timer, &QTimer::timeout, this, [&] { this->refreshData(); });
    timer->start(1000 / UPDATES_PER_SEC);
}

PluginSelectorWindow::~PluginSelectorWindow()
{
    for (auto& plug : qmlPlugins)
    {
        plug.second->close();
    }

    if (startedGameParser)
    {
        QMetaObject::invokeMethod(startedGameParser, "stop");
        startedGameParser = nullptr;
    }
    timer->stop();
    delete timer;
}
void PluginSelectorWindow::pluginEvent(const QString& plugin, const QString& event)
{
    auto it = qmlPlugins.find(plugin);
    if (it == qmlPlugins.end())
    {
        auto err = std::string("Cannot find plugin: " + event.toStdString());
        qDebug() << err.c_str();
        throw std::runtime_error(err);
    }

    auto object = it->second;

    if (event == "Start")
    {
        object->show();
    }
    else if (event == "Stop")
    {
        object->hide();
    }
}

void PluginSelectorWindow::gamePluginEvent(const QString& plugin, const QString& event)
{
    auto it = gameParsers.find(plugin);
    if (it == gameParsers.end())
    {
        auto err = std::string("Cannot find game plugin: " + event.toStdString());
        qDebug() << err.c_str();
        throw std::runtime_error(err);
    }

    auto object = it->second;

    if (event == "Start")
    {
        if (startedGameParser)
        {
            QMetaObject::invokeMethod(startedGameParser, "stop");
        }

        // clear Data tab
        for (auto& dataEntry : this->dataEntries)
        {
            dataEntry.second->setText("");
        }

        startedGameParser = object;
        QMetaObject::invokeMethod(object, "start");
    }
    else if (event == "Stop")
    {
        if (object == startedGameParser)
        {
            startedGameParser = nullptr;
            QMetaObject::invokeMethod(object, "stop");
        }
    }
}

void PluginSelectorWindow::findPlugins()
{
    {
        // plugins
        auto directory = QDir(QApplication::applicationDirPath());

        auto ret = directory.cd("qml");
        assert(ret);
        ret = directory.cd("plugins");

        auto fileList = directory.entryInfoList(QStringList("*.qml"));
        qDebug() << "Loading plugins from:" << directory;
        for (const auto& plugin : fileList)
        {
            qDebug() << "-" << plugin.fileName();

            QQmlComponent component(&engine, plugin.absoluteFilePath());
            QObject*      object = component.create();

            /*
                  auto view = new QQuickView();
                  view->setSource(QUrl::fromLocalFile(plugin.absoluteFilePath()));
                  // view.show();
                  auto* object = view;
            */

            if (!object)
            {
                qDebug() << "cannot load " << plugin << "\n" << component.errors();
            }
            else
            {
                auto* item = qobject_cast<QQuickWindow*>(object);
                // auto* item = object;

                if (!item)
                {
                    qDebug() << plugin << " has to be of type QQuickWindow";
                }
                else
                {
                    item->hide();
                    qmlPlugins[plugin.fileName()] = item;
                }
            }
        }
    }
    {
        // game parsers
        auto directory = QDir(QApplication::applicationDirPath());
        auto ret       = directory.cd("qml");
        assert(ret);
        ret = directory.cd("games");

        auto fileList = directory.entryInfoList(QStringList("*.qml"));
        qDebug() << "Loading game parsers from:" << directory;
        for (const auto& plugin : fileList)
        {
            qDebug() << "-" << plugin.fileName();
            QQmlComponent component(&engine, plugin.absoluteFilePath());

            QObject* object = component.create();

            if (!object)
            {
                int errs = component.errors().size();
                qDebug() << "cannot load " << plugin << "\n" << component.errors();
            }
            else
            {
                gameParsers[plugin.fileName()] = object;
            }
        }
    }
}

void PluginSelectorWindow::createGUI()
{
    auto tabWidget      = new QTabWidget;
    auto pluginWidget   = new QWidget;
    auto selectorLayout = new QVBoxLayout();

    table = new QTableWidget();
    table->setColumnCount(2);
    table->resizeRowsToContents();
    table->resizeColumnsToContents();
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    QStringList labels;
    labels << "Key"
           << "Value";
    table->setHorizontalHeaderLabels(labels);
    table->setWordWrap(true);
    tabWidget->addTab(pluginWidget, "Plugins");
    tabWidget->addTab(table, "Data");

    auto receiverGroup = new QGroupBox("Receivers");
    auto pluginsGroup  = new QGroupBox("Plugins");

    selectorLayout->addWidget(receiverGroup);
    selectorLayout->addWidget(pluginsGroup);
    selectorLayout->addStretch();
    pluginWidget->setLayout(selectorLayout);

    auto pluginGrid = new QGridLayout();
    pluginsGroup->setLayout(pluginGrid);

    auto gameParsersGrid = new QGridLayout();

    receiverGroup->setLayout(gameParsersGrid);
    this->setCentralWidget(tabWidget);

    int grindIndex = 0;
    for (const auto& p : this->qmlPlugins)
    {
        auto& plugin = p.first;
        auto& object = p.second;
        qDebug() << "Plugin:" << plugin;

        pluginGrid->addWidget(new QLabel(plugin), grindIndex, 0);
        pluginGrid->addWidget(new QLabel(""), grindIndex, 1);

        auto start = new QPushButton("start");
        pluginGrid->addWidget(start, grindIndex, 2);

        auto stop = new QPushButton("stop");
        pluginGrid->addWidget(stop, grindIndex, 3);

        // auto meta = object->metaObject();
        //        for (int i = meta->methodOffset(); i < meta->methodCount(); ++i)
        //            qDebug() << meta->method(i).methodSignature();

        QPalette pal;
        pal.setColor(QPalette::Button, QColor(Qt::green));
        stop->setPalette(pal);

        this->connect(start, &QPushButton::clicked, this, [=] {
            this->pluginEvent(plugin, "Start");
            QPalette pal;
            stop->setAutoFillBackground(true);
            stop->setPalette(pal);
            pal.setColor(QPalette::Button, QColor(Qt::green));
            start->setAutoFillBackground(true);
            start->setPalette(pal);
        });

        this->connect(stop, &QPushButton::clicked, this, [=] {
            this->pluginEvent(plugin, "Stop");
            QPalette pal;
            start->setAutoFillBackground(true);
            start->setPalette(pal);
            pal.setColor(QPalette::Button, QColor(Qt::green));
            stop->setAutoFillBackground(true);
            stop->setPalette(pal);
        });

        ++grindIndex;
    }

    grindIndex = 0;
    for (const auto& p : this->gameParsers)
    {
        auto& plugin = p.first;
        auto& object = p.second;
        qDebug() << "Game:" << plugin;

        gameParsersGrid->addWidget(new QLabel(plugin), grindIndex, 0);
        gameParsersGrid->addWidget(new QLabel(""), grindIndex, 1);

        auto start = new QPushButton("start");
        auto stop  = new QPushButton("stop");

        QPalette pal;
        pal.setColor(QPalette::Button, QColor(Qt::green));
        stop->setPalette(pal);

        auto meta = object->metaObject();

        if (meta->indexOfMethod("start()") != -1)
        {
            gameParsersGrid->addWidget(start, grindIndex, 2);
            this->connect(start, &QPushButton::clicked, this, [=] {
                this->gamePluginEvent(plugin, "Start");
                QPalette pal;
                stop->setPalette(pal);
                pal.setColor(QPalette::Button, QColor(Qt::green));
                start->setPalette(pal);
            });
        }

        if (meta->indexOfMethod("stop()") != -1)
        {
            gameParsersGrid->addWidget(stop, grindIndex, 3);
            this->connect(stop, &QPushButton::clicked, this, [=] {
                this->gamePluginEvent(plugin, "Stop");
                QPalette pal;
                start->setPalette(pal);
                pal.setColor(QPalette::Button, QColor(Qt::green));
                stop->setPalette(pal);
            });
        }
        ++grindIndex;
    }
}

void PluginSelectorWindow::refreshData()
{
    if (startedGameParser)
    {
        QString data;
        QMetaObject::invokeMethod(startedGameParser, "getJsonData", Q_RETURN_ARG(QString, data));
        auto doc = QJsonDocument::fromJson(data.toLatin1()).object();
        // auto    dataMap = doc.object().toVariantMap();

        for (auto& plugin : qmlPlugins)
        {
            auto telemetryMessage = plugin.second->findChild<QObject*>("telemetry");
            if (telemetryMessage)
            {
                QMetaObject::invokeMethod(telemetryMessage, "onReceive", Q_ARG(QString, data));
            }
            QMetaObject::invokeMethod(plugin.second, "onUpdate");
        }

        static size_t iteration;
        // update gui once per sec
        if (!(iteration++ % UPDATES_PER_SEC))
        {
            // qDebug() << "update gui: " << doc;
            for (auto it = doc.begin(); it != doc.end(); ++it)
            {
                auto key   = it.key();
                auto posIt = dataEntries.find(key);
                if (posIt == dataEntries.end())  // rare case of inserting non-existing key
                {
                    auto newPos = table->rowCount();
                    table->insertRow(newPos);

                    posIt            = dataEntries.find(key);
                    auto keyHolder   = new QTableWidgetItem(key);
                    auto valueHolder = new QTableWidgetItem();
                    keyHolder->setFlags(keyHolder->flags() ^ Qt::ItemIsEditable);
                    valueHolder->setFlags(valueHolder->flags() ^ Qt::ItemIsEditable);
                    dataEntries.insert({key, valueHolder});
                    table->setItem(newPos, 0, keyHolder);
                    table->setItem(newPos, 1, valueHolder);
                }
                QString stringified;
                auto    value = it.value();
                jsonToString(stringified, value);
                auto valueHolder = dataEntries.find(key)->second;
                assert(valueHolder);
                valueHolder->setText(stringified);
            }
        }
    }
    /*

qDebug() << "Property value:" << QQmlProperty::read(object, "someNumber").toInt();
QQmlProperty::write(object, "someNumber", 5000);

qDebug() << "Property value:" << object->property("someNumber").toInt();
object->setProperty("someNumber", 100);

    QObject* childObject = object->findChild<QObject*>("telemetry");
    assert(childObject);
    qDebug() << "Property value:" << childObject->property("wheelSlip");

    for (auto it : data.toStdMap())
    {
        auto prop = childObject->property(it.first.toStdString().c_str());
        if (prop.isValid())
        {
            prop.setValue(it.second.value<double>());
            qDebug() << "setting " << it.first << " to " << it.second << " type "
                     << it.second.type();
        }
    }*/
}

/*
bool PluginSelectorWindow::event(QEvent *ev)
{

auto ret = QWindow::event(ev);
if (ev->type() == QEvent::Close && wheelSlip)
{
    wheelSlip->close();
    wheelSlip = nullptr;
}
return ret;

return false;
}
*/
