#include "button_press.hpp"
using namespace elev;

ButtonPress::ButtonPress(sysclk_t time, std::atomic<int>* btn, msec_t duration):
pressed_at(time), btn(btn), duration(duration)
{
    //Presses the button
    *btn = 1;
}

bool ButtonPress::poll(void) {
    auto t = std::chrono::system_clock::now();
    //dt is milliseconds
    auto dt = std::chrono::nanoseconds((pressed_at - t).count());
    if(dt > duration) {
        *btn = 0;
        return true;
    }
    return false;
}