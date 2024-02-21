#ifndef HTTP_HEADER_H
#define HTTP_HEADER_H

#include <sys/types.h>

enum HTTP_STATUS_CODE {
	OK=200,
	NOT_FOUND=400,
	INTERN_ERR=500
};

/**
 * @brief Renvoie une chaine de caractère contenant la ressource demandée. Doit être libéré avec free()
 * 
 * @param request Chaine de caractère contenant la request line (exemple: GET /index.html HTTP/1.1).
 * Peut être toute la requête envoyée par le client
 * @return Ressource demandée par le client
 */
char *get_request_path(const char *request);

char *build_http_header(enum HTTP_STATUS_CODE status, size_t *buf_len);

#endif // HTTP_HEADER_H