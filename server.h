#ifndef SERVER_H
#define SERVER_H

int create_server(int port);

int accept_connection(int server_socket);

#endif // SERVER_H