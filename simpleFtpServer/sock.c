#include "sock.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int create_listen_socket(int port)
{
    int socketd = socket(PF_INET, SOCK_STREAM, 0);

    if (socketd < 0)
    {
        perror("Failed to create the socket.");

        return -1;
    }

    // Reuse the address
    int value = 1;

    if (setsockopt(socketd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int)) != 0)
    {
        perror("Failed to set the socket option");

        return -1;
    }

    // Bind the socket to a port
    struct sockaddr_in address;

    bzero(&address, sizeof(struct sockaddr_in));

    address.sin_family = AF_INET;

    address.sin_port = htons(port);

    address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socketd, (const struct sockaddr *)&address, sizeof(struct sockaddr_in)) != 0)
    {
        perror("Failed to bind the socket");

        return -1;
    }

    // Set the socket to listen for connections
    if (listen(socketd, 1) != 0)
    {
        perror("Failed to listen for connections");

        return -1;
    }

    return socketd;
}

int create_connection_socket(int listen_socket)
{
    struct sockaddr_in clientAddress;

    socklen_t clientAddressLength = sizeof(struct sockaddr_in);

    int clientd = accept(listen_socket, (struct sockaddr *)&clientAddress, &clientAddressLength);

    if (clientd < 0)
    {
        perror("Failed to accept the client connection");
    }

    printf("Accepted the client connection from %s:%d.\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));

    return clientd;
}

int create_data_socket(int listen_socket, long time_out)
{
    fd_set rfds;
    struct timeval tv;
    int retval;

    FD_ZERO(&rfds);
    FD_SET(listen_socket, &rfds);

    tv.tv_sec = time_out;
    tv.tv_usec = 0;

    retval = select((listen_socket+1), &rfds, NULL, NULL, &tv);
    
    if (retval)
        return create_connection_socket(listen_socket);
    else
       return retval;
}

int send_to_socket(int socket, char *buffer, int length)
{
    return send(socket, buffer, length, 0);
}

int read_from_socket(int socket, char *buffer, int length)
{
    return recv(socket, buffer, length, 0);
}
