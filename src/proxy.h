#ifndef PROXY_H
#define PROXY_H

#include <stdbool.h>

typedef struct Proxy
{
	char ip[80];
	char user_password[80];
	char target_site[80];
} Proxy;

bool Proxy_Load(const char *file_path,
				Proxy **proxy_list_ptr,
				int *proxy_list_size);

#endif