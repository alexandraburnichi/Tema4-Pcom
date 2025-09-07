#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* EXIT, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson-master/parson.h"
#include "parson-master/parson.c"
#include <ctype.h>

#define HOST "34.246.184.49"
#define SMOL_STRING 25
#define BIG_STRING 250

// case-urile pentru main (comenzile date de user)
typedef enum { 
    REGISTER, 
    LOGIN, 
    ENTER_LIBRARY, 
    GET_BOOKS, 
    GET_BOOK, 
    ADD_BOOK, 
    DELETE_BOOK, 
    LOGOUT, 
    EXIT, 
    UNKNOWN 
} RequestType;

RequestType getRequestType(char* request) {
    if (strcmp(request, "register") == 0)
        return REGISTER;
    if (strcmp(request, "login") == 0)
        return LOGIN;
    if (strcmp(request, "enter_library") == 0)
        return ENTER_LIBRARY;
    if (strcmp(request, "get_books") == 0)
        return GET_BOOKS;
    if (strcmp(request, "get_book") == 0)
        return GET_BOOK;
    if (strcmp(request, "add_book") == 0)
        return ADD_BOOK;
    if (strcmp(request, "delete_book") == 0)
        return DELETE_BOOK;
    if (strcmp(request, "logout") == 0)
        return LOGOUT;
    if (strcmp(request, "exit") == 0)
        return EXIT;
    return UNKNOWN;
}



// functie pentru crearea unui obiect JSON cu user si parola, pentru REGISTER si LOGIN
char *create_json_object_with_credentials(char *username, char *password) {
    JSON_Value *val = json_value_init_object();
    JSON_Object *obj = json_value_get_object(val);
    
    json_object_dotset_string(obj, "username", username);
    json_object_dotset_string(obj, "password", password);

    return json_serialize_to_string_pretty(val);
}

// functie pentru crearea unui obiect JSON, de tip BOOK
char *create_json_object_book(char *title, char *author, char *page_count, char *genre, char *publisher) {

    JSON_Value *val = json_value_init_object();
    JSON_Object *obj = json_value_get_object(val);
    
    json_object_set_string(obj, "title", title);
    json_object_set_string(obj, "author", author);
    json_object_set_string(obj, "page_count", page_count);
    json_object_set_string(obj, "genre", genre);
    json_object_set_string(obj, "publisher", publisher);

    return json_serialize_to_string_pretty(val);
}

// se extrag cartile
char *extract_payload(char *server_res) {
    char *res = strstr(server_res, "timeout");
    //char *r;
    //strtok(res, "\n");
    //strtok(NULL, "\n");
    //r = strtok(NULL, "\n");
    return res;
}

// verificam daca un string este un numar, facand abstractie de ultimul caracter
int verify_number_without_last(char *str) {
    for(int i = 0; i < strlen(str) - 1; i++) {
        if(!isdigit(str[i])) {
            return 0; // fals
        }
    }
    return 1; // adevarat
}

// verificam daca un string este un numar
int verify_number(char *str) {
    for(int i = 0; i < strlen(str); i++) {
        if(!isdigit(str[i])) {
            return 0; // fals
        }
    }
    return 1; // adevarat
}

//pentru add_book, citim de la tastatura si eliminam newline-ul
void trim_newline(char *str) {
    int len = strlen(str);
    if(len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';  // Replace the last character with a null terminator if it's a newline.
    }
}


