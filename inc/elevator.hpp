#ifndef ELEVATOR_HPP
#define ELEVATOR_HPP
/**
 * @file elevator.hpp
 * @author Oskar Oestby (oskar.oestby@gmail.com)
 * @brief The elevator simulator. Each object simulates its own elevator.
 * @version 0.1
 * @date 2019-04-17
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include <chrono>
#include <mutex>
#include <array>
#include <vector>
#include <atomic>

#include <commands.hpp>
#include <button_press.hpp>

#include <sys/time.h>

///TODO: The track length should be based on a single floor to work with other than 4 floors.
#define TRACK_LENGTH    (40.5)      ///Total length of the track with 4 floors.
#define TRACK_TIME      (8.0)       ///Seconds per 4 floors

/**
 * @brief Too keep the namespace clean a namespace for the elevator stuff is needed.
 * 
 */
namespace elev {

/**
 * @brief A way to keep track of directions in a explicit way.
 * 
 */
enum class Direction {
    UP      = 1,
    STOP    = 0,
    DOWN    = -1
};

/**
 * @brief An array used to keep track of all the buttons in the elevator.
 * 
 */
typedef std::unique_ptr<std::atomic<int>[][3]> button_t;

/**
 * @brief An array used to keep track of all the elevators lights.
 * 
 */
typedef std::unique_ptr<std::atomic<int>[]>    light_t;

/**
 * @brief A motor struct to save the state of the motor.
 * 
 */
typedef struct {
    std::atomic<double> speed;      //< The speed of the motor.
    Direction           direction;  //< The direction of the motor.
} motor_t;


/**
 * @brief All the elevators signals is kept in this struct.
 * 
 */
typedef struct {
    button_t            buttons;        //< All the elevators buttons. [0] is , [1] is and [2] is
    light_t             lights;         //< All the elevators lights
    std::atomic<int>    obstruction;    //< The obstruction sensor of the elevator.
    std::atomic<int>    door_light;     //< The door indicator light of the elevator.
    std::atomic<int>    stop;           //< The stop button state.
    std::atomic<int>    stop_light;     //< The stop light of the elevator.
    std::atomic<int>    floor_sensor;   //< The floor sensor of the elevator.
    motor_t             motor;          //< The current motor state.
    std::atomic<double> position;       //< The current position of the elevator.
} ElevatorSignals_t;

/**
 * @brief For easier to read code. The system_clock time_point is used to track the elapsed time after a button press.
 * 
 */
typedef std::chrono::time_point<std::chrono::system_clock> sysclk_t;

//Forward declaration
class ButtonPress;


/**
 * @brief The elevator class used for simulating an elevator.
 * 
 */
class Elevator {
    public:
        /**
         * @brief Construct a new Elevator object with a particular ID and numer of floors.
         * 
         * @param id The ID of the elevator.
         * @param num_floors The number of floors of the elevator. Default is 4.
         */
        Elevator(int id = 0, int num_floors = 4);
        /**
         * @brief Construct a new Elevator object with a desired speed.
         * 
         * @param id The ID of the elevator.
         * @param motor_speed The speed of the motor in cm/s
         * @param num_floors The number of floors.
         */
        Elevator(int id, double motor_speed, int num_floors);

        /**
         * @brief Destroy the Elevator object
         * 
         */
        ~Elevator();
        
        /**
         * @brief Updates the internal signals of the elevator.
         * 
         */
        void    update(void);

        /**
         * @brief Run the elevator simulation as long as the elevator is ok.
         * 
         */
        void    run(void);

        /**
         * @brief Check if the elevator is ok.
         * 
         * @return true The elevator is ok.
         * @return false The elevator is not ok.
         */
        bool    ok();

        /**
         * @brief Stop the elevator.
         * 
         */
        void    stop();

        /**
         * @brief Get the ID of the elevator.
         * 
         * @return int The ID of the elevator
         */
        int     getId(void);

        /**
         * @brief Given a command returns a command with information based on the given command.
         * 
         * @param cmd The command to be executed.
         * @return command_t Result of the executed command.
         */
        command_t executeCommand(const command_t &cmd);

        /**
         * @brief Get the position of the elevator.
         * 
         * @return double The position of the elevator.
         */
        double  getElevatorPosition(void);
    private:
        /**
         * @brief Increment or decrement the position of the elevator based on elapsed time and its speed.
         * 
         */
        void    updatePosition(void);

        /**
         * @brief Update signals so that they match the current state of the elevator.
         * 
         */
        void    updateSignals(void);
        
        /**
         * @brief Set a new timestamp.
         * 
         */
        void    updateTime(void);

        /**
         * @brief Get a signal
         * 
         * @param cmd The command describing what signal to return
         * @return int The value of the signal given in the command.
         */
        int     getSignal(const command_t &cmd);

        /**
         * @brief Set a specific signal.
         * 
         * @param cmd Describes what signal and what value to set it to.
         */
        void    setSignal(const command_t &cmd);

        /**
         * @brief Null all signals and reset position.
         * 
         */
        void    resetSelf(void);

    private:
        /**
         * @brief The ID of the elevator.
         * 
         */
        const std::atomic<int> _id;
        /**
         * @brief Number of floors this elevator has.
         * 
         */
        std::atomic<int> _num_floors;

        /**
         * @brief Saves the runnign state of the elevator.
         * 
         */
        std::atomic<bool> running;

        /**
         * @brief All the elevators internal signals.
         * 
         */
        ElevatorSignals_t           signals;

        /**
         * @brief Holds all the button events.
         * 
         */
        std::vector<ButtonPress>    events;

        /**
         * @brief Deprecated. Saves the last traveled direciton.
         * 
         */
        std::atomic<Direction>   _last_dir;

        
        /**
         * @brief Elapsed time since last loop.
         * 
         */
        std::atomic<double> dt;
        
        /**
         * @brief Timestamps for saving running time of the loop. 
         * 
         */
        sysclk_t    timestamp, t;
};
} //namespace elev

#endif //ELEVATOR_HPP

