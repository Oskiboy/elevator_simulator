#include <cmath>
#include <iostream>
#include <thread>

#include <elevator.hpp>

#include <unistd.h>

using namespace elev;

Elevator::Elevator(int id):
        _id(id), 
        pos(0.1), 
        _motor_speed(TRACK_LENGTH / TRACK_TIME)
{
    for(auto it = signals.begin(); it != signals.end(); ++it) {
        *it = 0;
    }
    timestamp = std::chrono::system_clock::now();
}

Elevator::Elevator(int id, double motor_speed):
Elevator(id)
{
    _motor_speed = motor_speed;
}

Elevator::~Elevator() {
    //empty
}

void Elevator::run(void) {
    ok_mtx.lock();
    running = true;
    ok_mtx.unlock();

    while(ok()) {
        m.lock();
        update();
        m.unlock();
        usleep(20);
    }
}

bool Elevator::ok() {
    std::lock_guard<std::mutex> lock(ok_mtx);
    return running;
}

void Elevator::stop() {
    std::lock_guard<std::mutex> lock(ok_mtx);
    running = false;
}


void Elevator::update(void) {
    updateTime();
    //Calculate the increment in the elevator position.
    if(signals[static_cast<int>(Signals::MOTOR_DIR)]) {
        updatePosition();
    }
    //Update all sensors
    updateSignals();
    
}

void Elevator::updatePosition(void) {
    pos += signals[static_cast<int>(Signals::MOTOR_DIR)] * _motor_speed * dt;

    //Be careful to check for boundry conditions.
    if(pos > TRACK_LENGTH) {
        pos = TRACK_LENGTH;
    }
    if(pos < 0) {
        pos = 0;
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
        if(std::abs(pos - (i * TRACK_LENGTH / 3.0)) < 0.01) {
            signals[static_cast<int>(Signals::FLOOR_SENSOR)] = i + 1;
            break;
        } else {
            signals[static_cast<int>(Signals::FLOOR_SENSOR)] = -1;
        }
    }
}

int Elevator::getId(void) {
    std::lock_guard<std::mutex> lock(m);
    return _id;
}

void Elevator::setMotorDirection(Direction d) {
    std::lock_guard<std::mutex> lock(m);
    signals[static_cast<int>(Signals::MOTOR_DIR)] = static_cast<int>(d);
}

void Elevator::startMotor(void) {
    std::lock_guard<std::mutex>lock(m);
    signals[static_cast<int>(Signals::MOTOR_DIR)] = _last_dir;
}

void Elevator::stopMotor(void) {
    std::lock_guard<std::mutex>lock(m);
    _last_dir = signals[static_cast<int>(Signals::MOTOR_DIR)]; 
    signals[static_cast<int>(Signals::MOTOR_DIR)] = 0;
}

int Elevator::setSignal(Signals sig, int value) {
    std::lock_guard<std::mutex> lock(m);
    signals[static_cast<int>(sig)] = value;
    return signals[static_cast<int>(sig)];
}

int Elevator::getSignal(Signals sig) {
    std::lock_guard<std::mutex> lock(m);
    return signals[static_cast<int>(sig)];
}

double Elevator::getElevatorPosition(void) {
    std::lock_guard<std::mutex>lock(m);
    return pos;
}