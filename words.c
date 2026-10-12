#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFF_SIZE 1024

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

  while ((read_bytes = read(fd, buff, BUFF_SIZE)) > 0) {
    write(STDOUT_FILENO, buff, read_bytes);
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
