#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"
#include "user-agent.h"

static struct curl_slist *_headers_list = NULL;

static size_t _WriteFunc(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	(void) ptr;
	(void) userdata;
	/* printf("ptr: %ld\n", ptr);
	 printf("size: %ld\n", size);
	 printf("nmemb: %ld\n", nmemb);
	 printf("userdata: %ld\n", userdata);*/

	return nmemb * size;
}

static void _AddHeader(const char *key, const char *value)
{
	char buffer[400];


	memset(buffer, 0, sizeof(buffer));
	strcat(buffer, key);
	strcat(buffer, ": ");
	strcat(buffer, value);

	_headers_list = curl_slist_append(_headers_list, buffer);
}

static void _InitHeaders(void)
{
	_AddHeader("Content-Type", "text/plain;charset=UTF-8");
	_AddHeader("cf-visitor", "https");
	_AddHeader("User-Agent", UserAgent_SelectRandom());
	_AddHeader("Connection", "keep-alive");
	_AddHeader("Accept", "application/json, text/plain, */*");
	_AddHeader("Accept-Language", "ru");
	_AddHeader("x-forwarded-proto", "https");
	_AddHeader("Accept-Encoding", "gzip, deflate, br");
}

void HTTP_ModuleInit(void)
{
	curl_global_init(CURL_GLOBAL_ALL);
	_InitHeaders();
}

void HTTP_ModuleFree(void)
{
	curl_slist_free_all(_headers_list);
}

bool HTTP_Create(HTTP **http_ptr, unsigned int id)
{
	HTTP *http;
	CURL *curl;
	char cookie_file[80];


	sprintf(cookie_file, "%d.txt", id);
	*http_ptr = malloc(sizeof(HTTP));
	http = *http_ptr;
	curl = curl_easy_init();
	if (!curl)
	{
		printf("Curl easy init error\n");
		return false;
	}
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
	curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, true);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _WriteFunc);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, _headers_list);
	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie_file);
	curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookie_file);

	http->curl = curl;
	http->is_verbose = false;
	return true;
}

void HTTP_SetVerbose(HTTP *http, bool is_verbose)
{
	http->is_verbose = is_verbose;
}

bool HTTP_SetURL(HTTP *http, const char *url)
{
	curl_easy_setopt(http->curl, CURLOPT_URL, url);

	#if 0
	if (strncmp(url, "https", 5) == 0)
	{
		curl_easy_setopt(http->curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS);
	}
	#endif

	return true;
}

bool HTTP_SetProxy(HTTP *http, const char *proxy_ip, const char *proxy_user_password)
{
	curl_easy_setopt(http->curl, CURLOPT_PROXY, proxy_ip);
	if (strlen(proxy_user_password) > 0)
	{
		curl_easy_setopt(http->curl, CURLOPT_PROXYUSERPWD, proxy_user_password);
	}
	return true;
}

long HTTP_Request(HTTP *http)
{
	long response_code;
	CURLcode error;
	char *url;
	struct curl_slist *cookies = NULL;


	curl_easy_getinfo(http->curl, CURLINFO_EFFECTIVE_URL, &url);
	error = curl_easy_perform(http->curl);
	if (error)
	{
		if (http->is_verbose)
			printf("[%s] ERROR: %s\n", url, curl_easy_strerror(error));
		return 0;
	}
	
	curl_easy_getinfo(http->curl, CURLINFO_RESPONSE_CODE, &response_code);
	if (http->is_verbose)
		printf("[%s] SUCCESS: http code: %ld\n", url, response_code);

    error = curl_easy_getinfo(http->curl, CURLINFO_COOKIELIST, &cookies);
    if (error)
    {
    	if (http->is_verbose)
			printf("Get cookie error: %s\n", curl_easy_strerror(error));
		return 0;
    }

	return response_code;
}

void HTTP_Free(HTTP **http_ptr)
{
	curl_easy_cleanup((*http_ptr)->curl);
	free(*http_ptr);
}