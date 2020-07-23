/*  This file contains declaration of functions related to client connetion.
    A client connection would have a ClientConnection struct to store related
    information, such as control socket, data connection socket and so on.*/

#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#define MAX_SIZE 1024
#define WHITESPACE " "
#define REQUEST_SYS_ALLOCATED_PORT 0
#define MAX_CMD_TOKENDS 5

#define CLIENT_USER_NAME "anonymous"
#define TYPE_A 1
#define TYEP_I 0

#define DATA_SOCKET_TIMEOUT_S 10
#define RSP_MSG_LIMIT 1024
#define CMD_MSG_LIMIT 1024
#define RSP_MSG_FORMAT "%d %s\r\n"

typedef struct clinet_connection_t
{
    int connection_id;
    int control_socket;
    int pasv_socket;
    int login_flag;
    int terminate_flag;
    char* root_dir;
    int type; // 1 means A (default), 0 means I
    char *next_cmd_tokens[MAX_CMD_TOKENDS];
    int next_cmd_size;
} clinet_connection_t;

/*
 * Function:  initialize_client_connection 
 * --------------------
 * create a new clinet_connection_t pointer and set initial state of client connection
 *
 *  connection_socket: file descriptor of new accepted client control socket
 *
 *  returns: new clinet_connection_t pointer with proper setted inital states
 */
clinet_connection_t* initialize_client_connection(int connection_socket);

/*
 * Function:  handle_client_connection 
 * --------------------
 * handle client connection which pointed by pointer passed by client_connection_ptr argument,
 *      general steps are send greeting msg first and then until terminate flag set, keep read incomming
 *      command and excute command
 *
 *  client_connection_ptr: pointer for a clinet connection struct
 *
 *  returns: no return value
 */
void handle_client_connection(clinet_connection_t* client_connection_ptr);


/*
 * Function:  prepare_client_connection 
 * --------------------
 * Before continue to next command comming from client connection, clean up previous excution statas
 *
 *  client_connection_ptr: pointer for a clinet connection struct
 *
 *  returns: no return value
 */
void prepare_client_connection(clinet_connection_t* client_connection_ptr);

/*
 * Function:  clean_up_client_connection 
 * --------------------
 * Clean up client connection, including close all sockets related to this connection 
 *      and free all mem used by this connetion.
 *
 *  client_connection_ptr: pointer for a clinet connection struct
 *
 *  returns: no return value
 */
void clean_up_client_connection(clinet_connection_t*  client_connection_ptr);

/*
 * Function:  handle_next_command 
 * --------------------
 * handle next command comming from conncetion, read and then do execution.
 *
 *  client_connection_ptr: pointer for a clinet connection struct
 *
 *  returns: no return value
 */
void handle_next_command(clinet_connection_t*  client_connection_ptr);

/*
 * Function:  read_next_command 
 * --------------------
 * from connection control socket read next incomming command msg, if read fail or no msg set terminate flag and return,
 *      otherwise trim white space from raw command msg, and split by whitespace into tokens, store next incomming command
 *      in correct field inside client connection struct. 
 *
 *  client_connection_ptr: pointer for a clinet connection struct
 *
 *  returns: no return value
 */
void read_next_command(clinet_connection_t*  client_connection_ptr);

/*
 * Function:  excute_next_command 
 * --------------------
 * excute next command stored in client connection struct, if command size is 0 skip it, 
 *      ohterwise based on first command token call corespond command excution funciton to perfom correct command action.
 *
 *  client_connection_ptr: pointer for a clinet connection struct
 *
 *  returns: no return value
 */
void excute_next_command(clinet_connection_t*  client_connection_ptr);

/*
 * Function:  verify_login_state 
 * --------------------
 * check connection login state, if not login would send login request
 *
 *  client_connection_ptr: pointer for a clinet connection struct
 *
 *  returns: 1 for logged, 0 for not logged
 */
int verify_login_state(clinet_connection_t*  client_connection_ptr);

/*
 * Function:  check_cmd_size 
 * --------------------
 * check next command size with target size, if size not mathed would send syntax error response
 *
 *  client_connection_ptr: pointer for a clinet connection struct
 *  size                 : target size
 *
 *  returns: 1 for size matched, 0 for not matched
 */
int check_cmd_size(clinet_connection_t*  client_connection_ptr, int size);

/*
 * Function:  check_pasv_mode 
 * --------------------
 * check does pasve socket open for client connection, if not send pasv command request
 *
 *  client_connection_ptr: pointer for a clinet connection struct
 *
 *  returns: 1 for open, 0 for close
 */
int check_pasv_mode(clinet_connection_t*  client_connection_ptr);

#endif