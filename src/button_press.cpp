#include "button_press.hpp"
using namespace elev;

ButtonPress::ButtonPress(sysclk_t time, std::atomic<int>* btn):
pressed_at(time), btn(btn)
{
    //Empty
}

bool ButtonPress::poll(void) {
    auto t = std::chrono::system_clock::now();
    //dt is milliseconds
    auto dt = (pressed_at - t).count() / static_cast<double>(1000000);
    if(dt > 50) {
        *btn = 0;
        return true;
    }
    return false;
}