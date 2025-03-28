#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include "server.h"
#include "http.h"

int create_server(int port){

	int ret;
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket < 0){
		return server_socket;
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	int enable = 1;
	ret = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
	if(ret < 0){
		return ret;
	}

	ret = bind(server_socket, (struct sockaddr*)&addr, sizeof(addr));
	if(ret < 0)
		return ret;
	
	ret = listen(server_socket, 1);

	return server_socket;
}

int accept_connection(int server_socket){
	int client_socket;
	int ret;
	struct sockaddr_in client_addr;
	socklen_t socklen = sizeof(client_addr);

	client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &socklen);
	if(client_socket < 0){
		return client_socket;
	}

	printf("Nouvelle connexion : %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

	pthread_t thread;
	int *p_client_socket = malloc(sizeof(int));
	*p_client_socket = client_socket;
	if(p_client_socket == NULL){
		fprintf(stderr, "Une erreur de mémoire est survenue\n");
		return -1;
	}
	ret = pthread_create(&thread, NULL, read_http_request, (void*)p_client_socket);
	if(ret < 0){
		printf("Impossible de créer un thread pour le client %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		return ret;
	}
	pthread_detach(thread);
	return 0;
}
