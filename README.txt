CS214 Project 2: Word occurence count
--------------------------------------------------------------------------------------------------------
AUTHORS
Name: Depak Borhara
NetID: db1532
--------------------------------------------------------------------------------------------------------
OVERVIEW
This project implements a word occurrence counter in C.
The program reads one or more files and directories and counts each distinct word.
It combines the word counts from all processed files into one list.

The program:
- Reads files with open(), read(), and close().
- Traverses directories with opendir(), readdir(), and closedir().
- Uses a suffix filter for files found during directory traversal.
- Stores words and their occurrence counts in a dynamic array.
- Sorts words by decreasing occurrence count.
- Uses strcmp() to break ties in count.
--------------------------------------------------------------------------------------------------------
WORD STORAGE
The WordCounter structure contains:
char *word:  
    A pointer to a dynamically allocated string.
int count: 
    The number of times that word appeared in all input files.

WordCounter *words:
    points to a dynamic array of WordCounter structures.
size_t words_counter:
    stores the number of distinct words in that array.

add_char():
    Expands a temporary string with realloc() as letters are read.

add_word():
Searches the array for an existing word:
    - If the word already exists, its increments count by one.
    - If the word is new, realloc() expands the array and malloc() stores a copy of the word.
    - strcmp() makes uppercase and lowercase words distinct.

This design favors simple and modular code. 
Searching for an existing word takes time proportional to the number of distinct words already stored.
--------------------------------------------------------------------------------------------------------
FILE READING
read_file_dl():
- Opens a file and reads it in blocks of BUFFSIZE (1024) bytes.
- An outer loop reads each block. An inner loop examines each byte.
- The temporary word remains allocated between read() calls.
This lets the program process a word that crosses a buffer boundary.

