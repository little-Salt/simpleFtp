#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "service.h"
#include "sock.h"
#include "connection.h"
#include "thread_pool.h"

#define DEFALUT

thread_pool* pool = NULL;

void handle_sigint(int sig) 
{ 
	thread_pool_shutdown(pool);
	exit(EXIT_SUCCESS);
} 

/*
 * Function:  interact 
 * --------------------
 * thread action method to handle accepted client connection
 *
 *  args: file descriptor of new accepted client control socket
 *
 *  returns: NULL after all actions finished
 */
void* connection_hanlder(void* args) {
	int connection_socket = *(int*) args;

	// create client connection
	clinet_connection_t* client_connection_ptr = initialize_client_connection(connection_socket);

	// handle client connection
	handle_client_connection(client_connection_ptr);

	// clean up client connection
	clean_up_client_connection(client_connection_ptr);

	return NULL;
}

/*
 * Function:  service_start 
 * --------------------
 * start ftp server service, open socket waiting for client connection, 
 * 		and create new thread for success client connection to interact with client. only handle one
 * 		connection at one time
 *
 *  port: port number that serverice should listen on for client connection
 *
 *  returns: no returns
 */
void service_start(int port, unsigned int thread_size)
{
	// create TCP socket for servert to listen
	int listen_socket = create_listen_socket(port);

	if (listen_socket < 0) {
		perror("Failed to open linsten socket, service terminating.");

        exit(EXIT_FAILURE);
	}

	pool = thread_pool_create(thread_size);

	// start waiting for client connection
	signal(SIGINT, handle_sigint); 
	while (1)
	{
		// Accept the client connection
		thread_pool_wait_idle(pool);

		int client_connection_socket = create_connection_socket(listen_socket);

		if (client_connection_socket < 0)
		{
			perror("Failed to accept the client connection");

			continue;
		}

		// Create a separate thread to interact with the client
		// pthread_t thread;

		// if (pthread_create(&thread, NULL, interact, &client_connection_socket) != 0)
		// {
		// 	perror("Failed to create the thread");

		// 	continue;
		// }

		// // The main thread just waits until the interaction is done
		// pthread_join(thread, NULL);

		// printf("Current client connection interaction has finished.\n\n");

		thread_pool_add_task(pool, &connection_hanlder, &client_connection_socket);
	}

	// exit
	exit(EXIT_SUCCESS);
}