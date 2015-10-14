/**
 * @file buffer_synchronization.h
 * @author Nathan Edwards
 * @date 13 Oct 2015
 * @brief Concurrent programming assignment exploring buffer synchronization
 */ 

#ifndef _BUFFER_SYNCHRONIZATION_H_
#define _BUFFER_SYNCHRONIZATION_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define CAPACITY 20 /* number of total elements allowed in the buffer */
#define BYTES 32

typedef struct s_buffer {
  char **buff; /* buffer */
  int buff_size; /* amount of elements in the buffer */
  int quit_flag; /* flag to indicate that no more strings will be fed to buffer */
} t_buff;

typedef struct s_thread_parameters {
  int sleep_time; /* sleep time to be used */
  char *file; /* file name for output/input */
} t_thread_parameters;

t_buff gl_buff;

sem_t lock;
sem_t space_available;
sem_t items_available;

void buff_init();
void buff_terminate();
int buff_empty();
int buff_full();
void buff_write(const char *);
char *buff_read();
void *thread_read_with_mutex(void *ptr);
void *thread_write_with_mutex(void *ptr);
void *thread_read_with_condition_variables(void *ptr);
void *thread_write_with_condition_variables(void *ptr);
#endif
