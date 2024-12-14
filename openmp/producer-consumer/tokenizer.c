#include "queue_lk.h"
#include <dirent.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_DIR "./data/"
#define MAX_PRODUCERS 8
#define MAX_CONSUMERS 16
#define MAX_FILE_NAME 256

int Get_files(char files[MAX_PRODUCERS][MAX_FILE_NAME]);
void Add_line(struct queue_s *queue, int my_rank, char *line);
int Get_line(struct queue_s *queue, int my_rank, char **line);
void Usage(char *prog_name);
void Produce(struct queue_s *queue, char *file);
void Tokenize(const char *line, int length, int rank);

int main(int argc, char *argv[]) {
  int num_producers, num_consumers, thread_count, done_sending;
  if (argc != 2) {
    Usage(argv[0]);
  }
  num_consumers = strtol(argv[1], NULL, 10);
  if (num_consumers > MAX_CONSUMERS || num_consumers <= 0) {
    perror("Num_consumers supplied not in bounds");
    Usage(argv[0]);
  }

  char files[MAX_PRODUCERS][MAX_FILE_NAME];
  num_producers = Get_files(files);
  thread_count = num_consumers + num_producers;
  done_sending = 0;
  struct queue_s *msg_queue = Allocate_queue();
  printf("Starting with %d producers %d consumers for %d total threads\n",
         num_producers, num_consumers, thread_count);
  printf("Reading from files:\n");
  for (int i = 0; i < num_producers; i++) {
    printf("%s\n", files[i]);
  }
#pragma omp parallel num_threads(thread_count) default(none)                   \
    shared(thread_count, msg_queue, done_sending, num_consumers,               \
               num_producers, files)
  {
    int my_rank = omp_get_thread_num();
    if (my_rank < num_producers) {
#ifdef DEBUG
      printf("Rank: %d is producer with file %s\n", my_rank, files[my_rank]);
#endif
      Produce(msg_queue, files[my_rank]);
#pragma omp critical
      done_sending++;
    } else {
#ifdef DEBUG
      printf("Rank: %d is consumer\n", my_rank);
#endif
      char *line = NULL;
      int size = 0;
      while (done_sending < num_producers ||
             (msg_queue->enqueued - msg_queue->dequeued) != 0) {
        line = NULL;
        size = Get_line(msg_queue, my_rank, &line);
        if (line == NULL)
          continue; // no data to tokenize
#ifdef DEBUG
        printf("Thread %d dequed %s with size %d\n", my_rank, line, size);
#endif 
        Tokenize(line, size, my_rank);
        free(line);
      }
    }
  }
  Free_queue(msg_queue);
  return 0;
}
/*
 * fills files array with pointers to regular files that are contained in the
 * FILE_DIR directory returns number of files found
 */
int Get_files(char files[MAX_PRODUCERS][MAX_FILE_NAME]) {
  struct dirent *de;
  int num_files = 0;
  DIR *dr = opendir(FILE_DIR);

  if (dr == NULL)
    return 0;

  while ((de = readdir(dr)) != NULL && num_files < MAX_PRODUCERS) {
    if (de->d_type == DT_REG) {
      // TODO: Fix memory safety.
      strcpy(files[num_files], FILE_DIR);
      strcat(files[num_files], de->d_name);
      num_files++;
    }
  }
  closedir(dr);
  return num_files;
}
void Usage(char *prog_name) {
  fprintf(stderr, "usage: %s <number of consumers> \n", prog_name);
  fprintf(stderr, "   number of consumers = number of threads dedicated to "
                  "tokenizing output min 1.\n Number of producers is "
                  "determined by number of files in ./data/\n");
  exit(0);
}
/* enqueues char *line into queue referenced by struct queue_s* queue atomically
 */
void Add_line(struct queue_s *queue, int my_rank, char *line) {
  if (queue == NULL) {
    perror("Add_line recieved NULL queue");
    return;
  }
  int size = strlen(line);
  omp_set_lock(&queue->lock);
  Enqueue(queue, my_rank, line, size);
  omp_unset_lock(&queue->lock);
}

/* Gets the first line from the queue referenced in queue  atomically and
 * returns it in the char** line. returns size of the line recieved.
 */

int Get_line(struct queue_s *queue, int my_rank, char **line) {
  int src, msg_size;
  int queue_size = queue->enqueued - queue->dequeued;

  if (queue_size == 0)
    return 0;
  else if (queue_size == 1) {
    omp_set_lock(&queue->lock);
    Dequeue(queue, &src, line, &msg_size);
    omp_unset_lock(&queue->lock);
  } else {
    Dequeue(queue, &src, line, &msg_size);
  }
  return msg_size;
}

/* Enqueues all lines of a file into the queue referenced by queue
 */
void Produce(struct queue_s *queue, char *file) {
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  fp = fopen(file, "r");
  while ((read = getline(&line, &len, fp)) != -1) {
    Add_line(queue, 0, line);
    line = NULL; // ensure new line is allocated
  }
  fclose(fp);
}
/*Tokenizes the line passed into it and prints it to standard out*/
void Tokenize(const char *line, int size, int rank) {
  char *token;
  char *temp;
  char *copy;
  // copying here may be uneccessary, but to be extra cautious not to mangle the
  // string with strtok_r I copy.
  copy = malloc((size + 1) * sizeof(char));
  strcpy(copy, line);
  token = strtok_r(copy, " ", &temp);

  while (token != NULL) {
    token[strcspn(token, "\n")] = 0; // trim newline if present.
    printf("Rank %d found token: %s\n", rank, token);
    token = strtok_r(NULL, " ", &temp);
  }
  free(copy);
}
