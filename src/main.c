#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>

#include "server.h"

#define DEFAULT_HTTP_PORT	80

int main(int argc, char const *argv[]){

	int ret;
	int server_socket;
	int port = DEFAULT_HTTP_PORT;
	printf("Création du serveur HTTP en cours...\n");
	
	if(argc > 1){
		port = atoi(argv[1]);
		if(port == 0){ // erreur atoi()
			port = DEFAULT_HTTP_PORT;
		}
	}

	server_socket = create_server(port);
	if(server_socket < 0){
		perror("Erreur lors de la création du serveur");
		return EXIT_FAILURE;
	}

	printf("Serveur créé sur le port %d!\n", port);

	while(1){
		printf("En attente d'une connexion...\n");
		ret = accept_connection(server_socket);
		if(ret < 0){
			perror("Une erreur s'est produite lors de la connexion");
			return EXIT_FAILURE;
		}
	}

	return 0;
}
