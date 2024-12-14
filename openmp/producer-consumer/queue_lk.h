/* File:     queue_lk.h
 * Purpose:  Header file for queue_lk.c, which implements a queue with 
 *           OpenMP locks
 */
#ifndef _QUEUE_LK_H_
#define _QUEUE_LK_H_
#include <omp.h>

struct queue_node_s {
   int src;
   char* mesg;
	 int msg_sz;
   struct queue_node_s* next_p;
};

struct queue_s{
   omp_lock_t lock;
   int enqueued;
   int dequeued;
   struct queue_node_s* front_p;
   struct queue_node_s* tail_p;
};

struct queue_s* Allocate_queue(void);
void Free_queue(struct queue_s* q_p);
void Enqueue(struct queue_s* q_p, int src, char* mesg, int msg_sz);
int Dequeue(struct queue_s* q_p, int* src_p, char** mesg_p, int* msg_sz);

#endif
