#include <sys/types.h>
#include "connection.h"
#include "sock.h"
#include "command.h"
#include "utils.h"

static int id = 0;

clinet_connection_t* initialize_client_connection(int connection_socket)
{
    clinet_connection_t* client_connection_ptr = (clinet_connection_t*)malloc(sizeof(clinet_connection_t));
    client_connection_ptr->connection_id = id++;
    client_connection_ptr->control_socket = connection_socket;
    client_connection_ptr->pasv_socket = -1;
    client_connection_ptr->login_flag = 0;
    client_connection_ptr->terminate_flag = 0;
    client_connection_ptr->root_dir = (char *)malloc(sizeof(char) * (MAX_SIZE + 1));
    if (getcwd(client_connection_ptr->root_dir, MAX_SIZE) == NULL)
    {
        printf("Clinet connection id: %d - Failed get root work dir.\n", client_connection_ptr->connection_id);
        client_connection_ptr->terminate_flag = 1;
    }
    client_connection_ptr->type = TYPE_A;
    client_connection_ptr->next_cmd_size = 0;
    // client_connection_ptr->next_cmd_tokens = (char**)malloc(sizeof(char*)*(MAX_CMD_TOKENDS + 1));
    return client_connection_ptr;
}

void handle_client_connection(clinet_connection_t* client_connection_ptr)
{
    // send "220 welcom" greeting
    greet(client_connection_ptr);

    // while loop to handle command
    while (!client_connection_ptr->terminate_flag)
    {
        // // handle next command
        prepare_client_connection(client_connection_ptr);
        handle_next_command(client_connection_ptr);
    }
}

void prepare_client_connection(clinet_connection_t* client_connection_ptr)
{
    // prepare client for next command
    for (int i = 0; i < client_connection_ptr->next_cmd_size; ++i)
        free((client_connection_ptr->next_cmd_tokens)[i]);
    // if (client_connection_ptr->next_cmd_tokens != NULL)
    //     free_double_ptr(client_connection_ptr->next_cmd_tokens, client_connection_ptr->next_cmd_size);
    client_connection_ptr->next_cmd_size = 0;
    printf("Clinet connection id: %d - Ready for next command.\n", client_connection_ptr->connection_id);
}

void clean_up_client_connection(clinet_connection_t* client_connection_ptr)
{
    int connection_id = client_connection_ptr->connection_id;
    // close control and any data socket
    close(client_connection_ptr->control_socket);
    close(client_connection_ptr->pasv_socket);
    // free ptr mem
    if (client_connection_ptr->root_dir != NULL)
        free(client_connection_ptr->root_dir);
    if (client_connection_ptr->next_cmd_size != 0) {
        for (int i = 0; i < client_connection_ptr->next_cmd_size; ++i)
            free((client_connection_ptr->next_cmd_tokens)[i]);
    }
    if (client_connection_ptr != NULL)
    free(client_connection_ptr);
    printf("Clinet connection id: %d  - Clean up connection.\n", connection_id);
}


void handle_next_command(clinet_connection_t* client_connection_ptr)
{
    read_next_command(client_connection_ptr);
    excute_next_command(client_connection_ptr);
}


void read_next_command(clinet_connection_t* client_connection_ptr)
{
    // read next incoming raw command string
    char recv_str[CMD_MSG_LIMIT];
    bzero(&recv_str, sizeof(recv_str));
    int recv_len = read_from_socket(client_connection_ptr->control_socket, recv_str, CMD_MSG_LIMIT);
    if (recv_len <= 0)
    {
        printf("Clinet connection id: %d - Failed read from socket.\n", client_connection_ptr->connection_id);
        client_connection_ptr->terminate_flag = 1;
        return;
    }

    printf("Clinet connection id: %d - Read: recieve command string: %s", client_connection_ptr->connection_id, recv_str);

    // trim wihtespce and tokenize
    char cmd_str[CMD_MSG_LIMIT];
    bzero(&cmd_str, sizeof(recv_str));
    trim_whitespace(cmd_str, recv_len, recv_str);
    int cmd_size = str_split(client_connection_ptr->next_cmd_tokens, cmd_str, WHITESPACE, MAX_CMD_TOKENDS);
    client_connection_ptr->next_cmd_size = cmd_size;
    printf("Clinet connection id: %d - Read: Tokenized command string get %d tokens.\n", client_connection_ptr->connection_id, cmd_size);
}


void excute_next_command(clinet_connection_t* client_connection_ptr)
{
    int cmd_size = client_connection_ptr->next_cmd_size;

    // skip empty cmd
    if (cmd_size == 0) {
        printf("Clinet connection id: %d - Excute: empty command, skipping.\n", client_connection_ptr->connection_id);
        return;
    }

    // check cmd token size
    char *cmd = (client_connection_ptr->next_cmd_tokens)[0];

    printf("Clinet connection id: %d - Excute: %s - starting.\n", client_connection_ptr->connection_id, cmd);

    if (strcasecmp(cmd, "USER") == 0)
    {
        user(client_connection_ptr);
    }
    else if (strcasecmp(cmd, "QUIT") == 0)
    {
        quit(client_connection_ptr);
    }
    else if (strcasecmp(cmd, "CWD") == 0)
    {
        cwd(client_connection_ptr);
    }
    else if (strcasecmp(cmd, "CDUP") == 0)
    {
        cdup(client_connection_ptr);
    }
    else if (strcasecmp(cmd, "TYPE") == 0)
    {
        type(client_connection_ptr);
    }
    else if (strcasecmp(cmd, "MODE") == 0)
    {
        mode(client_connection_ptr);
    }
    else if (strcasecmp(cmd, "STRU") == 0)
    {
        stru(client_connection_ptr);
    }
    else if (strcasecmp(cmd, "RETR") == 0)
    {
        retr(client_connection_ptr);
    }
    else if (strcasecmp(cmd, "PASV") == 0)
    {
        pasv(client_connection_ptr);
    }
    else if (strcasecmp(cmd, "NLST") == 0)
    {
        nlst(client_connection_ptr);
    }
    else /* default: */
    {
        unknown(client_connection_ptr);
    }
    printf("Clinet connection id: %d - Excute: %s - end.\n", client_connection_ptr->connection_id, cmd);
}

int verify_login_state(clinet_connection_t* client_connection_ptr)
{
    if (client_connection_ptr->login_flag)
        return 1;

    char rsp[RSP_MSG_LIMIT];
    snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, NOT_LOGGED, "Please login with USER and PASS.");
    send_to_socket(client_connection_ptr->control_socket, rsp, strlen(rsp));
    return 0;
}

int check_cmd_size(clinet_connection_t* client_connection_ptr, int size)
{
    if (client_connection_ptr->next_cmd_size == size)
        return 1;

    char rsp[RSP_MSG_LIMIT];
    snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, WRONG_ARGUMENT, "Error in parameters or argument.");
    send_to_socket(client_connection_ptr->control_socket, rsp, strlen(rsp));
    return 0;
}

int check_pasv_mode(clinet_connection_t* client_connection_ptr)
{
    if (client_connection_ptr->pasv_socket != -1)
        return 1;

    char rsp[RSP_MSG_LIMIT];
    snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, NO_DATA_CONNECTION, "Use PASV first.");
    send_to_socket(client_connection_ptr->control_socket, rsp, strlen(rsp));
    return 0;
}
