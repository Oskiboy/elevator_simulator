#include <cmath>
#include <iostream>
#include <thread>
#include <algorithm>

#include <elevator.hpp>

#include <unistd.h>

using namespace elev;

Elevator::Elevator(int id, int num_floors):
        _id(id),  
        _num_floors(num_floors)
{
    signals.buttons = std::make_unique<std::atomic<int>[][3]>(_num_floors);
    signals.lights = std::make_unique<std::atomic<int>[]>(_num_floors);
    signals.motor.speed = TRACK_LENGTH / TRACK_TIME;
    signals.position = 0.1;
    signals.floor_sensor = -1;
    timestamp = std::chrono::system_clock::now();
}


Elevator::Elevator(int id, double motor_speed, int num_floors):
Elevator(id, num_floors)
{
    signals.motor.speed = motor_speed;
}

Elevator::~Elevator() {
    //empty
}

void Elevator::run(void) {
    running = true;

    while(ok()) {
        update();
        //std::cout << signals.position << std::endl;
        usleep(20);
    }
}

bool Elevator::ok() {
    return running;
}

void Elevator::stop() {
    running = false;
}


void Elevator::update(void) {
    updateTime();
    //Calculate the increment in the elevator position.
    if(signals.motor.direction != Direction::STOP) {
        updatePosition();
    }
    //Update all sensors
    updateSignals();
    
}


void Elevator::updatePosition(void) {
    double new_pos = signals.position + static_cast<int>(signals.motor.direction) * signals.motor.speed * dt;
    signals.position.store(new_pos);
    //Be careful to check for boundry conditions.
    if(signals.position > TRACK_LENGTH) {
        signals.position = TRACK_LENGTH;
    }
    if(signals.position < 0) {
        signals.position = 0;
    }
}


void Elevator::updateTime(void) {
    //Get the current time and calculate the dt.
    t = std::chrono::system_clock::now();
    dt = (t - timestamp).count() / static_cast<double>(1000000000);
    timestamp = t;
}


void Elevator::updateSignals(void) {
    //Iterate over all floors and update the floor indicator
    //if the elevator is close enough to a floor. 
    for(int i = 0; i < 4; ++i) {
        if(std::abs(signals.position - (i * TRACK_LENGTH / 3.0)) < 0.01) {
            signals.floor_sensor = i + 1;
            break;
        } else {
            signals.floor_sensor = -1;
        }
    }
    std::remove_if(events.begin(), events.end(), [=](ButtonPress &b){
            return b.poll();
        }
    );
}

int Elevator::getId(void) {
    return _id;
}


int Elevator::getSignal(const command_t &cmd) {
    switch(cmd.signal) {
        case CommandSignal::BUTTON:
            return signals.buttons[cmd.floor][cmd.selector];
            break;
        case CommandSignal::LIGHT:
            return signals.lights[cmd.floor];
            break;
        case CommandSignal::STOP:
            return signals.stop;
            break;
        case CommandSignal::OBSTRUCTION:
            return signals.obstruction;
            break;
        case CommandSignal::FLOOR_SENSOR:
            return signals.floor_sensor;
            break;
        case CommandSignal::MOTOR:
            return static_cast<int>(signals.motor.direction);
            break;
        default:
            return 0;
            break;
    }
}

void Elevator::setSignal(const command_t &cmd) {
    switch(cmd.signal) {
        case CommandSignal::BUTTON:
            if(cmd.selector == 3) {
                events.emplace_back(
                    std::chrono::system_clock::now(),
                    &signals.stop,
                    std::chrono::milliseconds(1000)
                );
            } else if(cmd.selector == 4) {
                events.emplace_back(
                    std::chrono::system_clock::now(),
                    &signals.obstruction,
                    std::chrono::milliseconds(1000)
                );
            } else {
                events.emplace_back(
                    std::chrono::system_clock::now(),
                    &signals.buttons[cmd.floor][cmd.selector],
                    std::chrono::milliseconds(100)
                );
            }
            break;
        case CommandSignal::LIGHT:
            signals.lights[cmd.floor] = cmd.value;
            break;
        case CommandSignal::STOP:
            signals.stop = cmd.value;
            break;
        case CommandSignal::OBSTRUCTION:
            signals.obstruction = cmd.value;
            break;
        case CommandSignal::MOTOR:
            signals.motor.direction = static_cast<Direction>(cmd.value);
            break;
        default:
            break;
    }
}

command_t Elevator::executeCommand(const command_t &cmd) {
    command_t ret = cmd;
    if(cmd.cmd == CommandType::GET) {
        ret.value = getSignal(cmd);
    } else {
        setSignal(cmd);
    }
    ret.position = signals.position;
    return ret;
}

double Elevator::getElevatorPosition(void) {
    return signals.position;
}