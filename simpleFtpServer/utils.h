#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#ifndef _UTILS_H_

#define _UTILS_H_

size_t trim_whitespace(char *out, size_t len, const char *str);

size_t str_split(char **out, char *str, const char delim[], size_t out_size);

size_t count_tokens(char *str, const char delim[]);

uint32_t find_ip();

void free_double_ptr(char **d_ptr, size_t size);

#endif