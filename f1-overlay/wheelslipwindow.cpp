#include "wheelslipwindow.hpp"

#include "helpers.h"

#include <QDebug>
#include <QPainter>
#include <QTimer>

WheelSlipWindow::WheelSlipWindow()
{
    static QTimer timer(this);
    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    timer.start(1000 / 60);

    connect(this, SIGNAL(slipSignal()), this, SLOT(update()));

    this->setFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);

    this->setOpacity(0.6);

    // ALTERNATIVE TO  format.setAlphaBufferSize(8);
    // render only selected area of the window
    /*
    auto a   = QRegion(QRect(0, 0, 150, 150), QRegion::Ellipse);
    auto b   = QRegion(QRect(0, 400, 150, 150), QRegion::Ellipse);
    auto c   = QRegion(QRect(300, 0, 150, 150), QRegion::Ellipse);
    auto d   = QRegion(QRect(300, 400, 150, 150), QRegion::Ellipse);
    auto all = a.united(b).united(c).united(d);
    this->setMask(all);
    */
}
void WheelSlipWindow::initializeGL()
{
}

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
    textFont.setPixelSize(50);

    painter.setPen(textPen);
    painter.setFont(textFont);

    qreal circleRadius = 150;

    painter.setBrush(QColor::fromHsv(getHueForWheelSlip(this->fl), 255, 255));
    painter.drawEllipse(QRectF(0, 0, circleRadius, circleRadius));

    painter.drawText(QRect(0, 0, circleRadius, circleRadius), Qt::AlignCenter,
                     QString::number(this->fl));

    painter.setBrush(QColor::fromHsv(getHueForWheelSlip(this->fr), 255, 255));
    painter.translate(300, 0);
    painter.drawEllipse(QRectF(0, 0, circleRadius, circleRadius));
    painter.drawText(QRect(0, 0, circleRadius, circleRadius), Qt::AlignCenter,
                     QString::number(this->fr));

    painter.setBrush(QColor::fromHsv(getHueForWheelSlip(this->rl), 255, 255));
    painter.translate(-300, 400);
    painter.drawEllipse(QRectF(0, 0, circleRadius, circleRadius));
    painter.drawText(QRect(0, 0, circleRadius, circleRadius), Qt::AlignCenter,
                     QString::number(this->rl));

    painter.setBrush(QColor::fromHsv(getHueForWheelSlip(this->rr), 255, 255));
    painter.translate(300, 0);
    painter.drawEllipse(QRectF(0, 0, circleRadius, circleRadius));
    painter.drawText(QRect(0, 0, circleRadius, circleRadius), Qt::AlignCenter,
                     QString::number(this->rr));

    painter.end();
}

void WheelSlipWindow::updateSlip(int rl, int rr, int fl, int fr)
{
    this->rl = rl;
    this->rr = rr;
    this->fl = fl;
    this->fr = fr;

    // emiting signal from a separate thread is a joke, just use timer from the mainthread to
    // render updated values

    // emit slipSignal();
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
