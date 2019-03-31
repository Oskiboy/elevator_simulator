#ifndef BUTTON_PRESS_HPP
#define BUTTON_PRESS_HPP
#include <chrono>
#include <mutex>

namespace elev {
typedef std::chrono::time_point<std::chrono::system_clock> sysclk_t;

class ButtonPress {
    public:
        ButtonPress(sysclk_t time, int* btn, std::mutex* m);
        ButtonPress() = delete;
        bool poll(void);
    private:
        sysclk_t pressed_at;
        int* btn;
        std::mutex* sig_m;
};
} //namespace elev

#endif //BUTTON_PRESS_HPP
