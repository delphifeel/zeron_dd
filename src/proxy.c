#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proxy.h"

#define DEFAULT_LIST_SIZE 	(100)

bool Proxy_Load(Proxy **proxy_list_ptr, int *proxy_list_size)
{
	FILE *fp;
	int c;
	char line[256];
	Proxy *proxy;
	int line_size = 0;
	unsigned int list_size = 0;
	bool result = true;
	unsigned int line_uspwd_index;
    unsigned int proxy_list_cap = DEFAULT_LIST_SIZE;


	fp = fopen("../proxy_parser/proxies.txt", "r");
	if (fp == NULL)
	{
		printf("Cant open proxy file\n");
		return false;
	}

	*proxy_list_ptr = malloc(proxy_list_cap * sizeof(Proxy));

	while ((c = fgetc(fp)) != EOF) 
	{
		if (line_size > 255)
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

			memset(proxy, 0, sizeof(*proxy));

			line_uspwd_index = strchr(line, '|') - line + 1;
			if (line_uspwd_index >= sizeof(proxy->ip))
			{
				printf("Proxy load error: ip too big\n");
				result = false;
				break;
			}
			memcpy(proxy->ip, line, line_uspwd_index - 1);
			if ((line_size - line_uspwd_index) >= sizeof(proxy->user_password))
			{
				printf("Proxy load error: uspwd too big\n");
				result = false;
				break;
			}
			memcpy(proxy->user_password, 
				   line + line_uspwd_index, 
				   line_size - line_uspwd_index - 1);

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