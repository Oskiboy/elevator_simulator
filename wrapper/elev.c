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
#include <arpa/inet.h>
#include <errno.h>

// Number of signals and lamps on a per-floor basis (excl sensor)
#define PORT 6060
#define N_BUTTONS 3

static int sockfd;
static struct sockaddr_in servaddr;
static unsigned char cmd[4];

int write_to_socket(unsigned char _cmd[4]) {
    if(_cmd[0] == 255) {
        return 0;
    }
    
    int ret = 0;
    elev_init();
    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf(" ERROR - Socket creation failed...\n"); 
        exit(0); 
    }

    //Connect to the server.
    ret = connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (ret < 0) { 
        printf("ERROR - Connection to the server failed... ERROR: %d\n", errno); 
        exit(0); 
    }

    //Send our command to the server.
    ret = send(sockfd, _cmd, 4 * sizeof(_cmd[0]), 0);
    if(ret < 0) {
        printf("ERROR - could not read from socket\n");
    }

    if(_cmd[0] > 5) {
        char buffer[4];
        bzero(&buffer, sizeof(buffer));
        read(sockfd, buffer, sizeof(buffer));
        close(sockfd);
        return buffer[1];
    }

    close(sockfd);
    return 0;
}


int elev_init(void) {   
    // assign IP, PORT 
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    return 1;
}

void elev_set_motor_direction(elev_motor_direction_t dirn) {
    cmd[0] = 1;
    cmd[1] = dirn;
    cmd[2] = 0;
    cmd[3] = 0;
    write_to_socket(cmd);
}

void elev_set_door_open_lamp(int value) {
    cmd[0] = 4;
    cmd[1] = value;
    cmd[2] = 0;
    cmd[3] = 0;
    write_to_socket(cmd);
}

int elev_get_obstruction_signal(void) {
    cmd[0] = 9;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;
    return write_to_socket(cmd);
}

int elev_get_stop_signal(void) {
    cmd[0] = 8;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;
    return write_to_socket(cmd);
}

void elev_set_stop_lamp(int value) {
    cmd[0] = 5;
    cmd[1] = value;
    cmd[2] = 0;
    cmd[3] = 0;
    write_to_socket(cmd);
}

int elev_get_floor_sensor_signal(void) {  
    cmd[0] = 7;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;
    return write_to_socket(cmd);
}

void elev_set_floor_indicator(int floor) {
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    cmd[0] = 3;
    cmd[1] = floor;
    cmd[2] = 0;
    cmd[3] = 0;
    write_to_socket(cmd);
}

int elev_get_button_signal(elev_button_type_t button, int floor) {
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    assert(!(button == BUTTON_CALL_UP && floor == N_FLOORS - 1));
    assert(!(button == BUTTON_CALL_DOWN && floor == 0));
    assert(button == BUTTON_CALL_UP || button == BUTTON_CALL_DOWN || button == BUTTON_COMMAND);
    cmd[0] = 6;
    cmd[1] = button;
    cmd[2] = floor;
    cmd[3] = 0;
    return write_to_socket(cmd);
}

void elev_set_button_lamp(elev_button_type_t button, int floor, int value) {
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    assert(!(button == BUTTON_CALL_UP && floor == N_FLOORS - 1));
    assert(!(button == BUTTON_CALL_DOWN && floor == 0));
    assert(button == BUTTON_CALL_UP || button == BUTTON_CALL_DOWN || button == BUTTON_COMMAND);
    cmd[0] = 2;
    cmd[1] = button;
    cmd[2] = floor;
    cmd[3] = value;
    write_to_socket(cmd);
}
