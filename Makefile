CC = gcc
CFLAGS = -std=c99 -g -Wall -fsanitize=address -fsanitize=undefined -Wextra -D_POSIX_C_SOURCE=200809L

words: words.c
	$(CC) $(CFLAGS) words.c -o words

clean:
	rm -f words
