#ifndef ELEVATOR_HPP
#define ELEVATOR_HPP

#include <chrono>
#include <mutex>
#include <array>
#include <vector>
#include <atomic>

#include <commands.hpp>
#include <button_press.hpp>

#include <sys/time.h>

#define TRACK_LENGTH    40.5   //Total length of the track in cm
#define TRACK_TIME      8.0     //Seconds per track length

namespace elev {

enum class Direction {
    UP      = 1,
    STOP    = 0,
    DOWN    = -1
};

typedef std::unique_ptr<std::atomic<int>[][3]> button_t;
typedef std::unique_ptr<std::atomic<int>[]>    light_t;

typedef struct {
    std::atomic<double> speed;
    Direction           direction;
} motor_t;

typedef struct {
    button_t            buttons;
    light_t             lights;
    std::atomic<int>    obstruction;
    std::atomic<int>    door_light;
    std::atomic<int>    stop;
    std::atomic<int>    stop_light;
    std::atomic<int>    floor_sensor;
    motor_t             motor;
    std::atomic<double> position;
} ElevatorSignals_t;

typedef std::chrono::time_point<std::chrono::system_clock> sysclk_t;
class ButtonPress;

class Elevator {
    public:
        Elevator(int id = 0, int num_floors = 4);
        Elevator(int id, double motor_speed, int num_floors);
        ~Elevator();
        //Called to update the internal state of the machine.
        void    update(void);
        void    run(void);
        bool    ok();
        void    stop();
        int     getId(void);

        //Primary communication interface
        command_t executeCommand(const command_t &cmd);

        double  getElevatorPosition(void);
    private:
        void    updatePosition(void);
        void    updateSignals(void);
        void    updateTime(void);

        int     getSignal(const command_t &cmd);
        void    setSignal(const command_t &cmd);

        void    resetSelf(void);

    private:
        const std::atomic<int> _id;
        int _num_floors;

        std::atomic<bool> running;

        //Signals
        ElevatorSignals_t           signals;
        std::vector<ButtonPress>    events;

        //Motor stuff
        std::atomic<Direction>   _last_dir;

        //Timestamps
        std::atomic<double> dt;
        sysclk_t    timestamp, t;
};
} //namespace elev

#endif //ELEVATOR_HPP

