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
    void updateSlip(int rl, int rr, int fl, int fr);

  signals:
    void slipSignal();

  private:
    int rl;
    int rr;
    int fl;
    int fr;

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    int  mouseClickX;
    int  mouseClickY;
};

#endif  // WHEELSLIPWINDOW_H
