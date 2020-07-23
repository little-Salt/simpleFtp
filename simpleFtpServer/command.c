/*  This file contain actual implementation of supported ftp server
    command functions.*/
#include "connection.h"
#include "command.h"
#include "sock.h"
#include "utils.h"
#include "dir.h"

void greet(clinet_connection_t* client_connection_ptr)
{
    char rsp[RSP_MSG_LIMIT];
    snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, SERVICE_READY_SUCCESS, "Welcom to simple ftp service.");
    send_to_socket(client_connection_ptr->control_socket, rsp, strlen(rsp));
}

void user(clinet_connection_t* client_connection_ptr)
{
    if (!check_cmd_size(client_connection_ptr, 2))
        return;

    char rsp[RSP_MSG_LIMIT];
    if (client_connection_ptr->login_flag)
    {
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, NOT_LOGGED, "Already Logged.");
    }
    else
    {
        char *user = (client_connection_ptr->next_cmd_tokens)[1];
        if (strcmp(user, CLIENT_USER_NAME) == 0)
        {
            snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, USER_LOGGED_SUCCESS, "Login successful.");
            client_connection_ptr->login_flag = 1;
        }
        else
        {
            snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, NOT_LOGGED, "This FTP server is accepted anonymous user only.");
        }
    }
    send_to_socket(client_connection_ptr->control_socket, rsp, strlen(rsp));
}

void quit(clinet_connection_t* client_connection_ptr)
{
    if (!check_cmd_size(client_connection_ptr, 1))
        return;

    client_connection_ptr->terminate_flag = 1;

    char rsp[RSP_MSG_LIMIT];
    snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, SERVICE_CLOSE_SUCCESS, "Goodbye.");
    send_to_socket(client_connection_ptr->control_socket, rsp, strlen(rsp));
}

void cwd(clinet_connection_t* client_connection_ptr)
{
    if (!verify_login_state(client_connection_ptr))
        return;
    if (!check_cmd_size(client_connection_ptr, 2))
        return;

    char *dir = (client_connection_ptr->next_cmd_tokens)[1];

    char rsp[RSP_MSG_LIMIT];
    // check does dir name starts with ./ or ../ or contains ../ in it
    if (strstr(dir, "./") == dir || strstr(dir, "../") != NULL || strcmp(dir, ".") == 0 || strcmp(dir, "..") == 0)
    {
        // dir path invalid
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, REQUESTED_FILE_ACTION_FAIL, "dir path contain token not allowed.");
    }
    else if (chdir(dir) != 0)
    {
        // change dir fail
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, REQUESTED_FILE_ACTION_FAIL, "Failed to change directory.");
    }
    else
    {
        // change dir success
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, REQUESTED_FILE_ACTION_SUCCESS, "Directory successfully changed.");
    }

    send_to_socket(client_connection_ptr->control_socket, rsp, strlen(rsp));
}

void cdup(clinet_connection_t* client_connection_ptr)
{
    if (!verify_login_state(client_connection_ptr))
        return;
    if (!check_cmd_size(client_connection_ptr, 1))
        return;

    char cur_dir[MAX_SIZE];
    char rsp[RSP_MSG_LIMIT];
    if (getcwd(cur_dir, MAX_SIZE) != NULL && strcmp(client_connection_ptr->root_dir, cur_dir) != 0 && chdir("..") == 0)
    {
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, REQUESTED_FILE_ACTION_SUCCESS, "Change to parent dir.");
    }
    else
    {
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, REQUESTED_FILE_ACTION_FAIL, "Failed to change to parent dir.");
    }

    send_to_socket(client_connection_ptr->control_socket, rsp, strlen(rsp));
}

