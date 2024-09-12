//
// Created by tomas on 14-08-24.
//

#include "HttpServer.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void tcpHandleFunc(TcpServer* server, const char data[], uint8_t argc, ...);

char* adjustStringToSize(char buffer[]);

static void extractRequestLine(char buffer[], const char* data);
static void extractMethod(char buffer[], const char* data);
static void extractPath(char buffer[], const char* data);
static void extractHttpVersion(char buffer[], const char* data);
static void extractHeaders(char buffer[], const char* data);
static void setHeaders(HashTable* headersTable, const char* data);
static void extractBody(char buffer[], const char* data);

static HttpResponse* statusResponse(HttpResponse* response, uint16_t statusCode);
static void sendResponse(HttpResponse* response, const char* body);

static void initializeHttpStatuses();

HashTable* http_statuses = NULL;

//* ========== REQUEST ==========

HttpRequest* init_request() {
    HttpRequest* req = (HttpRequest*)malloc(sizeof(HttpRequest));
    req->_requestLine = NULL;
    req->method = NULL;
    req->path = NULL;
    req->version = NULL;
    req->_headers = NULL;
    req->headers = NULL;
    req->body = NULL;
    
    req->_requestLine = malloc(HTTP_SERVER_DEFAULT_REQUEST_LINE_SIZE * sizeof(char));
    req->method = malloc(HTTP_SERVER_DEFAULT_METHOD_SIZE * sizeof(char));
    req->path = malloc(HTTP_SERVER_DEFAULT_PATH_SIZE * sizeof(char));
    req->version = malloc(HTTP_SERVER_DEFAULT_VERSION_SIZE * sizeof(char));
    req->_headers = malloc(HTTP_SERVER_DEFAULT_HEADERS_SIZE * sizeof(char));
    req->headers = newHashTable(100);
    req->body = malloc(HTTP_SERVER_DEFAULT_BODY_SIZE * sizeof(char));

    return req;
}

void free_request(HttpRequest* req) {
    free(req->_requestLine);
    free(req->method);
    free(req->path);
    free(req->version);
    free(req->_headers);
    free(req->headers);
    free(req->body);
    free(req);
}
//* ========== END REQUEST ==========

//* ========== RESPONSE ==========

HttpResponse* init_response() {
    HttpResponse* res = (HttpResponse*)malloc(sizeof(HttpResponse));
    res->code = 0;
    res->status = NULL;
    res->version = NULL;
    res->contentType = NULL;
    res->location = NULL;
    res->body = NULL;
    res->tcpServer = NULL;
    res->stat = NULL;
    res->send = NULL;

    res->status = malloc(HTTP_SERVER_DEFAULT_RESPONSE_STATUS_SIZE * sizeof(char));
    res->version = malloc(HTTP_SERVER_DEFAULT_VERSION_SIZE * sizeof(char));
    res->contentType = malloc(HTTP_SERVER_DEFAULT_RESPONSE_CONTENT_TYPE_SIZE * sizeof(char));
    res->location = malloc(HTTP_SERVER_DEFAULT_RESPONSE_LOCATION_SIZE * sizeof(char));
    res->body = malloc(HTTP_SERVER_DEFAULT_RESPONSE_BODY_SIZE * sizeof(char));

    return res;
}

void free_response(HttpResponse* res) {
    free(res->status);
    free(res->version);
    free(res->contentType);
    free(res->location);
    free(res->body);
    free(res);
}
//* ========== END RESPONSE ==========

//* ========== SERVER ==========
/* 
HttpServer* httpserver_constructor(
    const int domain,
    const int service,
    const int protocol,
    const u_long interface,
    const int port,
    const int backlog,
    const int buffer_size,
    const HttpHandleFunc handle
)
{
    if (http_statuses == NULL)
    {
        initializeHttpStatuses();
    }
    HttpServer* httpServer = (HttpServer*)malloc(sizeof(HttpServer));
    httpServer->tcpServer = (TcpServer*)malloc(sizeof(TcpServer));
    if (httpServer->tcpServer)
    {
        *httpServer->tcpServer = server_constructor(domain, service, protocol, interface, port, backlog, buffer_size, tcpHandleFunc);
    }
    httpServer->onRequest = handle;
    return httpServer;
} */

HttpServer* base_httpserver_constructor(const HttpHandleFunc handle)
{
    if (http_statuses == NULL)
    {
        printf("===== INITIALIZING STATUSES =====\n");
        initializeHttpStatuses();
    }
    HttpServer* httpServer = (HttpServer*)malloc(sizeof(HttpServer));
    httpServer->tcpServer = (TcpServer*)malloc(sizeof(TcpServer));

    httpServer->onRequest = handle;
    httpServer->onRequest(NULL, NULL, NULL);
    handle(NULL, NULL, NULL);

    if (httpServer->tcpServer)
    {
        printf("===== INITIALIZING TCP SERVER =====\n");
        *httpServer->tcpServer = base_server_constructor(tcpHandleFunc); 
    }
    printf("===== ASSIGNING HANDLER FUNCTION =====\n");
    return httpServer;
}
//* ========== END SERVER ==========

