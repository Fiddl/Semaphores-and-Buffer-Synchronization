#include "buffer_synchronization.h"

/**
 * @file rw.c
 * @author Nathan Edwards
 *
 * File repeatedly reads a line of text from
 * an input file, writes the line into a buffer,
 * then writes the line from the buffer to an output file.
 */
int main(int argc, char **argv) {
  char *line;
  char *str;
  char *input_path;
  char *output_path;
  FILE *input_file;
  FILE *output_file;
  size_t length;
  ssize_t read;

  if (argc == 3) {
    input_path = argv[1];
    output_path = argv[2];
    length = 0;

    input_file = fopen(input_path, (char *)"r");
    output_file = fopen(output_path, (char *)"w+");

    if (input_file == NULL || output_file == NULL) {
      return 1;
    }

    buff_init();

    while ((read = getline (&line, &length, input_file)) != -1) {
      buff_write(line);
      str = buff_read();
      #ifdef DBUG
      printf("Read: %s", str);
      #endif
      fwrite(str, 1, strlen(str), output_file);
    }

    return 0;
  }
  else {
    printf("Usage: ./rc <input file> <output file>\n\n");
    return 1;
  }
}
