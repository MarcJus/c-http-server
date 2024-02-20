#ifndef HTTP_HEADER_H
#define HTTP_HEADER_H

/**
 * @brief Renvoie une chaine de caractère contenant la ressource demandée. Doit être libéré avec free()
 * 
 * @param request Chaine de caractère contenant la request line (exemple: GET /index.html HTTP/1.1).
 * Peut être toute la requête envoyée par le client
 * @return Ressource demandée par le client
 */
char *get_request_path(const char *request);

#endif // HTTP_HEADER_H