//* ========== UTILS ==========
void tcpHandleFunc(TcpServer* server, const char data[], uint8_t argc, ...)
{

    // Get httpServer from variable args
    if (argc < 1)
    {
        perror("Failed to get httpServer from variable args");
        return;
    }

    printf("===== INITIALIZING VAARGS =====\n");
    va_list args;
    va_start(args, argc);
    HttpServer* httpServer;
    if (argc > 0)
    {
        httpServer = (HttpServer*)va_arg(args, HttpServer *);
    }
    va_end(args);
    printf("===== END INITIALIZING VAARGS =====");

    HttpRequest* request = init_request();
    HttpResponse* response = init_response();

    if (!request || !response)
    {
        perror("Failed to allocate memory for request/response");
        return;
    }

    extractRequestLine(request->_requestLine, data);
    extractMethod(request->method, request->_requestLine);
    extractPath(request->path, request->_requestLine);
    extractHttpVersion(request->version, request->_requestLine);
    extractHeaders(request->_headers, data);
    setHeaders(request->headers, request->_headers);
    extractBody(request->body, data);

    // request->_requestLine = adjustStringToSize(request->_requestLine);
    // request->method = adjustStringToSize(request->method);
    // request->path = adjustStringToSize(request->path);
    // request->version = adjustStringToSize(request->version);
    // request->_headers = adjustStringToSize(request->_headers);
    // request->body = adjustStringToSize(request->body);

    response->code = 200;
    strcpy(response->version, request->version); 
    response->tcpServer = server; // Use the server passed in
    response->stat = statusResponse;
    response->send = sendResponse;

    printf("Host: %s\n", request->headers->get(request->headers, "Host"));
    printf("===== CALLING ON REQUEST =====\n");
    // Call the onRequest handler
    httpServer->onRequest(NULL, NULL, NULL);
    httpServer->onRequest(httpServer, request, response);

    // Free allocated memory after use
    free_request(request);
    free_response(response);
}

static HttpResponse* statusResponse(HttpResponse* response, uint16_t statusCode)
{
    response->code = statusCode;
    char stat[3];
    sprintf(stat, "%i", statusCode);
    strcpy(response->status, http_statuses->get(http_statuses, stat));
    return response;
}

static void sendResponse(HttpResponse* response, const char* body)
{
    char ret[512];
    sprintf(ret, "HTTP/%s%s %i %s\r\n", response->version, response->code, response->status);

    char responseString[(int)strlen(body) + 4];
    sprintf(responseString, "\r\n\r\n%s", body);
    
    response->tcpServer->out(response->tcpServer, ret);
    response->tcpServer->out(response->tcpServer, "\r\nContent-Type: text/html");
    response->tcpServer->out(response->tcpServer, responseString);
}

char* adjustStringToSize(char* buffer) {
    return realloc(buffer, (int)strlen(buffer) + 1);
}

static void extractRequestLine(char buffer[], const char* data)
{
    char* requestLine = malloc(HTTP_SERVER_DEFAULT_REQUEST_LINE_SIZE * sizeof(char));

    uint16_t i = 0;
    for (; i < HTTP_SERVER_DEFAULT_REQUEST_LINE_SIZE; i++)
    {
        if (data[i] == '\n' || data[i] == '\r')
        {
            break;
        }
        requestLine[i] = data[i];
    }
    requestLine[i] = '\0';
    // requestLine = adjustStringToSize(requestLine);

    printf("Length: %zu\n", i);

    strncpy(buffer, requestLine, i);
    buffer[i] = '\0';

    free(requestLine);
}

static void extractMethod(char buffer[], const char* data)
{
    char* method = malloc(HTTP_SERVER_DEFAULT_METHOD_SIZE * sizeof(char));

    uint16_t i = 0;
    for (; i < HTTP_SERVER_DEFAULT_METHOD_SIZE; i++)
    {
        if (data[i] == ' ')
        {
            break;
        }
        method[i] = data[i];
    }

    method[i] = '\0';
    // method = adjustStringToSize(method);

    strcpy(buffer, method);
}

static void extractPath(char* buffer, const char* request)
{

  // Find the first space character which separates the path
  const char* pathStart = strchr(request, ' ');

  // Check for invalid request if no space found
  if(pathStart == NULL)
  {
    return; 
  }

  // Advance past the space
  pathStart++;  

  // Find the next space which ends the path
  const char* pathEnd = strchr(pathStart, ' ');

  // Calculate the length of the path 
  int pathLen = (pathEnd == NULL) ? (int)strlen(pathStart) : (pathEnd - pathStart);

  // Copy the path into the buffer
  strncpy(buffer, pathStart, pathLen);

  // Null terminate the buffer
  buffer[pathLen] = '\0';

}

static void extractHttpVersion(char* buffer, const char* request)
{
  // Find first space
  const char* versionStart = strchr(request, ' ');
  // Skip second space  
  versionStart = strchr(versionStart + 2, ' ');
  // Find end of version
  const char* versionEnd = strchr(versionStart, '\0');

  if (versionEnd == NULL)
  {
    return;
  }

  int length = versionEnd - versionStart;

  strncpy(buffer, versionStart, length);

  // Null terminate
  buffer[length] = '\0';

}

