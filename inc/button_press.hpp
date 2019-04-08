#ifndef BUTTON_PRESS_HPP
#define BUTTON_PRESS_HPP
#include <chrono>
#include <mutex>
#include <atomic>

namespace elev {
using sysclk_t = std::chrono::time_point<std::chrono::system_clock> ;
using msec_t = std::chrono::duration<double>;
class ButtonPress {
    public:
        ButtonPress(sysclk_t time, std::atomic<int>* btn, msec_t duration);
        ButtonPress() = delete;
        bool poll(void);
    private:
        sysclk_t pressed_at;
        std::atomic<int>* btn;
        msec_t duration;
};

} //namespace elev

#endif //BUTTON_PRESS_HPP
