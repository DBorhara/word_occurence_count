#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

#define BUFFSIZE 1024

typedef struct
{
    char *word;
    int count;
} WordCounter;

char *add_char(char *word, size_t *word_len, char c);
void add_word(WordCounter **words, size_t *words_counter, char *word);

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        char msg[] = "Invalid number of arguments\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        exit(EXIT_FAILURE);
    }

    WordCounter *words = NULL;
    size_t words_counter = 0;

    // Master argv loop
    for (int i = 1; i < argc; i++)
    {
        char const *path = argv[i];

        // Declare stat struct
        struct stat data;

        int r = stat(path, &data);
        if (r < 0)
        {
            perror(path);
            exit(EXIT_FAILURE);
        }

        // Check file file

        if (S_ISREG(data.st_mode)) // regular file
        {
            char buffer[BUFFSIZE];
            ssize_t bytes;
            int fd = open(path, O_RDONLY);
            if (fd < 0)
            {
                perror("Open file error");
                exit(EXIT_FAILURE);
            }

            char *word = NULL;
            size_t word_length = 0;
            // The Double Loop
            while ((bytes = read(fd, buffer, BUFFSIZE)) > 0)
            {
                for (ssize_t j = 0; j < bytes; j++)
                {
                    char c = buffer[j];
                    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
                    {
                        word = add_char(word, &word_length, c);
                    }
                    else
                    {
                        if (word_length == 0)
                        {
                            continue;
                        }
                        add_word(&words, &words_counter, word);
                        free(word);
                        word = NULL;
                        word_length = 0;
                    }
                }
            }
            if (bytes < 0)
            {
                perror("read");
                free(word);
                close(fd);
                exit(EXIT_FAILURE);
            }
            if (word_length > 0)
            {
                add_word(&words, &words_counter, word);
                free(word);
                word = NULL;
                word_length = 0;
            }
            close(fd);
        }
        else if (S_ISDIR(data.st_mode)) // directory
        {
            DIR *dirp = opendir(path);
            if (!dirp)
            {
                perror(path);
                exit(EXIT_FAILURE);
            }
            struct dirent *de;

            while (de = readdir(dirp))
            {
                //read file and add to count until no more NULL
            }
            

        }
        else
        {
            // something else
        }
    }

    // Sorting by count goes here
    
    // Print Test
    for (size_t i = 0; i < words_counter; i++)
    {
        printf("%s %d\n", words[i].word, words[i].count);
    }
    return 0;
}

char *add_char(char *word, size_t *word_len, char c)
{
    char *temp = realloc(word, *word_len + 2);

    if (temp == NULL)
    {
        perror("realloc");
        free(word);
        exit(EXIT_FAILURE);
    }

    word = temp;

    word[*word_len] = c;
    (*word_len)++;

    word[*word_len] = '\0';

    return word;
}

void add_word(WordCounter **words, size_t *words_counter, char *word)
{
    for (size_t i = 0; i < *words_counter; i++)
    {
        if (strcmp((*words)[i].word, word) == 0)
        {
            (*words)[i].count++;
            return;
        }
    }

    // New space in word counter
    WordCounter *temp = realloc(*words, (*words_counter + 1) * sizeof(WordCounter));

    if (temp == NULL)
    {
        perror("Memory Reallocation failed in add_word");
        exit(EXIT_FAILURE);
    }

    *words = temp;

    (*words)[*words_counter].word = malloc(strlen(word) + 1);

    if ((*words)[*words_counter].word == NULL)
    {
        perror("Memory allocation failed in add_word");
        exit(EXIT_FAILURE);
    }
    strcpy((*words)[*words_counter].word, word);

    (*words)[*words_counter].count = 1;

    (*words_counter)++;
}
