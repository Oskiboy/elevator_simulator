#include <cmath>
#include <iostream>
#include <thread>

#include <elevator.hpp>

#include <unistd.h>

using namespace elev;

Elevator::Elevator(int id, int num_floors):
        _id(id), 
        pos(0.1), 
        _motor_speed(TRACK_LENGTH / TRACK_TIME),
        _num_floors(num_floors)
{
    signals.buttons = std::unique_ptr<int[][3]>(new int[_num_floors][3]);
    signals.lights = std::unique_ptr<int[]>(new int[_num_floors]);
    nullSignals();
    timestamp = std::chrono::system_clock::now();
}

void Elevator::nullSignals() {
    for(int i = 0; i < _num_floors; ++i) {
        signals.buttons[i][0] = 0;
        signals.buttons[i][1] = 0;
        signals.buttons[i][2] = 0;
        signals.lights[i] = 0;
    }

    signals.obstruction = 0;
    
    signals.stop = 0;
    
    signals.floor_sensor = -1;
    
    signals.position = 0;

    signals.motor.direction = Direction::STOP;
    signals.motor.speed = 0;

}

Elevator::Elevator(int id, double motor_speed, int num_floors):
Elevator(id, num_floors)
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
        update();
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
    if(signals.motor.direction != Direction::STOP) {
        updatePosition();
    }
    //Update all sensors
    updateSignals();
    
}


void Elevator::updatePosition(void) {
    std::lock_guard<std::mutex> lock(sig_m);
    signals.position += static_cast<int>(signals.motor.direction) * signals.motor.speed * dt;

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
        if(std::abs(pos - (i * TRACK_LENGTH / 3.0)) < 0.01) {
            sig_m.lock();
            signals.floor_sensor = i + 1;
            sig_m.unlock();
            break;
        } else {
            sig_m.lock();
            signals.floor_sensor = -1;
            sig_m.unlock();
        }
    }
}

int Elevator::getId(void) {
    std::lock_guard<std::mutex> lock(sig_m);
    return _id;
}


void Elevator::setMotorDirection(Direction d) {
    std::lock_guard<std::mutex> lock(sig_m);
    signals.motor.direction = d;
}

void Elevator::startMotor(void) {
    std::lock_guard<std::mutex>lock(sig_m);
    signals.motor.direction = _last_dir;
}

void Elevator::stopMotor(void) {
    std::lock_guard<std::mutex>lock(sig_m);
    _last_dir = signals.motor.direction;
    signals.motor.direction = Direction::STOP;
}

int Elevator::getSignal(const command_t &cmd) {
    std::lock_guard<std::mutex>lock(sig_m);
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
    std::lock_guard<std::mutex>lock(sig_m);
    switch(cmd.signal) {
        case CommandSignal::BUTTON:
            signals.buttons[cmd.floor][cmd.selector] = cmd.value;
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
    command_t ret{};
    if(cmd.cmd == CommandType::GET) {
        ret.value = getSignal(cmd);
    } else {
        setSignal(cmd);
    }
    return ret;
}

double Elevator::getElevatorPosition(void) {
    std::lock_guard<std::mutex>lock(sig_m);
    return pos;
}