// Wrapper for libComedi Elevator control.
// These functions provides an interface to the elevators in the real time lab
//
// 2007, Martin Korsgaard


#include "channels.h"
#include "elev.h"

#include <assert.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "arpa/inet.h"

// Number of signals and lamps on a per-floor basis (excl sensor)
#define PORT 6060
#define N_BUTTONS 3

static int sockfd;
static struct sockaddr_in servaddr;


int write_to_socket(char* cmd) {
    send(sockfd, cmd, 4 * sizeof(cmd[0]), 0);
    if((int)cmd[0] > 5) {
        char buffer[4];
        bzero(&buffer, 4*sizeof(buffer[0]));
        read(sockfd, buffer, 4 * sizeof(buffer[0]));
        printf("%d%d%d%d", buffer[0],buffer[1],buffer[2],buffer[3]);
        return buffer[1];
    }
    return 0;
}


int elev_init(void) {

    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf(" ERROR - Socket creation failed...\n"); 
        exit(0); 
    }

    // assign IP, PORT 
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
  
    // connect the client socket to server socket 
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("ERROR - Connection to the server failed...\n"); 
        exit(0); 
    }
    return 1;
}

void elev_set_motor_direction(elev_motor_direction_t dirn) {
    char cmd[4] = {1, dirn, 0, 0};
    write_to_socket(cmd);
}

void elev_set_door_open_lamp(int value) {
    char cmd[4] = {4 , value, 0, 0};
    write_to_socket(cmd);
}

int elev_get_obstruction_signal(void) {
    char cmd[4] = {9, 0, 0, 0};
    return write_to_socket(cmd);
}

int elev_get_stop_signal(void) {
    char cmd[4] = {8, 0, 0, 0};
    return write_to_socket(cmd);
}

void elev_set_stop_lamp(int value) {
    char cmd[4] = {5, value, 0, 0};
    write_to_socket(cmd);
}

int elev_get_floor_sensor_signal(void) {  
    char cmd[4] = {7, 0, 0, 0};
    return write_to_socket(cmd);

}

void elev_set_floor_indicator(int floor) {
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    char cmd[4] = {3, floor, 0, 0};
    write_to_socket(cmd);
}

int elev_get_button_signal(elev_button_type_t button, int floor) {
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    assert(!(button == BUTTON_CALL_UP && floor == N_FLOORS - 1));
    assert(!(button == BUTTON_CALL_DOWN && floor == 0));
    assert(button == BUTTON_CALL_UP || button == BUTTON_CALL_DOWN || button == BUTTON_COMMAND);
    char cmd[4] = {6, button, floor, 0};
    return write_to_socket(cmd);
}

void elev_set_button_lamp(elev_button_type_t button, int floor, int value) {
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    assert(!(button == BUTTON_CALL_UP && floor == N_FLOORS - 1));
    assert(!(button == BUTTON_CALL_DOWN && floor == 0));
    assert(button == BUTTON_CALL_UP || button == BUTTON_CALL_DOWN || button == BUTTON_COMMAND);
    char cmd[4] = {2, button, floor, value};
    write_to_socket(cmd);
}
