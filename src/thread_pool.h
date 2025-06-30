#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

int create_thread_pool();
void add_new_client(int client_socket);

/**
 * @brief Permet d'obtenir la valeur du client socket
 * 
 * @return Socket du client ou 0 s'il a déjà été récupéré
 */
int get_client();

#endif //__THREAD_POOL_H__