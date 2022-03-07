#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proxy.h"

#define DEFAULT_LIST_SIZE 	(100)

static bool _ParseLine(char line[], Proxy *proxy)
{
	char *ip = strtok(line, "|");
	char *user_password = strtok(NULL, "|");
	char *target_site = strtok(NULL, "|");

	if (strlen(ip) > sizeof(proxy->ip) - 1)
	{
		printf("Parse line error: ip buffer to small\n");
		return false;
	}
	if (strlen(user_password) > sizeof(proxy->user_password) - 1)
	{
		printf("Parse line error: user_password buffer to small\n");
		return false;
	}
	if (strlen(target_site) > sizeof(proxy->target_site) - 1)
	{
		printf("Parse line error: target_site buffer to small\n");
		return false;
	}

	strcpy(proxy->ip, ip);
	strcpy(proxy->user_password, user_password);
	strcpy(proxy->target_site, target_site);

	return true;
}

bool Proxy_Load(const char *file_path, Proxy **proxy_list_ptr, int *proxy_list_size)
{
	FILE *fp;
	int c;
	char line[400];
	Proxy *proxy;
	int line_size = 0;
	unsigned int list_size = 0;
	bool result = true;
	unsigned int line_uspwd_index;
    unsigned int proxy_list_cap = DEFAULT_LIST_SIZE;


	fp = fopen(file_path, "r");
	if (fp == NULL)
	{
		printf("Cant open proxy file\n");
		return false;
	}

	*proxy_list_ptr = malloc(proxy_list_cap * sizeof(Proxy));

	while ((c = fgetc(fp)) != EOF) 
	{
		if (line_size > sizeof(line) - 1)
		{
			printf("Line too big\n");
			result = false;
			break;
		}
		line[line_size++] = c;
		if (c == '\n')
		{
            if (list_size == proxy_list_cap)
            {
                proxy_list_cap *= 2;
                *proxy_list_ptr = realloc(*proxy_list_ptr, proxy_list_cap * sizeof(Proxy));
                if (*proxy_list_ptr == NULL)
                {
                    printf("OOM Proxy_Load\n");
                    exit(1);
                }
            }

			proxy = *proxy_list_ptr + list_size;
			list_size++;

			line[line_size - 1] = '\0';
			if (!_ParseLine(line, proxy))
			{
				result = false;
				break;
			}

			line_size = 0;
		}
	}

    if (result == false)
    {
        free(*proxy_list_ptr);
    }
	*proxy_list_size = list_size;
	fclose(fp);
	return result;
}