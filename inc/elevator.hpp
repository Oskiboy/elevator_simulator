#ifndef ELEVATOR_HPP
#define ELEVATOR_HPP

#include <chrono>
#include <mutex>
#include <array>

#include <signals.hpp>

#include <sys/time.h>

#define TRACK_LENGTH    40.5   //Total length of the track in cm
#define TRACK_TIME      8.0     //Seconds per track length

namespace elev {
enum class Direction {
    UP      = 1,
    STOP    = 0,
    DOWN    = -1
};

class Elevator {
    public:
        Elevator(int id = 0);
        Elevator(int id, double motor_speed);
        ~Elevator();
        //Called to update the internal state of the machine.
        void    update(void);
        void    run(void);
        bool    ok();
        void    stop();
        int     getId(void);
        
        //Motor controller functions
        void    setMotorDirection(Direction new_dir);
        void    startMotor(void);
        void    stopMotor(void);

        //All signals
        int setSignal(Signals sig, int value);
        int getSignal(Signals sig);

        double  getElevatorPosition(void);
    private:
        void updatePosition(void);
        void updateSignals(void);
        void updateTime(void);


    private:
        const int _id;
        std::mutex m, ok_mtx;
        bool running;
        //Signals
        std::array<int, static_cast<int>(Signals::NUM_SIGNALS)> signals;
        
        //Motor stuff
        double  pos;
        double  _motor_speed;
        int     _last_dir;
        double  dt;
        std::chrono::time_point<std::chrono::system_clock> timestamp, t;
};
} //namespace elev
#endif //ELEVATOR_HPP
