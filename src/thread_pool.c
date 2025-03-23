#include <stdlib.h>
#include <pthread.h>

#include "thread_pool.h"
#include "http.h"

#define THREAD_POOL_SIZE	5

pthread_t threads_list[THREAD_POOL_SIZE] = {};
pthread_mutex_t pool_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pool_cond = PTHREAD_COND_INITIALIZER;

int active_threads = 0;
int client_socket;

int create_thread_pool(){
	for(int i = 0; i < THREAD_POOL_SIZE; i++){
		if(pthread_create(&threads_list[i], NULL, read_http_request, NULL) < 0)
			return -1;
	}
	return 0;
}

int add_new_client(int sock){
	pthread_mutex_lock(&pool_mutex);
	client_socket = sock;
	pthread_cond_signal(&pool_cond);
	pthread_mutex_unlock(&pool_mutex);
}

int get_client(){
	int ret = 0;

	pthread_mutex_lock(&pool_mutex);
	pthread_cond_wait(&pool_cond, &pool_mutex);
	ret = client_socket;
	client_socket = 0;
	pthread_mutex_unlock(&pool_mutex);

	return ret;
}