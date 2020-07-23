#ifndef _SOCK_H_
#define _SOCK_H_

/*
 * Function:  create_listen_socket 
 * --------------------
 * create a socket and bind to specific port, and listen on this new socket
 *
 *  port: port number specific to be used
 *
 *  returns: 0 for success, -1 for error happend in any step
 */
int create_listen_socket(int port);

/*
 * Function:  create_connection_socket 
 * --------------------
 * Accept next connection from listen socket.
 *
 *  listen_socket: file descriptor for socket whcih waiting for connection
 *
 *  returns: any value greater than 0 for success, -1 for error happend in any step
 */
int create_connection_socket(int listen_socket);

/*
 * Function:  create_data_socket 
 * --------------------
 * Accept next connection from listen socket and timeout after wait for time out period.
 *
 *  listen_socket: file descriptor for socket whcih waiting for connection
 *  time_out     : time out interval in Second
 *
 *  returns: any value greater than 0 for success, -1 for error happend in any step
 */
int create_data_socket(int listen_socket, long time_out);

/*
 * Function:  send_to_socket 
 * --------------------
 * send msg with specific length by socket
 * 
 *  socket: file descriptor for socket whcih using for send msg
 *  buffer     : msg pointer
 *  length     : msg length
 *
 *  returns: On success, these calls return the number of bytes sent.  On error, -1 is returned
 */
int send_to_socket(int socket, char *buffer, int length);

/*
 * Function:  read_from_socket 
 * --------------------
 * read msg from socket and write up to curten length to a string pointer.
 * 
 *  socket: file descriptor for socket whcih using for send msg
 *  buffer     : pointer for stor msg
 *  length     : max lenght write to string pointer
 *
 *  returns: On success, these calls return the number of bytes received.  On error, -1 is returned.
 *      When a stream socket peer has performed an orderly shutdown, the return value will be 0.
 */
int read_from_socket(int socket, char * buffer, int length);

#endif