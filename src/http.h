#ifndef HTTP_H
#define HTTP_H

enum status_code {
	OK = 200,
	NOT_FOUND = 404,
	INTERNAL_ERROR = 503
};
typedef enum status_code status_code_t;

struct http_response {
	char *file_name; // Nom du fichier, et non de la ressource

	char *buffer; // Buffer qui contient la réponse

	size_t length; // Taille de la ressource

	status_code_t status_code; // Status code

	int fd; // File descriptor. -1 en cas d'erreur

};

void free_http_response(struct http_response *response);

void *read_http_request(void *client_socket);

#endif // HTTP_H