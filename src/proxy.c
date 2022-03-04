#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proxy.h"

bool Proxy_Load(Proxy proxy_list[], int proxy_list_max_size, int *proxy_list_size)
{
	int c;
	char line[256];
	Proxy *proxy;
	int line_size = 0;
	FILE *fp = fopen("../proxy_parser/proxies.txt", "r");
	if (fp == NULL)
	{
		printf("Cant open proxy file\n");
		return false;
	}
	int list_size = 0;
	bool result = true;

	while ((c = fgetc(fp)) != EOF) 
	{
		if (line_size > 255)
		{
			printf("Line too big\n");
			return false;
		}
		line[line_size++] = c;
		if (c == '\n')
		{
			// parse line
			proxy = proxy_list + list_size;
			list_size++;

			memset(proxy, 0, sizeof(*proxy));

			int line_uspwd_index = strchr(line, '|') - line + 1;
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
			memcpy(proxy->user_password, line + line_uspwd_index, line_size - line_uspwd_index - 1);

			if (list_size == proxy_list_max_size)
			{
				printf("Proxy load error: proxy list is too small\n");
				result = false;
				break;
			}

			line_size = 0;
		}
	}

	*proxy_list_size = list_size;
	fclose(fp);
	return result;
}