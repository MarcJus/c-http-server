#include <stdlib.h>

#include "queue.h"

struct node {
	int data;
	node_t next;
};

struct queue {
	node_t head;
	node_t tail;
};

queue_t queue_init(){
	queue_t queue = malloc(sizeof(struct queue));
	queue->head = NULL;
	queue->tail = NULL;
	return queue;
}

int queue_push(queue_t queue, int data){
	node_t node = malloc(sizeof(struct node));
	if(node == NULL)
		return -1;

	node->data = data;
	node->next = NULL;
	if(queue->tail != NULL){
		queue->tail->next = node;
	}
	queue->tail = node;

	if(queue->head == NULL){
		queue->head = node;
	}
	return 0;
}

int queue_pop(queue_t queue){
	node_t temp = queue->head;
	if(temp == NULL){
		return 0;
	}

	queue->head = temp->next;
	if(queue->head == NULL)
		queue->tail = NULL;

	int result = temp->data;
	free(temp);
	return result;
}