#include "buffer_synchronization.h"

/**
 * Initializes buffer.
 */
void buff_init() {
  gl_buff.buff = calloc(CAPACITY, sizeof (char *));
  gl_buff.buff_size = 0;
  gl_buff.quit_flag = 0;
}

/**
 * Frees allocated memory for buffer.
 */
void buff_terminate() {
  free(gl_buff.buff);
}

/**
 * Checks if buffer is empty.
 *
 * @return returns true (non-zero) if buffer is empty
 */
int buff_empty() {
  return (gl_buff.buff_size == 0);
}

/**
 * Checks if buffer is full
 *
 * @return returns true (non-zero) if buffer is full
 */
int buff_full() {
  return (gl_buff.buff_size == CAPACITY);
}

/**
 * Writes string to buffer, if there is space. NULL otherwise.
 *
 * @param str character string
 * @return pointer to newly allocated string. NULL if unsuccessful
 */
void buff_write(const char *str) {
  char *new_str;
  
  if (buff_full()) {
    return;
  }

  if (str == NULL) {
    return;
  }

  #ifdef DEBUG
  printf("Allocating string of size %d\n", (int)strlen(str));fflush(stdout);
  #endif

  new_str = malloc(strlen(str) + 1);
  strcpy(new_str, str);

  gl_buff.buff[gl_buff.buff_size++] = (char *)new_str;

  #ifdef DEBUG
  printf("added string %s\n", gl_buff.buff[gl_buff.buff_size - 1]);fflush(stdout);
  #endif
}

/**
 * Reads most recently inserted string. Decrements counter
 *
 * @return pointer to newly allocated string. NULL if unsuccessful
 */
char *buff_read() {
  char *str;

  if (buff_empty()) {
    return NULL;
  }

  str = gl_buff.buff[--gl_buff.buff_size];

  #ifdef DEBUG
  printf("STRING %s\n", str);
  #endif

  return str;
}

/**
 * Reads lines from a file and inputs it into a global buffer
 *
 * @param file path
 * @return NULL
 */
void *thread_read_with_mutex(void *params) {
  FILE *fp;
  char *line;
  size_t length;
  ssize_t read;
  char *file;
  t_thread_parameters *p;

  p = (t_thread_parameters *)params;
  
  file = malloc(sizeof(strlen(p->file)));
  strcpy(file, p->file);

  fp = fopen (file, "r");

  if (fp != NULL) {
    read = getline (&line, &length, fp);
    while (read > -1) {
      usleep (p->sleep_time);
      if (read > 1) {
	
	while (buff_full()) {
	  printf("fill thread: could not write -- not enough space\n");
	}
	
	printf ("fill thread: writing into buffer\n");
	sem_wait (&lock);
	buff_write (line);
	read = getline (&line, &length, fp);
	sem_post (&lock);
      }
    }
    
    while (buff_full()) {
      printf("fill thread: could not write -- not enough space\n");
    }
    sem_wait (&lock);
    printf ("fill thread: writing [QUIT] into buffer\n");
    buff_write ("QUIT");
    sem_post (&lock);
  }
  fclose (fp);
  printf("fill thread: quitting\n");
  return NULL;
}

/**
 * Stores string from top of global buffer into a given file
 *
 * @param file path
 * @return NULL
 */
void *thread_write_with_mutex (void *params) {
  FILE *fp;
  char *str;
  char *file;
  t_thread_parameters *p;

  p = (t_thread_parameters *)params;
  file = malloc(sizeof(strlen(p->file)));
  strcpy(file, p->file);
  fp = fopen (file, "w+");
  if (fp != NULL) {
    while ((gl_buff.quit_flag == 0) || (!buff_empty())) {
      usleep (p->sleep_time);
      while (buff_empty()) {
	printf ("drain thread: no new strings in buffer\n");
      }
      
      sem_wait (&lock);
      str = buff_read();
      
      if ((strcmp (str, "QUIT")) == 0) {
	printf ("drain thread: read [QUIT] from buffer\n");
	gl_buff.quit_flag = 1;
      }
      else {
	printf ("drain thread: read from buffer\n");
	#ifdef DEBUG
	printf ("read '%s' size of %d\n", str, (int) strlen(str));
	#endif
	fwrite (str, 1, strlen(str), fp);
      }
    
      sem_post (&lock);
    }
  }
 
  fclose (fp);
  
  printf ("drain thread: quitting\n");
  return NULL;
}

/**
 * Reads lines from a file and inputs it into a global buffer
 *
 * @param file path
 * @return NULL
 */
void *thread_read_with_condition_variables (void *params) {
  FILE *fp;
  char *line;
  size_t length;
  ssize_t read;
  char *file;
  t_thread_parameters *p;

  p = (t_thread_parameters *)params;
  
  file = malloc(sizeof(strlen(p->file)));
  strcpy(file, p->file);

  fp = fopen (file, "r");

  if (fp != NULL) {
    read = getline (&line, &length, fp);
    while (read > -1) {
      usleep (p->sleep_time);
      if (read > 1) {
	sem_wait (&space_available);
	sem_wait (&lock);
	printf ("fill thread: writing into buffer\n");
	buff_write (line);
	read = getline (&line, &length, fp);
	sem_post (&lock);
	sem_post (&items_available);
      }
    }
    
    sem_wait (&space_available);
    sem_wait (&lock);
    printf ("fill thread: writing [QUIT] into buffer\n");
    buff_write ("QUIT");
    sem_post (&lock);
    sem_post (&items_available);
  }
  fclose (fp);
  printf("fill thread: quitting\n");
  return NULL;
}

/**
 * Stores string from top of global buffer into a given file
 *
 * @param file path
 * @return NULL
 */
void *thread_write_with_condition_variables (void *params) {
  FILE *fp;
  char *str;
  char *file;
  t_thread_parameters *p;

  p = (t_thread_parameters *)params;
  file = malloc(sizeof(strlen(p->file)));
  strcpy(file, p->file);
  fp = fopen (file, "w+");
  if (fp != NULL) {
    while ((gl_buff.quit_flag == 0) || (!buff_empty())) {
      usleep (p->sleep_time);
      
      sem_wait (&items_available);
      sem_post (&lock);
      str = buff_read();
      
      if ((strcmp(str, "QUIT")) == 0) {
	printf ("drain thread: read [QUIT] from buffer\n");
	gl_buff.quit_flag = 1;
      }
      else {
	printf ("drain thread: read from buffer\n");
	#ifdef DEBUG
	printf ("read '%s' size of %d\n", str, (int) strlen(str));
	#endif
	fwrite (str, 1, strlen(str), fp);
      }
      
      sem_post (&lock);
      sem_post (&space_available);
    }
  }
 
  fclose(fp);
  
  printf("drain thread: quitting\n");
  return NULL;
}
