#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include "helpers.h"
#include "requests.h"

void add_to_message(char *message, const char *format, ...) {
    char *line = calloc(LINELEN, sizeof(char));
    va_list args;
    va_start(args, format);

    vsnprintf(line, LINELEN, format, args);
    strcat(message, line);

    va_end(args);
    free(line);
}

char *compute_get_request(char *host, char *url, char *cookie, char *target) {
    char *message = calloc(BUFLEN, sizeof(char));
    
    add_to_message(message, "GET %s HTTP/1.1\r\n", url);

    if (target != NULL) {
        add_to_message(message, "Authorization: Bearer %s\r\n", target);
    }

    add_to_message(message, "Host: %s\r\n", host);

    if (cookie != NULL) {
        add_to_message(message, "Cookie: %s;\r\n", cookie);
    }

    strcat(message, "\r\n");

    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char *body_data, char *jwt) {
    char *message = calloc(BUFLEN, sizeof(char));
    
    add_to_message(message, "POST %s HTTP/1.1\r\n", url);
    add_to_message(message, "Host: %s\r\n", host);

    if (jwt != NULL) {
        add_to_message(message, "Authorization: Bearer %s\r\n", jwt);
    }

    add_to_message(message, "Content-Type: %s\r\n", content_type);
    add_to_message(message, "Content-Length: %d\r\n", strlen(body_data));
    
    strcat(message, "\r\n");
    strcat(message, body_data);

    return message;
}

char *compute_delete_request(char *host, char *url, char *jwt) {
    char *message = calloc(BUFLEN, sizeof(char));

    add_to_message(message, "DELETE %s HTTP/1.1\r\n", url);
    add_to_message(message, "Host: %s\r\n", host);

    if (jwt != NULL) {
        add_to_message(message, "Authorization: Bearer %s\r\n", jwt);
    }

    strcat(message, "\r\n");

    return message;
}
