#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

Queue* create_queue() {
  Queue* q = calloc(1, sizeof(Queue));
  if (q == NULL) {
    fprintf(stderr, "allocation error in create_queue\n");
    exit(EXIT_FAILURE);
  }
  q->head = NULL;
  return q;
}

void free_queue(Queue* q, void (*free_func)(void*)) {
  if (q == NULL) {
    fprintf(stderr, "trying to free a NULL queue");
    exit(EXIT_FAILURE);
  }
  LinkedList_Node* current_node = q->head;
  while (current_node != NULL) {
    if (free_func != NULL) free_func(current_node->data);
    LinkedList_Node* next_node = current_node->next;
    free(current_node);
    current_node = next_node;
  }
  free(q);
}

void enqueue(Queue* q, void* data) {
  if (q == NULL) {
    fprintf(stderr, "trying to enqueue on a NULL queue\n");
    exit(EXIT_FAILURE);
  }
  LinkedList_Node* to_enqueue = calloc(1, sizeof(LinkedList_Node));
  if (to_enqueue == NULL) {
    fprintf(stderr, "allocation error in enqueue\n");
    exit(EXIT_FAILURE);
  }
  to_enqueue->data = data;
  to_enqueue->next = NULL;

  if (q->head == NULL) {
    q->head = to_enqueue;
  } else {
    LinkedList_Node* parent = q->head;
    while (parent->next != NULL) {
      parent = parent->next;
    }
    parent->next = to_enqueue;
  }
}

void* dequeue(Queue* q) {
  if (q == NULL) {
    fprintf(stderr, "trying to dequeue on a NULL queue\n");
  }
  LinkedList_Node* head = q->head;
  void* head_data = head->data;
  q->head = head->next;
  head->next = NULL;
  if (head != NULL) {
      free(head);
  }
  return head_data;
}

