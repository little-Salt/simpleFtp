/*  This file contain all declaration of supported ftp server
    command functions and related constant value.*/

#ifndef _COMMAND_H_
#define _COMMAND_H_

#define MINIMUM_CMD_SIZE 1

// response code
#define FILE_STATUS_OK 150 

#define REQUEST_SUCCESS 200
#define SERVICE_READY_SUCCESS 220
#define SERVICE_CLOSE_SUCCESS 221
#define CLOSE_DATA_CONNECTION 226
#define ENTER_PASSIVE_MODE 227
#define USER_LOGGED_SUCCESS 230
#define REQUESTED_FILE_ACTION_SUCCESS 250

#define NO_DATA_CONNECTION 425

#define SYNTAX_ERROR 500
#define WRONG_ARGUMENT 501
#define PARAMETER_NOT_SUPPORTED 504
#define NOT_LOGGED 530
#define REQUESTED_FILE_ACTION_FAIL 550

void greet(clinet_connection_t* client_connection_ptr);

void user(clinet_connection_t* client_connection_ptr);

void quit(clinet_connection_t* client_connection_ptr);

void cwd(clinet_connection_t* client_connection_ptr);

void cdup(clinet_connection_t* client_connection_ptr);

void type(clinet_connection_t* client_connection_ptr);

void mode(clinet_connection_t* client_connection_ptr);

void stru(clinet_connection_t* client_connection_ptr);

void retr(clinet_connection_t* client_connection_ptr);

void pasv(clinet_connection_t* client_connection_ptr);

void nlst(clinet_connection_t* client_connection_ptr);

void unknown(clinet_connection_t* client_connection_ptr);

#endif