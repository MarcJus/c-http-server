#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>

#include "server.h"
#include "parse_args.h"

// usage : [-v] [-r root] [port]

int main(int argc, char* const* argv){

	int ret;
	int server_socket;
	int port;
	printf("Création du serveur HTTP en cours...\n");
	
	int parsing = parse_args(argc, argv);
	if(parsing == EXIT_FAILURE){
		free_settings();
		return EXIT_FAILURE;
	}

	port = get_int_setting(SETTING_PORT);

	server_socket = create_server(port);
	if(server_socket < 0){
		perror("Erreur lors de la création du serveur");
		free_settings();
		return EXIT_FAILURE;
	}

	printf("Serveur créé sur le port %d!\n", port);

	while(1){
		printf("En attente d'une connexion...\n");
		ret = accept_connection(server_socket);
		if(ret < 0){
			perror("Une erreur s'est produite lors de la connexion");
			free_settings();
			return EXIT_FAILURE;
		}
	}

	return 0;
}
