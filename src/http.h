#ifndef HTTP_H
#define HTTP_H

#include <stdbool.h>
#include "curl/curl.h"

typedef struct HTTP
{
	CURL *curl;
	bool is_verbose;
} HTTP;

void HTTP_ModuleInit();
void HTTP_ModuleFree();

bool HTTP_Create(HTTP **http_ptr);
void HTTP_SetVerbose(HTTP *http, bool is_verbose);
bool HTTP_SetURL(HTTP *http, const char *url);
bool HTTP_SetProxy(HTTP *http, const char *proxy_ip, const char *proxy_user_password);
long HTTP_Request(HTTP *http);
void HTTP_Free(HTTP **http_ptr);

#endif