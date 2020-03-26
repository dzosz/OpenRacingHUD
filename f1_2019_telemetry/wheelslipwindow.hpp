#ifndef WHEELSLIPWINDOW_H
#define WHEELSLIPWINDOW_H

#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLWindow>

class WheelSlipWindow : public QOpenGLWindow
{
    Q_OBJECT
  public:
    WheelSlipWindow();

    void paintGL() override;
    void initializeGL() override;
    // void resizeGL(int width, int height) override;
    void updateSlip(double rl, double rr, double fl, double fr);

  signals:
    void slipSignal();

  private:
    int rearLeft   = 0;
    int rearRight  = 0;
    int frontLeft  = 0;
    int frontRight = 0;

    void  mousePressEvent(QMouseEvent *event);
    void  mouseReleaseEvent(QMouseEvent *event);
    int   mouseClickX;
    int   mouseClickY;
    bool  awaitingRender = true;
    qreal circleRadius;
};

#endif  // WHEELSLIPWINDOW_H
