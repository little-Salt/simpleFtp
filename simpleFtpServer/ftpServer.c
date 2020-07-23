#include <stdlib.h>
#include <errno.h>
#include "usage.h"
#include "service.h"

#define DEFALUT_THREAD_SIZE 1;

int main(int argc, char *argv[])
{
	// Check the command line arguments
	if (argc != 2)
	{
		usage(argv[0]);
		return -1;
	}

	// check port number in argument
	char *p;
	errno = 0;
	long port = strtol(argv[1], &p, 10);
	if (*p != '\0' || errno != 0)
	{
		usage(argv[0]);
		return -1;
	}

	if (port > 65535 || port < 1024)
	{
		usage(argv[0]);
		return -1;
	}
	unsigned long thread_size =  DEFALUT_THREAD_SIZE;
	if (argc > 3) {
		unsigned long thread_size = strtoul(argv[2], &p, 10);
		if (*p != '\0' || errno != 0)
		{
			usage(argv[0]);
			return -1;
		}
	}

	// start ftp service at given port
	service_start((int) port, (unsigned int) thread_size);

	return 0;
}
