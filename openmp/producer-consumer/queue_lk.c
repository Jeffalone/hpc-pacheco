/* File:     queue_lk.c
 * Purpose:  Implement a queue with locks using a linked list and OpenMP.
 *           Operations are Enqueue, Dequeue, Print, Search, and Free.
 *           To be used with omp_msglk.c
 *
 * Compile:  gcc -g -Wall -DUSE_MAIN -fopenmp -o queue_lk queue_lk.c
 * Usage:    ./queue_lk
 *
 * Input:    Operations (first letter of op name) and, when necessary, keys
 * Output:   Prompts for input and results of operations
 *
 * IPP:      Section 5.8.9 (pp. 248 and ff.)
 */
#include "queue_lk.h"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

struct queue_s *Allocate_queue() {
  struct queue_s *q_p = malloc(sizeof(struct queue_s));
  q_p->enqueued = q_p->dequeued = 0;
  q_p->front_p = NULL;
  q_p->tail_p = NULL;
  omp_init_lock(&q_p->lock);
  return q_p;
} /* Allocate_queue */

/* Frees nodes in queue:  leaves queue struct allocated and lock
 * initialized */
void Free_queue(struct queue_s *q_p) {
  struct queue_node_s *curr_p = q_p->front_p;
  struct queue_node_s *temp_p;

  while (curr_p != NULL) {
    temp_p = curr_p;
    curr_p = curr_p->next_p;
    free(&(temp_p->mesg));
    free(temp_p);
  }
  q_p->enqueued = q_p->dequeued = 0;
  q_p->front_p = q_p->tail_p = NULL;
} /* Free_queue */

void Enqueue(struct queue_s *q_p, int src, char *mesg, int msg_sz) {
  struct queue_node_s *n_p = malloc(sizeof(struct queue_node_s));
  n_p->src = src;
  n_p->mesg = mesg;
	n_p->msg_sz = msg_sz;
  n_p->next_p = NULL;
  if (q_p->tail_p == NULL) { /* Empty Queue */
    q_p->front_p = n_p;
    q_p->tail_p = n_p;
  } else {
    q_p->tail_p->next_p = n_p;
    q_p->tail_p = n_p;
  }
  q_p->enqueued++;
} /* Enqueue */

int Dequeue(struct queue_s *q_p, int *src_p, char **mesg_p, int *msg_sz) {
  struct queue_node_s *temp_p;

  if (q_p->front_p == NULL)
    return 0;
  *src_p = q_p->front_p->src;
  *mesg_p = q_p->front_p->mesg;
	*msg_sz = q_p->front_p->msg_sz;
  temp_p = q_p->front_p;
  if (q_p->front_p == q_p->tail_p) /* One node in list */
    q_p->front_p = q_p->tail_p = NULL;
  else
    q_p->front_p = temp_p->next_p;
  free(temp_p);
  q_p->dequeued++;
  return 1;
} /* Dequeue */
