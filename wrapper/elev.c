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


#ifdef TTK_SIM
static void write_to_socket(char* cmd) {
    send(sockfd, cmd, sizeof(cmd), 0);
    bzero(&cmd, sizeof(cmd));
    read(sockfd, cmd, sizeof(cmd));
}
#else
static int write_to_socket(char* cmd, int value) {
    char buffer[256];
    char str[50];
    bzero(&buffer, sizeof(buffer));
    strcpy(buffer, cmd);
    strcat(buffer, " ");
    sprintf(str, "%d", value);
    strcat(buffer, str);
    printf("%s", buffer);
    send(sockfd, buffer, sizeof(buffer), 0);
    bzero(&buffer, sizeof(buffer));
    read(sockfd, buffer, sizeof(buffer));
    return atoi(buffer);
}
#endif

int elev_init(void) {

    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    memset(&servaddr, '0', sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
  
    // connect the client socket to server socket 
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server..\n"); 

    return 1;
}

void elev_set_motor_direction(elev_motor_direction_t dirn) {
    #ifdef TTK_SIM
    char cmd[4] = {1, (char)dirn, 0, 0};
    write_to_socket(cmd);
    #else
    write_to_socket("set MOTOR_DIR", (int)dirn);
    #endif
}

void elev_set_door_open_lamp(int value) {
    #ifdef TTK_SIM
    char cmd[4] = {4 , (char)value, 0, 0};
    write_to_socket(cmd);
    #else
    write_to_socket("set DOOR_LAMP", value);
    #endif
}

int elev_get_obstruction_signal(void) {
    #ifdef TTK_SIM
    char cmd[4] = {9, 0, 0, 0};
    write_to_socket(cmd);
    return (int)cmd[1];
    #else
    return write_to_socket("get OBSTRUCTION", 0);
    #endif
}

int elev_get_stop_signal(void) {
    #ifdef TTK_SIM
    char cmd[4] = {8, 0, 0, 0};
    write_to_socket(cmd);
    return (int)cmd[1];
    #else
    return write_to_socket("get STOP_BUTTON", 0);
    #endif
}

void elev_set_stop_lamp(int value) {
    #ifdef TTK_SIM
    char cmd[4] = {5, (char)value, 0, 0};
    write_to_socket(cmd);
    #else
    write_to_socket("set LIGHT_STOP", value);
    #endif
}

int elev_get_floor_sensor_signal(void) {  
    #ifdef TTK_SIM
    char cmd[4] = {7, 0, 0, 0};
    write_to_socket(cmd);
    return (int)cmd[1];
    #else  
    return write_to_socket("get FLOOR_SENSOR", 0);
    #endif
}

void elev_set_floor_indicator(int floor) {
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    #ifdef TTK_SIM
    char cmd[4] = {3, (char)floor, 0, 0};
    write_to_socket(cmd);
    #else
    switch (floor)
    {
        case 1:
            write_to_socket("set LIGHT_FLOOR_1", 1);
            break;
        case 2:
            write_to_socket("set LIGHT_FLOOR_2", 1);
            break;
        case 3:
            write_to_socket("set LIGHT_FLOOR_3", 1);
            break;
        case 4:
            write_to_socket("set LIGHT_FLOOR_3", 1);
            break;
        default:
            break;
    }
    #endif
}

int elev_get_button_signal(elev_button_type_t button, int floor) {
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    assert(!(button == BUTTON_CALL_UP && floor == N_FLOORS - 1));
    assert(!(button == BUTTON_CALL_DOWN && floor == 0));
    assert(button == BUTTON_CALL_UP || button == BUTTON_CALL_DOWN || button == BUTTON_COMMAND);
    #ifdef TTK_SIM
    char cmd[4] = {6, (char)button, (char)floor, 0};
    write_to_socket(cmd);
    return (int)cmd[1];
    #else
    switch (button)
    {
        case BUTTON_CALL_UP:
            switch (floor)
            {
                case 1:
                    return write_to_socket("get BUTTON_UP_1", 0);
                    break;
                case 2:
                    return write_to_socket("get BUTTON_UP_2", 0);
                    break;
                case 3:
                    return write_to_socket("get BUTTON_UP_3", 0);
                    break;
                default:
                    return -1;
                    break;
            }
            break;
        case BUTTON_CALL_DOWN:
            switch (floor)
            {
                case 2:
                    return write_to_socket("get BUTTON_DOWN_2", 0);
                    break;
                case 3:
                    return write_to_socket("get BUTTON_DOWN_3", 0);
                    break;
                case 4:
                    return write_to_socket("get BUTTON_DOWN_4", 0);
                    break;
                default:
                    return -1;
                    break;
            }
            break;
        case BUTTON_COMMAND:
            switch (floor)
            {
                case 1:
                    return write_to_socket("get BUTTON_COMMAND_1", 0);
                    break;
                case 2:
                    return write_to_socket("get BUTTON_COMMAND_2", 0);
                    break;
                case 3:
                    return write_to_socket("get BUTTON_COMMAND_3", 0);
                    break;
                case 4:
                    return write_to_socket("get BUTTON_COMMAND_4", 0);
                    break;
                default:
                    return -1;
                    break;
            }
            break;
        default:
            break;
    }
    #endif

}

void elev_set_button_lamp(elev_button_type_t button, int floor, int value) {
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    assert(!(button == BUTTON_CALL_UP && floor == N_FLOORS - 1));
    assert(!(button == BUTTON_CALL_DOWN && floor == 0));
    assert(button == BUTTON_CALL_UP || button == BUTTON_CALL_DOWN || button == BUTTON_COMMAND);
    #ifdef TTK_SIM
    char cmd[4] = {2, (char)button, (char)floor, (char)value};
    write_to_socket(cmd);
    #else
    switch (button)
    {
         case BUTTON_CALL_UP:
            switch (floor)
            {
                case 1:
                    write_to_socket("set BUTTON_UP_1", value);
                    break;
                case 2:
                    write_to_socket("set BUTTON_UP_2", value);
                    break;
                case 3:
                    write_to_socket("set BUTTON_UP_3", value);
                    break;
                default:
                    break;
            }
            break;
        case BUTTON_CALL_DOWN:
            switch (floor)
            {
                case 2:
                    write_to_socket("set BUTTON_DOWN_2", value);
                    break;
                case 3:
                    write_to_socket("set BUTTON_DOWN_3", value);
                    break;
                case 4:
                     write_to_socket("set BUTTON_DOWN_4", value);
                    break;
                default:
                    
                    break;
            }
            break;
        case BUTTON_COMMAND:
            switch (floor)
            {
                case 1:
                     write_to_socket("set BUTTON_COMMAND_1", value);
                    break;
                case 2:
                     write_to_socket("set BUTTON_COMMAND_2", value);
                    break;
                case 3:
                     write_to_socket("set BUTTON_COMMAND_3", value);
                    break;
                case 4:
                     write_to_socket("set BUTTON_COMMAND_4", value);
                    break;
                default:
                    
                    break;
            }
            break;
        default:
            break;
    }
    #endif
}
