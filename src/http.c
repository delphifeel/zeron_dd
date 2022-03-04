#include <stdio.h>
#include <stdlib.h>
#include "http.h"

static size_t _WriteFunc(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	// printf("ptr: %ld\n", ptr);
	// printf("size: %ld\n", size);
	// printf("nmemb: %ld\n", nmemb);
	// printf("userdata: %ld\n", userdata);

	return nmemb * size;
}

bool HTTP_Create(HTTP **http)
{

	*http = malloc(sizeof(HTTP));
	CURL *curl = curl_easy_init();
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

	(*http)->curl = curl;
	return true;
}

bool HTTP_SetURL(HTTP *http, const char *url)
{
	curl_easy_setopt(http->curl, CURLOPT_URL, url);

	// if (strncmp(url, "https", 5) == 0)
	{
		// curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS);
	}

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
	CURLcode error = curl_easy_perform(http->curl);
	if (error)
	{
		// printf("Curl perform error: %s\n", curl_easy_strerror(error));
		return 0;
	}

	long response_code;
	curl_easy_getinfo(http->curl, CURLINFO_RESPONSE_CODE, &response_code);
	// printf("Curl request http code: %ld\n", response_code);
	return response_code;
}

void HTTP_Free(HTTP **http)
{
	curl_easy_cleanup((*http)->curl);
	free(*http);
}