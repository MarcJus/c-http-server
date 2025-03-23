#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef struct node* node_t;

typedef struct queue* queue_t;

queue_t queue_init();

int queue_push(queue_t queue, int data);

int queue_pop(queue_t queue);

#endif //__QUEUE_H__