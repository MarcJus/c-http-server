#include "server.h"
#include <sys/socket.h>
#include <netinet/in.h>

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

	ret = bind(server_socket, (struct sockaddr*)&addr, sizeof(addr));
	if(ret < 0)
		return ret;
	
	ret = listen(server_socket, 1);

	return 0;
}
