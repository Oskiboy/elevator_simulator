#ifndef BUTTON_PRESS_HPP
#define BUTTON_PRESS_HPP
/**
 * @file button_press.hpp
 * @author Oskar Oestby (oskar.oestby@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-04-17
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include <chrono>
#include <mutex>
#include <atomic>

namespace elev {
/**
 * @brief Used for keeping track of timestamps
 * 
 */
using sysclk_t = std::chrono::time_point<std::chrono::system_clock>;

/**
 * @brief A milliseconds duration. 
 * 
 */
using msec_t = std::chrono::duration<double>;

/**
 * @brief The ButtonPress class is used to keep track of button presses.
 * 
 */
class ButtonPress {
    public:
        /**
         * @brief Construct a new ButtonPress event.
         * 
         * @param time The time of the button press.
         * @param btn A pointer to the button pressed.
         * @param duration The duration the button should be pressed.
         */
        ButtonPress(sysclk_t time, std::atomic<int>* btn, msec_t duration);

        /**
         * @brief A button press event should only be constructable with parameters.
         * 
         */
        ButtonPress() = delete;

        /**
         * @brief Check if the button event is carried through and the event should be deleted.
         * 
         * @return true The button event is finished and should be removed.
         * @return false The button event is not yet done.
         */
        bool poll(void);
    private:
        sysclk_t pressed_at;
        std::atomic<int>* btn;
        msec_t duration;
};

} //namespace elev

#endif //BUTTON_PRESS_HPP
