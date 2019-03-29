#ifndef ELEVATOR_HPP
#define ELEVATOR_HPP

#include <chrono>
#include <mutex>
#include <array>

#include <signals.hpp>
#include <commands.hpp>

#include <sys/time.h>

#define TRACK_LENGTH    40.5   //Total length of the track in cm
#define TRACK_TIME      8.0     //Seconds per track length

namespace elev {

enum class Direction {
    UP      = 1,
    STOP    = 0,
    DOWN    = -1
};

typedef std::unique_ptr<int[][3]> button_t;
typedef std::unique_ptr<int[]>    light_t;

typedef struct {
    double      speed;
    Direction   direction;
} motor_t;

typedef struct {
    button_t    buttons;
    light_t     lights;
    int         obstruction;
    int         door_light;
    int         stop;
    int         stop_light;
    int         floor_sensor;
    motor_t     motor;
    double      position;
} ElevatorSignals_t;


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
        
        //Motor controller functions
        void    setMotorDirection(Direction new_dir);
        void    startMotor(void);
        void    stopMotor(void);

        //All signals
        //int setSignal(Signals sig, int value);
        //int getSignal(Signals sig);
        command_t executeCommand(const command_t &cmd);

        double  getElevatorPosition(void);
    private:
        void updatePosition(void);
        void updateSignals(void);
        void updateTime(void);
        void nullSignals();

        int getSignal(const command_t &cmd);
        void setSignal(const command_t &cmd);

    private:
        const int _id;
        std::mutex sig_m, ok_mtx;
        bool running;

        //Signals
        ElevatorSignals_t signals;

        //Motor stuff
        double      pos;
        double      _motor_speed;
        Direction   _last_dir;
        int         _num_floors;
        double      dt;
        std::chrono::time_point<std::chrono::system_clock> timestamp, t;
};
} //namespace elev
#endif //ELEVATOR_HPP
