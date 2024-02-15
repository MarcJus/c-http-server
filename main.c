#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>

#include "server.h"

#define check(value, action) if(value < 0){ action }

int main(int argc, char const *argv[]){

	int ret;
	printf("Création du serveur HTTP en cours...\n");

	ret = create_server(80);
	check(ret, perror("Erreur lors de la création du serveur"); return EXIT_FAILURE;)
	
	return 0;
}