void type(clinet_connection_t* client_connection_ptr)
{
    if (!verify_login_state(client_connection_ptr))
        return;

    if (!check_cmd_size(client_connection_ptr, 2))
        return;

    char *type_code = (client_connection_ptr->next_cmd_tokens)[1];
    char rsp[RSP_MSG_LIMIT];
    if (strcasecmp(type_code, "A") == 0)
    {
        client_connection_ptr->type = TYPE_A;
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, REQUEST_SUCCESS, "Type set to A.");
    }
    else if (strcasecmp(type_code, "I") == 0)
    {
        client_connection_ptr->type = TYEP_I;
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, REQUEST_SUCCESS, "Type set to I.");
    }
    else if (strcasecmp(type_code, "E") == 0 || strcasecmp(type_code, "L") == 0)
    {
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, PARAMETER_NOT_SUPPORTED, "only support ASCII and Image types.");
    }
    else
    {
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, WRONG_ARGUMENT, "Bad TYPE command.");
    }

    send_to_socket(client_connection_ptr->control_socket, rsp, strlen(rsp));
}

void mode(clinet_connection_t* client_connection_ptr)
{
    if (!verify_login_state(client_connection_ptr))
        return;

    if (!check_cmd_size(client_connection_ptr, 2))
        return;

    char *mode_code = (client_connection_ptr->next_cmd_tokens)[1];
    char rsp[RSP_MSG_LIMIT];
    if (strcasecmp(mode_code, "S") == 0)
    {
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, REQUEST_SUCCESS, "Mode set to S.");
    }
    else if (strcasecmp(mode_code, "B") == 0 || strcasecmp(mode_code, "C") == 0)
    {
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, PARAMETER_NOT_SUPPORTED, "only support Stream mode.");
    }
    else
    {
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, WRONG_ARGUMENT, "Bad MODE command.");
    }

    send_to_socket(client_connection_ptr->control_socket, rsp, strlen(rsp));
}

void stru(clinet_connection_t* client_connection_ptr)
{
    if (!verify_login_state(client_connection_ptr))
        return;

    if (!check_cmd_size(client_connection_ptr, 2))
        return;

    char *stru_code = (client_connection_ptr->next_cmd_tokens)[1];
    char rsp[RSP_MSG_LIMIT];
    if (strcasecmp(stru_code, "F") == 0)
    {
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, REQUEST_SUCCESS, "Structure set to F.");
    }
    else if (strcasecmp(stru_code, "R") == 0 || strcasecmp(stru_code, "P") == 0)
    {
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, PARAMETER_NOT_SUPPORTED, "only support File structure.");
    }
    else
    {
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, WRONG_ARGUMENT, "Bad STRU command.");
    }

    send_to_socket(client_connection_ptr->control_socket, rsp, strlen(rsp));
}

void retr(clinet_connection_t* client_connection_ptr)
{
    if (!verify_login_state(client_connection_ptr))
        return;
    if (!check_pasv_mode(client_connection_ptr))
        return;

    if (!check_cmd_size(client_connection_ptr, 2))
        return;

    char *f_path = (client_connection_ptr->next_cmd_tokens)[1];
    char rsp[RSP_MSG_LIMIT];

    // check file status first
    FILE *file_ptr = fopen(f_path, "rb");
    if (file_ptr == NULL)
    {
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, REQUESTED_FILE_ACTION_FAIL, "Failed to open file.");
    }
    else
    {
        // create data socket
        int data_socket = create_data_socket(client_connection_ptr->pasv_socket, DATA_SOCKET_TIMEOUT_S);
        if (data_socket > 0)
        {
            // open data socket success

            // get file size
            fseek(file_ptr, 0L, SEEK_END);
            long f_size = ftell(file_ptr);
            rewind(file_ptr);

            snprintf(rsp, RSP_MSG_LIMIT, "%d Opening BINARY mode data connection for %s (%ld bytes)\r\n", FILE_STATUS_OK, f_path, f_size);
            send_to_socket(client_connection_ptr->control_socket, rsp, strlen(rsp));

            char data_buf[MAX_SIZE];
            while ((f_size = fread(data_buf, 1, sizeof data_buf, file_ptr)) > 0)
                write(data_socket, data_buf, f_size);

            // close data socket
            close(data_socket);

            snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, CLOSE_DATA_CONNECTION, "Transfer complete.");
        }
        else
        {
            // open data socke fail
            snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, NO_DATA_CONNECTION, "Failed to establish connection.");
        }
    }
    send_to_socket(client_connection_ptr->control_socket, rsp, strlen(rsp));

    // clean up pasv socket
    if (client_connection_ptr->pasv_socket != -1)
    {
        close(client_connection_ptr->pasv_socket);
        client_connection_ptr->pasv_socket = -1;
    }
}

