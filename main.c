#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>

#include "server.h"

int main(int argc, char const *argv[]){

	int ret;
	int server_socket;
	printf("Création du serveur HTTP en cours...\n");

	server_socket = create_server(80);
	if(server_socket < 0){
		perror("Erreur lors de la création du serveur");
		return EXIT_FAILURE;
	}

	printf("Serveur créé !\n");

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
