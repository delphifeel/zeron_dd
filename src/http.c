#include <stdio.h>
#include <stdlib.h>
#include "http.h"

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

bool HTTP_Create(HTTP **http_ptr)
{
	HTTP *http;
	CURL *curl;


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

	/*if (strncmp(url, "https", 5) == 0)
	{
		curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS);
	}*/

	return true;
}

bool HTTP_SetProxy(HTTP *http, const char *proxy_ip, const char *proxy_user_password)
{
	curl_easy_setopt(http->curl, CURLOPT_PROXY, proxy_ip);
	curl_easy_setopt(http->curl, CURLOPT_PROXYUSERPWD, proxy_user_password);
	return true;
}

long HTTP_Request(HTTP *http)
{
	long response_code;
	CURLcode error;


	error = curl_easy_perform(http->curl);
	if (error)
	{
		if (http->is_verbose)
			printf("Request error: %s\n", curl_easy_strerror(error));
		return 0;
	}
	
	curl_easy_getinfo(http->curl, CURLINFO_RESPONSE_CODE, &response_code);
	if (http->is_verbose)
		printf("Request success - http code: %ld\n", response_code);
	return response_code;
}

void HTTP_Free(HTTP **http_ptr)
{
	curl_easy_cleanup((*http_ptr)->curl);
	free(*http_ptr);
}