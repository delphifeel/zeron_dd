#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#include "proxy.h"
#include "http.h"

void *task(void *user_data)
{
	Proxy *proxy = user_data;
	HTTP *http;
	HTTP_Create(&http);
	HTTP_SetURL(http, "google.com");
	// HTTP_SetProxy(http, proxy->ip, proxy->user_password);
	long http_code = HTTP_Request(http);
	if (http_code != 200)
	{
		// printf("ERROR. proxy: '%s', code: %ld\n", proxy->ip, http_code);
	}
	HTTP_Free(&http);

	return NULL;
}


int main()
{
	HTTP_GLOBAL_INIT();

	#define PROXY_LIST_SIZE 	(10)
	Proxy *proxy_list = malloc(PROXY_LIST_SIZE * sizeof(Proxy));
	if (proxy_list == NULL)
	{
		printf("OFM proxy_list\n");
		return 0;
	}
	int proxy_list_size = 0;
	Proxy_Load(proxy_list, PROXY_LIST_SIZE, &proxy_list_size);
	printf("Read %d proxy\n", proxy_list_size);

	pthread_t threads_list[proxy_list_size];

	printf("Start threads\n");
	for (int i = 0; i < proxy_list_size; i++)
	{
		pthread_create(threads_list + i, NULL, task, proxy_list + i);
	}

	for (int i = 0; i < proxy_list_size; i++)
	{
		pthread_join(threads_list[i], NULL);
	}

	free(proxy_list);
}