static void extractHeaders(char buffer[], const char* data)
{
    
    uint16_t headerStart = 0;
    uint16_t headerEnd = 0;
    bool newLineFlag = false;

    for (
        uint16_t i = 0
        ; i < HTTP_SERVER_DEFAULT_HEADERS_SIZE + HTTP_SERVER_DEFAULT_REQUEST_LINE_SIZE
        ; i++
    )
    {
        if (data[i] == '\r')
        {
            if (!newLineFlag) // Execute only on the first new line found
            {
                headerStart = i + 2; // Move to the next character after the newline
                newLineFlag = true;   // Set the flag to true
            }
        }

        // Check for the end of headers (two consecutive newlines)
        if (newLineFlag && data[i] == '\r' && data[i + 2] == '\r')
        {
            headerEnd = i; // Set end position for headers
            break;
        }
    }

    strncpy(buffer, &data[headerStart], headerEnd - headerStart);
    buffer[headerEnd - headerStart] = '\0';
}

static void setHeaders(HashTable* headersTable, const char* data)
{
    uint32_t keyStart = 0;
    uint32_t valueStart = 0;
    uint32_t colon = 0;

    for (uint16_t i = 0; i < HTTP_SERVER_DEFAULT_HEADERS_SIZE; i++)
    {
        if (data[i] == ':' && data[i + 1] == ' ')
        {
            colon = i;
            i += 2;
            valueStart = i;
            continue;
        }

        if (data[i] == '\r' && valueStart != 0)
        {
            char* key = malloc(sizeof(char) * (colon - keyStart + 1));
            char* value = malloc(sizeof(char) * (i));
            
            strncpy(key, &data[keyStart], colon - keyStart);
            key[colon - keyStart] = '\0';
            strncpy(value, &data[valueStart], i - valueStart);
            value[i - valueStart] = '\0';
            
            headersTable->set(headersTable, key, value);
            free(key);
            free(value);
            
            i += 2;
            keyStart = i;

            if (data[i] == '\r') break;
        }
    }

}

static void extractBody(char buffer[], const char* data)
{
    char* body = malloc(HTTP_SERVER_DEFAULT_BODY_SIZE * sizeof(char));

    bool isBody = false;
    uint16_t i;
    for (; i < HTTP_SERVER_DEFAULT_REQUEST_SIZE; i++)
    {
        if (data[i] == '\0') break;

        if (data[i] == '\r' && data[i + 2] == '\r') 
        {
            i += (data[i] == '\n') ? 1 : 3;
            isBody = true;
        }
        
        if (isBody)
        {
            body[i] = data[i];
        }
    }
    body[i] = '\0';

    strcpy(buffer, body);
}

static void initializeHttpStatuses()
{

    http_statuses = newHashTable(150);
    http_statuses->set(http_statuses, "100", "CONTINUE");
    http_statuses->set(http_statuses, "101", "SWITCHING PROTOCOLS");
    http_statuses->set(http_statuses, "102", "PROCESSING");
    http_statuses->set(http_statuses, "103", "CHECKPOINT");
    http_statuses->set(http_statuses, "200", "OK");
    http_statuses->set(http_statuses, "201", "CREATED");
    http_statuses->set(http_statuses, "202", "ACCEPTED");
    http_statuses->set(http_statuses, "203", "NON-AUTHORITATIVE INFORMATION");
    http_statuses->set(http_statuses, "204", "NO CONTENT");
    http_statuses->set(http_statuses, "205", "RESET CONTENT");
    http_statuses->set(http_statuses, "206", "PARTIAL CONTENT");
    http_statuses->set(http_statuses, "207", "MULTI-STATUS");
    http_statuses->set(http_statuses, "208", "ALREADY REPORTED");
    http_statuses->set(http_statuses, "300", "MULTIPLE CHOICES");
    http_statuses->set(http_statuses, "301", "MOVED PERMANENTLY");
    http_statuses->set(http_statuses, "302", "FOUND");
    http_statuses->set(http_statuses, "303", "SEE OTHER");
    http_statuses->set(http_statuses, "304", "NOT MODIFIED");
    http_statuses->set(http_statuses, "305", "USE PROXY");
    http_statuses->set(http_statuses, "306", "SWITCH PROXY");
    http_statuses->set(http_statuses, "307", "TEMPORARY REDIRECT");
    http_statuses->set(http_statuses, "308", "PERMANENT REDIRECT");
    http_statuses->set(http_statuses, "400", "BAD REQUEST");
    http_statuses->set(http_statuses, "401", "UNAUTHORIZED");
    http_statuses->set(http_statuses, "402", "PAYMENT REQUIRED");
    http_statuses->set(http_statuses, "403", "FORBIDDEN");
    http_statuses->set(http_statuses, "404", "NOT FOUND");
    // TODO: Add More
}

//* ========== END UTILS ==========