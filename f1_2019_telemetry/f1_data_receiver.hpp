#ifndef F1_DATA_RECEIVER_HPP
#define F1_DATA_RECEIVER_HPP

#include <functional>

struct F1DataReceiver
{
    F1DataReceiver();
    ~F1DataReceiver();
    void start();
    void stop();

    void setWheelCallback(std::function<void(double, double, double, double)>);
};
#endif  // F1_DATA_RECEIVER_HPP
