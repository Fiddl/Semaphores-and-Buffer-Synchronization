#include "buffer_synchronization.h"

/**
 * @file transfer2.c
 * @author Nathan Edwards
 *
 * Creates two threads to be executed simulatneously.
 * Fill thread reads lines of text from an input file
 * and writes it into a buffer. Drain thread copies the
 * lines from the buffer then writes them to the output
 * file. Syncrhonized over a mutex for locking with two
 * condition variables "spaces_available" and
 * "items_available".
 */
int main (int argc, char **argv) {
  t_thread_parameters *fill_parameters;
  t_thread_parameters *drain_parameters;
  pthread_t fill;
  pthread_t drain;

  if (argc == 5) {
    buff_init();
    sem_init (&lock, 0, 1);
    sem_init (&space_available, 0, CAPACITY);
    sem_init (&items_available, 0, 0);
    fill_parameters = malloc (sizeof(t_thread_parameters));
    drain_parameters = malloc (sizeof(t_thread_parameters));
    fill_parameters->file = malloc (sizeof(strlen(argv[1])));
    strcpy(fill_parameters->file, argv[1]);
    drain_parameters->file = (char *)argv[2];
    fill_parameters->sleep_time = atoi (argv[3]);
    drain_parameters->sleep_time = atoi (argv[4]);

    printf ("Buffer capacity is %d\n", CAPACITY);
    pthread_create (&fill, 
		    NULL,
		    thread_read_with_condition_variables,
		    (void *)fill_parameters);
    pthread_create (&drain,
		    NULL,
		    thread_write_with_condition_variables,
		    (void *)drain_parameters);
    
    pthread_join (fill, NULL);
    pthread_join (drain, NULL);

    
    printf ("Exiting\n");
    pthread_exit(NULL);
  }
  else {
    printf("./transfer1 <input file> <output file> <fill sleep> <drain sleep>\n");
    return 1;
  }
}