void pasv(clinet_connection_t* client_connection_ptr)
{

    if (!verify_login_state(client_connection_ptr))
        return;

    if (!check_cmd_size(client_connection_ptr, 1))
        return;

    // clean previous data socket
    if (client_connection_ptr->pasv_socket != -1)
    {
        close(client_connection_ptr->pasv_socket);
        client_connection_ptr->pasv_socket = -1;
    }

    char rsp[RSP_MSG_LIMIT];

    // find ip
    uint32_t ip = (uint32_t)find_ip();

    // create pasv socket and find the corresponsed port number
    int pasv_socket = create_listen_socket(REQUEST_SYS_ALLOCATED_PORT);
    client_connection_ptr->pasv_socket = pasv_socket;

    struct sockaddr_in pasv_socket_address;
    size_t address_len = sizeof(pasv_socket_address);
    if (pasv_socket > 0 && getsockname(pasv_socket, (struct sockaddr *)&pasv_socket_address, (socklen_t *)&address_len) == 0)
    {
        // open pasv socket and get port success
        int port = (int)ntohs(pasv_socket_address.sin_port);
        snprintf(rsp, RSP_MSG_LIMIT, "%d Entering Passive Mode (%d,%d,%d,%d,%d,%d).\r\n", ENTER_PASSIVE_MODE,
                 ip & 0xff, (ip >> 8) & 0xff, (ip >> 16) & 0xff, (ip >> 24) & 0xff, (port >> 8) & 0xff, port & 0xff);
    }
    else
    {
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, NO_DATA_CONNECTION, "Entering Passive Mode fail.");
    }

    send_to_socket(client_connection_ptr->control_socket, rsp, strlen(rsp));
}

void nlst(clinet_connection_t* client_connection_ptr)
{

    if (!verify_login_state(client_connection_ptr))
        return;

    if (!check_pasv_mode(client_connection_ptr))
        return;

    if (!check_cmd_size(client_connection_ptr, 1))
        return;

    char rsp[RSP_MSG_LIMIT];
    // create data socket
    int data_socket = create_data_socket(client_connection_ptr->pasv_socket, DATA_SOCKET_TIMEOUT_S);
    if (data_socket > 0)
    {
        // send to client data socket good
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, FILE_STATUS_OK, "Here comes the directory listing.");
        send_to_socket(client_connection_ptr->control_socket, rsp, strlen(rsp));

        // send list
        listFiles(data_socket, ".");

        // close data socket
        close(data_socket);

        // after send list
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, CLOSE_DATA_CONNECTION, "Directory send OK.");
    }
    else
    {
        // open data socke fail
        snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, NO_DATA_CONNECTION, "Failed to establish connection.");
    }

    send_to_socket(client_connection_ptr->control_socket, rsp, strlen(rsp));

    // clean up pasv socket
    if (client_connection_ptr->pasv_socket != -1)
    {
        close(client_connection_ptr->pasv_socket);
        client_connection_ptr->pasv_socket = -1;
    }
}

void unknown(clinet_connection_t* client_connection_ptr)
{
    if (!verify_login_state(client_connection_ptr))
        return;

    char rsp[RSP_MSG_LIMIT];
    snprintf(rsp, RSP_MSG_LIMIT, RSP_MSG_FORMAT, SYNTAX_ERROR, "Unknown command.");
    send_to_socket(client_connection_ptr->control_socket, rsp, strlen(rsp));
}
