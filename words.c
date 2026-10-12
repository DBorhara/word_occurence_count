#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFF_SIZE 1024

typedef struct {
  char *word;
  int count;
} WordCounter;

int is_letter(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

char *add_letter(char *word, size_t *word_length, char c) {

  char *temp =
      realloc(word, *word_length + 2); // +2 for new char and terminator
}

int main(int argc, char **argv) {
  if (argc < 2) {
    char err[] = "Not enough arguments.\n";
    write(STDERR_FILENO, err, sizeof(err) - 1);
    return EXIT_FAILURE;
  }
  int fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    perror("open");
    return EXIT_FAILURE;
  }

  char buff[BUFF_SIZE];
  ssize_t read_bytes;
  WordCounter *words = NULL;
  size_t word_counter;

  char *current_word = NULL;
  size_t current_word_len = 0;

  while ((read_bytes = read(fd, buff, BUFF_SIZE)) > 0) {
    for (ssize_t i = 0; i < read_bytes; i++) {
      char c = buff[i];
      if (is_letter(c)) {
        // Add c to current word and increase word len
      } else {
        // clear current_word and reset length
      }
    }
  }
  if (read_bytes == -1) {
    perror("read");
    close(fd);
    return EXIT_FAILURE;
  }

  if (close(fd) == -1) {
    perror("close");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
