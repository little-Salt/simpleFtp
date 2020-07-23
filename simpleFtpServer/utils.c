#include "utils.h"

/*
 * Function:  trim_whitespace 
 * --------------------
 * trim leading and trailing whitespace, and copy to dst pointer
 *
 *  out: pointer for store result
 * 	len: length of origin string
 * 	str: origin string
 *
 *  returns: the length of string after triming
 */
size_t trim_whitespace(char *out, size_t len, const char *str)
{
	if (len == 0)
		return 0;

	const char *end;
	size_t out_size;

	// Trim leading space
	while (isspace((unsigned char)*str))
		str++;

	if (*str == 0)
	{
		*out = 0;
		return 1;
	}

	// Trim trailing space
	end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char)*end))
		end--;
	end++;

	// Set output size to minimum of trimmed string length and buffer size minus 1
	out_size = (end - str) < len - 1 ? (end - str) : len - 1;

	// Copy trimmed string and add null terminator
	memcpy(out, str, out_size);
	out[out_size] = 0;

	return out_size;
}

/*
 * Function:  str_split 
 * --------------------
 * split string into tokens by delimiter, and save to provide pointer
 *
 *  out: pointer for store result
 * 	str: origin string
 * 	delim: delimiter used for split
 *  out_size: size of dst array
 *
 *  returns: the number of tokens
 */
size_t str_split(char **out, char *str, const char delim[], size_t out_size)
{
	char *token;
	char *temp;
	size_t size = 0;
	token = strtok(str, delim);

	while (token != NULL && size < out_size)
	{
		int length = strlen(token);
		out[size] = malloc(sizeof(char) * length);
		strcpy(out[size], token);
		token = strtok(NULL, delim);
		size++;
	}

	// out[size++] = 0;

	return size;
}

/*
 * Function:  count_tokens 
 * --------------------
 * count tokens number for string split by delimiter
 *
 * 	str: origin string
 * 	delim: delimiter used for split
 *
 *  returns: the number of tokens
 */
size_t count_tokens(char *str, const char delim[])
{
	char copy[1024];
	strcpy(copy, str);
	char *token;
	size_t size = 0;
	token = strtok(copy, delim);

	while (token != NULL)
	{
		token = strtok(NULL, delim);
		size++;
	}

	return size;
}

/*
 * Function:  find_ip 
 * --------------------
 * go throung interface address link list, found first ipv4 and non-loopback ip
 *
 *  returns: a ipv4 and non-loopback ip
 */
uint32_t find_ip()
{
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	uint32_t addr;

	getifaddrs(&ifap);
	for (ifa = ifap; ifa; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET && !(ifa->ifa_flags & IFF_LOOPBACK))
		{
			sa = (struct sockaddr_in *)ifa->ifa_addr;
			addr = sa->sin_addr.s_addr;
			break;
		}
	}

	freeifaddrs(ifap);
	return addr;
}

/*
 * Function:  free_double_ptr 
 * --------------------
 * a help method to free mem related to a doulb pointer
 * 
 * 	d_ptr: a double pointer
 * 	size : size of doulbe pointer
 *
 *  returns: no return values
 */
void free_double_ptr(char **d_ptr, size_t size)
{
	while (size--)
	{
		free(d_ptr[size]);
	}

	free(d_ptr);
}