Word rules:
- ASCII letters A-Z and a-z are part of words.
- A straight apostrophe (') can occur within a word, including at the start or end.
- A hyphen (-) is part of a word only when it is surrounded by two letters.
- Numbers, whitespace, and other punctuation separate words.
- Uppercase and lowercase letters are different for purposes of counting.

Examples:
foo-bar     -> one word: foo-bar
foo--bar    -> two words: foo and bar
foo-        -> one word: foo
-foo        -> one word: foo
a2z         -> two words: a and z
Foo foo     -> two distinct words: Foo and foo

The pending_hyphen flag delays the decision about a hyphen until the next byte is read.
The has_letter flag prevents punctuation-only strings from being added as words.

--------------------------------------------------------------------------------------------------------
DIRECTORY TRAVERSAL
recursive_dir_traverse():
    Opens a directory and examines its entries.
    It builds each entry's path from the directory path and the entry name.

For each entry:
- A regular file is read only if its name matches the selected suffix.
    - If no suffix is given it defaults to .txt file type.
- A subdirectory is searched recursively with the same suffix.
- A name beginning with a period (.) is skipped during directory traversal.
- lstat() checks entries without following symbolic links.
    - In order to remove the warning from the c99 standard flag at compilation, 
    an additional -D_POSIX_C_SOURCE=200809L flag was added to the make file flags, 
    but is not required to compile.

Explicit file arguments are always read, regardless of their suffix or leading period.
The suffix filter applies only to files discovered during directory traversal.

--------------------------------------------------------------------------------------------------------
FILE SUFFIX OPTION
The default suffix is .txt.
Users can supply a different suffix with -s.

Examples:
./words docs                  Use .txt for directory traversal.
./words -s.md docs            Use .md for directory traversal.
./words -s .md docs           Same as the previous command.
./words -s md docs            Match filenames ending in md; do not add a period.
./words -s.md example.txt     Read the explicit file example.txt regardless of suffix.

has_suffix(): 
    compares the end of a filename to the requested suffix.
It first checks whether the filename is long enough to contain that suffix.
--------------------------------------------------------------------------------------------------------
SORTING AND OUTPUT
After all input paths have been processed, main() calls qsort().
The comparison function sorts in this order:
1. Higher occurrence count first.
2. When counts match, lexicographical order using strcmp().

The output format is one word and count per line, separated by a space.

Example input:
spam eggs bacon spam

Expected output:
spam 2
bacon 1
eggs 1
--------------------------------------------------------------------------------------------------------
MEMORY MANAGEMENT
- add_char(): 
    allocates and resizes the temporary word buffer.
- add_word(): 
    allocates space for the WordCounter array and stored strings.
- read_file_dl(): 
    frees its temporary word after storing the word count.
- recursive_dir_traverse():
    frees each constructed pathname.
- main():
    frees each stored string and then frees the WordCounter array.

Dynamic allocation allows the program to process files and words without a fixed
maximum length. Memory allocation failures cause the program to report an error and exit.
--------------------------------------------------------------------------------------------------------
TESTING STRATEGY
The test cases below check word parsing, combined counts, traversal, filtering,
sorting, and error handling. Use small files with known results for correctness.
Also use longer files to check buffer boundaries and dynamic memory allocation.

TEST 1: Basic word counts and sorting
Input: spam eggs bacon spam
Expected: spam 2, followed by bacon 1 and eggs 1.
Purpose: Verify counting, decreasing count order, and lexicographical tie-breaking.

TEST 2: Capitalization
Input: Foo foo Foo
Expected: Foo 2 and foo 1.
Purpose: Verify that uppercase and lowercase words are distinct.

TEST 3: Hyphens and separators
Input: foo-bar foo--bar foo- -foo a2z
Expected: foo 3, followed by a 1, bar 1, foo-bar 1, and z 1.
Purpose: Verify a valid internal hyphen, invalid hyphens, and digits as separators.

TEST 4: Straight apostrophes
Input: 'oo can't rude'
Expected: 'oo 1, can't 1, rude' 1, in lexicographical order.
Purpose: Verify apostrophes at the start, middle, and end of words.

TEST 5: Multiple input files
File 1: apple apple banana
File 2: apple banana cherry
Expected: apple 3, banana 2, cherry 1.
Purpose: Verify that counts from different files share one WordCounter array.

TEST 6: Recursive directory traversal
Place .txt files in a directory and in a nested subdirectory.
Use repeated words in both files.
Purpose: Verify nested traversal and combined counts across directory levels.

TEST 7: Default suffix
Place .txt, .md, and .c files inside one directory. Run ./words directory.
Expected: Only the .txt files found during traversal contribute words.
Purpose: Verify the default suffix.

TEST 8: Explicit suffix formats
Run ./words -s.md directory and ./words -s .md directory.
Expected: The two commands produce the same result.
Purpose: Verify both accepted forms of the -s option.

TEST 9: Suffix without a period
Use files named notes.md and othermd. Run ./words -s md directory.
Expected: Both filenames match because both end in md.
Purpose: Verify that the program does not insert a period.

TEST 10: Explicit file argument
Run ./words -s.md example.txt.
Expected: example.txt is processed despite the .md suffix selection.
Purpose: Verify that suffix filtering does not apply to explicit file arguments.

TEST 11: Hidden entries
Put .hidden.txt and a .hidden-directory inside a traversed directory.
Also put a visible .txt file in that directory.
Expected: Hidden entries are skipped; the visible file is processed.
Purpose: Verify that traversal ignores names beginning with a period.

TEST 12: Buffer boundaries and long words
Create a file longer than 1024 bytes with a word that crosses a read() boundary.
Expected: The complete word is counted once.
Purpose: Verify that temporary word state persists across read() calls.

TEST 13: Empty files and invalid arguments
Test an empty file, a nonexistent path, -s without a suffix, and -s.md without a path.
Expected: An empty file produces no word output. Invalid arguments or paths report errors.
Purpose: Verify end-of-file handling and argument validation.
--------------------------------------------------------------------------------------------------------
BUILDING
Run: make
Builds the words executable from words.c using the provided Makefile.

Run: make clean
Removes the words executable.

The Makefile can use -std=c99 and enable AddressSanitizer and
UndefinedBehaviorSanitizer during testing.
For lstat() under strict C99 on Linux, the Makefile uses -D_POSIX_C_SOURCE=200809L 
to hide the warning.

--------------------------------------------------------------------------------------------------------
RUNNING
./words example.txt
./words docs
./words -s.md docs
./words -s .md docs
./words -s md docs
./words -s.md example.txt docs

Output is sent to standard output. Errors are sent to standard error.

--------------------------------------------------------------------------------------------------------
KNOWN LIMITATIONS
- The byte-oriented parser recognizes straight ASCII apostrophes (') but not the UTF-8
  curly apostrophe U+2019 shown in the project example. That case needs separate handling.