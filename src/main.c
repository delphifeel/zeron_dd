#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#include "proxy.h"
#include "http.h"


#define THREADS_COUNT 		(500)

static pthread_mutex_t mutex;
static int thread_proxy_index = 0;
static Proxy *proxy_list;
static int proxy_list_size = 0;
static int good_proxies = 0;

void *task(void *user_data)
{
	HTTP *http;
	HTTP_Create(&http);

	while (1)
	{
		pthread_mutex_lock(&mutex);

		if (thread_proxy_index >= proxy_list_size)
		{
			thread_proxy_index = 0;
		}

		int proxy_index = thread_proxy_index;
		thread_proxy_index++;
		pthread_mutex_unlock(&mutex);

		Proxy *proxy = &proxy_list[proxy_index];
		HTTP_SetURL(http, "1c.ru");
		HTTP_SetProxy(http, proxy->ip, proxy->user_password);
		long http_code = HTTP_Request(http);
		if (http_code)
		{
			if (http_code != 200)
			{
				// printf("ERROR proxy #%ld, code: %ld\n", proxy_index, http_code);
			}
			else
			{
				printf("good");
			}
		}
	}

	HTTP_Free(&http);
	return NULL;
}

// static void MultiAddAll()
// {
//   	CURLM *multi_handle = curl_multi_init();

// 	for (int i = 0; i < proxy_list_size; i++)
// 	{
// 		HTTP *http;
// 		HTTP_Create(&http);
// 		HTTP_SetURL(http, "1c.ru");
// 		HTTP_SetProxy(http, proxy_list[i].ip, proxy_list[i].user_password);

// 		curl_multi_add_handle(multi_handle, http->curl);
// 	}

// START:
// 	printf("Start multi\n");
// 	int still_running = 1;
// 	while (still_running)
// 	{
// 		CURLMcode error = curl_multi_perform(multi_handle, &still_running);
// 		if (error)
// 		{
// 			printf("Multi error\n");
// 			return;
// 		}
// 	}
// 	printf("Multi done\n");

// 	struct CURLMsg *m;
// 	do 
// 	{
// 		int msgq = 0;
// 		m = curl_multi_info_read(multi_handle, &msgq);
// 		if(m && (m->msg == CURLMSG_DONE))
// 		{
// 			curl_multi_remove_handle(multi_handle, m->easy_handle);
// 			curl_multi_add_handle(multi_handle, m->easy_handle);

// 			if (m->data.result != 0)
// 			{
// 				// printf("perform error\n");
// 				continue;
// 			}
// 			long response_code;
// 			curl_easy_getinfo(m->easy_handle, CURLINFO_RESPONSE_CODE, &response_code);
// 			if (response_code != 200)
// 			{
// 				printf("perform http code: %ld\n", response_code);
// 				continue;
// 			}

// 			printf("success: %ld\n", response_code);
// 		}
// 	} while (m);

// 	goto START;
// }


int main()
{
	HTTP_GLOBAL_INIT();
	pthread_mutex_init(&mutex, NULL);

	#define PROXY_LIST_SIZE 	(9000)
	proxy_list = malloc(PROXY_LIST_SIZE * sizeof(Proxy));
	if (proxy_list == NULL)
	{
		printf("OOM proxy_list\n");
		return 0;
	}
	Proxy_Load(proxy_list, PROXY_LIST_SIZE, &proxy_list_size);
	printf("Read %d proxy\n", proxy_list_size);

	pthread_t threads_list[THREADS_COUNT];

	printf("Start threads\n");
	for (int i = 0; i < THREADS_COUNT; i++)
	{
		pthread_create(threads_list + i, NULL, task, NULL);
	}

	for (int i = 0; i < THREADS_COUNT; i++)
	{
		pthread_join(threads_list[i], NULL);
	}

	pthread_mutex_destroy(&mutex);
	free(proxy_list);
}