int main(int argc, char *argv[])
{
    int sockfd;
    char *client_request = malloc(sizeof(char) * SMOL_STRING); 
    char *cookie=NULL;
    char *jwt=NULL;
    int library_entries = 0;
    int logins = 0;

while (1) {
    scanf("%s", client_request);
    sockfd = open_connection(HOST, 8080, AF_INET, SOCK_STREAM, 0);

    RequestType requestType = getRequestType(client_request);

// exista un case pentru fiecare comanda, inclusiv unul pentru 
// o comanda invalida numit UNKNOWN
    
    
    switch(requestType) {
    char *message, *res;

        case REGISTER:
        {
                printf("username=");
                char *username = calloc(1, sizeof(char));
                scanf("%s", username);

                printf("password=");
                char *password = calloc(1, sizeof(char));
                scanf("%s", password);

                char *target = create_json_object_with_credentials(username, password);
                message = compute_post_request(HOST, "/api/v1/tema/auth/register", "application/json", target, NULL);
                send_to_server(sockfd, message);
                res = receive_from_server(sockfd);

                if (strstr(res, "error")) {
                    puts("Username already in use.");
                } else {
                    puts("200 - OK - Registration Successful.");
                }
            }
break;


        case LOGIN: 
            switch (logins) {
                case 1: 
                    puts("You are already logged in.");
                    break;
                case 0: 
                {
                    char *message, *res, *username = calloc(1, sizeof(char)), *password = calloc(1, sizeof(char));

                    printf("username=");
                    scanf("%s", username);

                    printf("password=");
                    scanf("%s", password);

                    char *target = create_json_object_with_credentials(username, password);

                    message = compute_post_request(HOST, "/api/v1/tema/auth/login", "application/json", target, NULL);

                    send_to_server(sockfd, message);
                    res = receive_from_server(sockfd);

                    char *error_message = strstr(res, "error");
                    if (error_message) {
                        if(strstr(error_message, "Credentials")) {
                            puts("Incorrect username or password.");
                        } else {
                            puts("This username does not exist.");
                        }
                    } else {
                        puts("200 - OK - Welcome back! <3");

                        // extragem cookie-urile
                    cookie = strstr(res, "Set-Cookie: ");
                    cookie = strtok(cookie, ";");
                    cookie = strtok(cookie, " ");
                    cookie = strtok(NULL, " ");
                    logins ++;
                    }
                }
            }
break;



        case ENTER_LIBRARY: {
            if (logins == 0) {
                puts("You need to log in first.");
            } else {

                message = compute_get_request(HOST, "/api/v1/tema/library/access", cookie, NULL);

                send_to_server(sockfd, message);
                res = receive_from_server(sockfd);
                
                // extragem jwt-ul
                jwt = strstr(res, "token");
                jwt = strtok(jwt, "\"");
                jwt = strtok(NULL, "\"");
                jwt = strtok(NULL, "\"");

                if (jwt) {
                    puts("Library entered!");
                    library_entries ++;
                } else {
                    puts("Failed to enter library.");
                    jwt = NULL;
                }
            }
        }
break;



        case GET_BOOKS: {
            if (library_entries == 1) {

                    message = compute_get_request(HOST, "/api/v1/tema/library/books", NULL, jwt);

                    send_to_server(sockfd, message);
                    res = receive_from_server(sockfd);

                    res = extract_payload(res);
                    puts(res);
                } else {
                    puts("Access denied.");
                }
            }
break;




        case GET_BOOK: 
            if (library_entries != 1) {
                puts("Access denied.");
                break;
            }
            
            char *book_id = malloc(sizeof(char));
            printf("id=");
            scanf("%s", book_id);

            if (!verify_number(book_id)) {
                puts("Invalid ID!");
                free(book_id);
                break;
            }

            char route[100] = "/api/v1/tema/library/books/";
            strcat(route, book_id);

            message = compute_get_request(HOST, route, NULL, jwt);
            send_to_server(sockfd, message);

            res = receive_from_server(sockfd);

            if (strstr(res, "error")) {
                puts("Book not found! Please try again!");
            } else {
                puts("200 - OK - The book you are looking for is: ");
                puts(extract_payload(res));
            }
            free(book_id);
break;




        case ADD_BOOK:
            if (library_entries != 1) {
                puts("Access denied.");
                break;
            }

            char 
            *title = calloc(BIG_STRING, sizeof(char)),
            *author = calloc(BIG_STRING, sizeof(char)),
            *genre = calloc(BIG_STRING, sizeof(char)),
            *publisher = calloc(BIG_STRING, sizeof(char)),
            *page_count = calloc(BIG_STRING, sizeof(char));

            fgets(title, BIG_STRING - 1, stdin);

            const char *prompts[] = {"title", "author", "page_count", "genre", "publisher"};
            char *inputs[] = {title, author, page_count, genre, publisher};

            for(int i = 0; i < sizeof(prompts)/sizeof(prompts[0]); ++i) {
                printf("%s=", prompts[i]);
                memset(inputs[i], 0, BIG_STRING);
                fgets(inputs[i], BIG_STRING - 1, stdin);
                trim_newline(inputs[i]);
            }

            if (!verify_number_without_last(page_count)) {
                puts("Page count has to be a natural number.");
                break;
            }

            char *target = create_json_object_book(title, author, page_count, genre, publisher);
            message = compute_post_request(HOST, "/api/v1/tema/library/books", "application/json", target, jwt);


            message = compute_post_request(HOST, "/api/v1/tema/library/books", "application/json", target, jwt);
            send_to_server(sockfd, message);
            res = receive_from_server(sockfd);

            if (strstr(res, "error")) {
                puts("Invalid or incomplete information for the book!");
            } else {
                puts("200 - OK - Book added.");
            }
break;





        case DELETE_BOOK: 
        {
            if (library_entries != 1) 
            {
                puts("Access denied. ");
                break;
            }

            char *book_id = (char *)calloc(1, sizeof(char));
            printf("id=");
            scanf("%s", book_id);

            if (!verify_number(book_id)) 
            {
                printf("Invalid ID!\n");
                free(book_id);
                break;
            }

            char *route = (char *)calloc(40, sizeof(char));
            sprintf(route, "/api/v1/tema/library/books/%s", book_id);
            free(book_id);

            message = compute_delete_request(HOST, route, jwt);
            send_to_server(sockfd, message);
            res = receive_from_server(sockfd);

            if (strstr(res, "error")) 
            {
                puts("This book is not in the library.");
            } 
            else 
            {
                puts("200 - OK - Book deleted.");
            }

            free(route);
break;
        }



        case LOGOUT: {
            if(logins == 0) {
                puts("None is logged in at the moment.");
            } else {
                library_entries = 0;

                message = compute_get_request(HOST, "/api/v1/tema/auth/logout", cookie, NULL);

                send_to_server(sockfd, message);
                res = receive_from_server(sockfd);
                logins --;
                
                char *error = "error";
                if (strstr(res, error)) {
                    puts("Logout failed.");
                } else {
                    puts("200 - OK - Logged out, see you later!");
                }
            }
        }
break;
        
        case EXIT: {
            puts("Goodbye!");
            close(sockfd);
            free(client_request);
            return 0;
        }
break;

        case UNKNOWN:
        default: 
        {
            puts("Unknown request type.");
            break;
        }
    }
}
}
