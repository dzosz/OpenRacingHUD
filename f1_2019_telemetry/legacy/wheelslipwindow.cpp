#include "wheelslipwindow.hpp"

#include "helpers.h"

#include <QDebug>
#include <QGuiApplication>
#include <QPainter>
#include <QScreen>
#include <QTimer>

WheelSlipWindow::WheelSlipWindow()
{
    static QTimer timer(this);
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer.start(1000 / 60);

    connect(this, SIGNAL(slipSignal()), this, SLOT(update()));
    this->setFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);

    this->setOpacity(0.6);

    // scale ellipses
    QScreen* screen         = QGuiApplication::primaryScreen();
    QRect    screenGeometry = screen->geometry();

    double desktopHeight = screenGeometry.height();
    double desktopWidth  = screenGeometry.width();

    double scaleMultiplier = desktopHeight / 2160;
    circleRadius           = 150 * scaleMultiplier;

    auto pluginWidth  = circleRadius * 3;
    auto pluginHeight = circleRadius * 3.5;
    // this->resize(pluginWidth, pluginHeight);

    this->setGeometry((desktopWidth / 2) - (pluginWidth / 2),
                      (desktopHeight / 2) + (pluginHeight / 2), pluginWidth, pluginHeight);

    // ALTERNATIVE TO  format.setAlphaBufferSize(8);
    // render only selected area of the window, clicking between the regions does not activate the
    // window

    auto a   = QRegion(QRect(0, 0, circleRadius, circleRadius), QRegion::Ellipse);
    auto b   = QRegion(QRect(0, 2.5 * circleRadius, circleRadius, circleRadius), QRegion::Ellipse);
    auto c   = QRegion(QRect(2 * circleRadius, 0, circleRadius, circleRadius), QRegion::Ellipse);
    auto d   = QRegion(QRect(2 * circleRadius, 2.5 * circleRadius, circleRadius, circleRadius),
                     QRegion::Ellipse);
    auto all = a.united(b).united(c).united(d);
    this->setMask(all);
}

/*
void WheelSlipWindow::initializeGL()
{
}
*/
void WheelSlipWindow::paintGL()
{
    QPainter painter(this);

    // erase previous area so it's transparent
    // painter.setCompositionMode(QPainter::CompositionMode_Source);
    // painter.eraseRect(QRectF(0, 0, 500, 600));
    // painter.fillRect(QRectF(0, 0, 500, 600), Qt::transparent);
    // painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    // painter.setRenderHint(QPainter::Antialiasing, true);

    auto  textPen = QPen(Qt::black);
    QFont textFont;
    textFont.setPixelSize(circleRadius / 3);

    painter.setPen(textPen);
    painter.setFont(textFont);

    painter.setBrush(QColor::fromHsv(getHueForWheelSlip(this->frontLeft), 255, 255));
    painter.drawEllipse(QRectF(0, 0, circleRadius, circleRadius));

    painter.drawText(QRect(0, 0, circleRadius, circleRadius), Qt::AlignCenter,
                     QString::number(this->frontLeft));

    painter.setBrush(QColor::fromHsv(getHueForWheelSlip(this->frontRight), 255, 255));
    painter.translate(2 * circleRadius, 0);
    painter.drawEllipse(QRectF(0, 0, circleRadius, circleRadius));
    painter.drawText(QRect(0, 0, circleRadius, circleRadius), Qt::AlignCenter,
                     QString::number(this->frontRight));

    painter.setBrush(QColor::fromHsv(getHueForWheelSlip(this->rearLeft), 255, 255));
    painter.translate(-2 * circleRadius, 2.5 * circleRadius);
    painter.drawEllipse(QRectF(0, 0, circleRadius, circleRadius));
    painter.drawText(QRect(0, 0, circleRadius, circleRadius), Qt::AlignCenter,
                     QString::number(this->rearLeft));

    painter.setBrush(QColor::fromHsv(getHueForWheelSlip(this->rearRight), 255, 255));
    painter.translate(2 * circleRadius, 0);
    painter.drawEllipse(QRectF(0, 0, circleRadius, circleRadius));
    painter.drawText(QRect(0, 0, circleRadius, circleRadius), Qt::AlignCenter,
                     QString::number(this->rearRight));

    painter.end();
    awaitingRender = false;
}

void WheelSlipWindow::timeout()
{
    if (awaitingRender)
    {
        std::atomic_thread_fence(std::memory_order_acquire);
        this->update();
    }
}

void WheelSlipWindow::updateSlip(double RL, double RR, double FL, double FR)
{
    int rl = getPercentageWheelSlip(RL);
    int rr = getPercentageWheelSlip(RR);
    int fl = getPercentageWheelSlip(FL);
    int fr = getPercentageWheelSlip(FR);

    bool anyChanges = !(rl == rearLeft && rr == rearRight && fl == frontLeft && fr == frontRight);

    if (anyChanges)
    {
        this->rearLeft   = rl;
        this->rearRight  = rr;
        this->frontLeft  = fl;
        this->frontRight = fr;

        // emiting signal from a python thread is a latency joke, just use timer in the
        // mainthread to render updated values;
        // emit slipSignal();

        std::atomic_thread_fence(std::memory_order_release);
        this->awaitingRender = anyChanges;
    }
}

void WheelSlipWindow::mousePressEvent(QMouseEvent* event)
{
    mouseClickX = event->x();
    mouseClickY = event->y();
}

void WheelSlipWindow::mouseReleaseEvent(QMouseEvent* event)
{
    auto posx = event->globalX() - mouseClickX;
    auto posy = event->globalY() - mouseClickY;

    this->setPosition(posx, posy);
}
