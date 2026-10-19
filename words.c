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
void read_file_dl(const char *path, WordCounter **words, size_t *words_counter);
void recursive_dir_traverse(const char *path, WordCounter **words, size_t *words_counter);
int compare_words_alpha(const void *a, const void *b);

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
            read_file_dl(path, &words, &words_counter);
        }
        else if (S_ISDIR(data.st_mode)) // directory
        {
            recursive_dir_traverse(path, &words, &words_counter);
        }
    }

    // Sorting by count goes here
    qsort(words, words_counter, sizeof(WordCounter), compare_words_alpha);
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

void read_file_dl(const char *base_path,
                  WordCounter **words,
                  size_t *words_counter)
{
    char buffer[BUFFSIZE];
    ssize_t bytes;

    int fd = open(base_path, O_RDONLY);
    if (fd < 0)
    {
        perror("Open file error");
        exit(EXIT_FAILURE);
    }

    char *word = NULL;
    size_t word_length = 0;

    int pending_hyphen = 0;
    int has_letter = 0;

    while ((bytes = read(fd, buffer, BUFFSIZE)) > 0)
    {
        for (ssize_t j = 0; j < bytes; j++)
        {
            char c = buffer[j];

            int is_letter =
                (c >= 'A' && c <= 'Z') ||
                (c >= 'a' && c <= 'z');

            // Step 1: Resolve a previous hyphen
            if (pending_hyphen)
            {
                if (is_letter)
                {
                    word = add_char(word, &word_length, '-');
                }
                else
                {
                    if (has_letter)
                    {
                        add_word(words, words_counter, word);
                    }

                    free(word);
                    word = NULL;
                    word_length = 0;
                    has_letter = 0;
                }

                pending_hyphen = 0;
            }

            // Step 2: Process a letter
            if (is_letter)
            {
                word = add_char(word, &word_length, c);
                has_letter = 1;
            }

            // Step 3: Process an apostrophe
            else if (c == '\'')
            {
                word = add_char(word, &word_length, c);
            }

            // Step 4: Process a hyphen
            else if (c == '-' && word_length > 0 &&
                     ((word[word_length - 1] >= 'A' &&
                       word[word_length - 1] <= 'Z') ||
                      (word[word_length - 1] >= 'a' &&
                       word[word_length - 1] <= 'z')))
            {
                pending_hyphen = 1;
            }

            // Step 5: Process a separator
            else
            {
                if (has_letter)
                {
                    add_word(words, words_counter, word);
                }

                free(word);
                word = NULL;
                word_length = 0;
                has_letter = 0;
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

    // Process the last word
    if (has_letter)
    {
        add_word(words, words_counter, word);
    }

    free(word);
    close(fd);
}

void recursive_dir_traverse(const char *path, WordCounter **words, size_t *words_counter)
{
    DIR *dirp = opendir(path);
    if (!dirp)
    {
        perror(path);
        exit(EXIT_FAILURE);
    }
    struct dirent *de;

    while ((de = readdir(dirp)) != NULL)
    {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
        {
            continue;
        }
        size_t full_path_len = strlen(path) + strlen(de->d_name) + 2;
        char *full_path = malloc(full_path_len);
        if (full_path == NULL)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        snprintf(full_path, full_path_len, "%s/%s", path, de->d_name);

        struct stat data;

        if (lstat(full_path, &data) < 0)
        {
            perror("full path");
            free(full_path);
            exit(EXIT_FAILURE);
        }

        if (S_ISREG(data.st_mode))
        {
            read_file_dl(full_path, words, words_counter);
        }
        else if (S_ISDIR(data.st_mode))
        {
            recursive_dir_traverse(full_path, words, words_counter);
        }
        free(full_path);
    }
    closedir(dirp);
}

int compare_words_alpha(const void *a, const void *b)
{
    const WordCounter *w1 = a;
    const WordCounter *w2 = b;

    return strcmp(w1->word, w2->word);
}
