#ifndef QUEUE_H
#define QUEUE_H


typedef struct LinkedList_Node {
  void* data;
  struct LinkedList_Node* next;
} LinkedList_Node;

typedef struct Queue {
  LinkedList_Node* head;
} Queue;

Queue* create_queue();
void free_queue(Queue* q, void (*free_func)(void*));
void enqueue(Queue* q, void* data);
void* dequeue(Queue* q);

#endif
