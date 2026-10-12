CC = gcc
CFLAGS = -std=c99 -g -Wall -fsanitize=address -fsanitize=undefined -Wextra

words: words.c
	$(CC) $(CFLAGS) words.c -o words

clean:
	rm -f words
