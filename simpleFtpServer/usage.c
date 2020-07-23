#include <sys/types.h>
#include <stdio.h>
#include "usage.h"

// Given the name of the program print out usage instructions. */
void usage(char *progName) {

  fprintf(stderr, "Usage: %s <port> <thread_size>\n", progName);
  fprintf(stderr, "     <port>   Specifies the port the server will accept connections on.\n");
  fprintf(stderr, "              The port value must >= 1024 and <= 65535.\n");
  fprintf(stderr, "     <thread_size>   Specifies the thread size of server.\n");
  fprintf(stderr, "                     The size must >= 0.\n");
}
