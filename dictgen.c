/*
 * Brian Chrzanowski
 * Tue Nov 20, 2018 22:43
 *
 * program to generate wordlists based on passed in command line rules
 *
 * to build:
 *
 *		gcc dict_gen.c -o dictgen
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char list_upper[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};

const char list_lower[] = {
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
};

const char list_numeric[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
const char list_symbol[] = {'!', '@', '#', '$', '%', '^', '&', '*', '(', ')'};

#define LIST_UPPER 0x01
#define LIST_LOWER 0x02
#define LIST_NUMER 0x04
#define LIST_SYMBL 0x08
#define ARGS_ERR   0x10
#define DEFAULT_FLAGS (LIST_UPPER | LIST_NUMER)
#define MAX_WORDS  16

void print_list(char *list, int listlen, int wordlen);
void fill_table(char *wordlist, int len, unsigned int flags);
unsigned int parse_args(int argc, char **argv, int *max_len);
int next_word(int *arr, int wordlen, int max);

int main(int argc, char **argv)
{
	int max_len;
	unsigned int flags;
	char buffer[1024] = {0};

	flags = parse_args(argc, argv, &max_len);

	if (!(flags & ARGS_ERR)) {
		fill_table(buffer, sizeof(buffer), flags);
		print_list(buffer, strlen(buffer), max_len);
	}

	return 0;
}

/* print_list : generates words of maximum length from the charlist input */
void print_list(char *list, int listlen, int wordlen)
{
	int curr_list[MAX_WORDS] = {0};
	int curr_len, last;
	int i;
	char outbuf[MAX_WORDS * 2] = {0};

	for (curr_len = 0; curr_len < wordlen; curr_len++) {
		last = 1;
		memset(curr_list, 0, sizeof(int) * MAX_WORDS);
		for (;;) {
			for (i = 0; i < curr_len + 1; i++) { // create the string
				outbuf[i] = list[curr_list[i]];
			}

			printf("%s\n", outbuf); // print the string

			if (!last)
				break;

			// get the next word
			last = next_word(curr_list, curr_len, listlen);
		}
	}
}

/* next_word : set the int* array with the next chars, care for overflow */
int next_word(int *arr, int wordlen, int max)
{
	/* 
	 * arr - the array holding wordlist indicies
	 * wordlen - the length of the current word (0 indexed)
	 * max - the maximum value that arr[idx] should ever be
	 *
	 * returns true if this is the last word that should be generated
	 */

	int i, last;

	arr[wordlen]++;

	// handle the carry over
	for (i = wordlen; 0 <= i; i--) {
		if (arr[i] == max) {
			arr[i] = 0;

			if (0 <= i - 1) {
				arr[i - 1]++;
			}
		}
	}

	// check if everything is at the final index
	for (i = 0, last = 0; i < wordlen + 1; i++) {
		if (arr[i] != max - 1) {
			last = 1;
			break;
		}
	}

	return last;
}

/* fill_table : filling the chartable with the required symbols */
void fill_table(char *wordlist, int len, unsigned int flags)
{
	// memcpy one by one, checking if we have enough space
	char *ptr;
	ptr = wordlist;

	if (flags & LIST_UPPER) {
		memcpy(ptr, list_upper, sizeof(list_upper));
		ptr += sizeof(list_upper);
	}

	if (flags & LIST_LOWER) {
		memcpy(ptr, list_lower, sizeof(list_lower));
		ptr += sizeof(list_upper);
	}

	if (flags & LIST_NUMER) {
		memcpy(ptr, list_numeric, sizeof(list_numeric));
		ptr += sizeof(list_numeric);
	}

	if (flags & LIST_SYMBL) {
		memcpy(ptr, list_symbol, sizeof(list_symbol));
		ptr += sizeof(list_numeric);
	}
}

unsigned int parse_args(int argc, char **argv, int *max_len)
{
	char *s, *prog;
	int flags;

	flags = 0;
	prog = argv[0];

	while (--argc > 0 && (*++argv)[0] == '-') {
		for (s = argv[0] + 1; *s != '\0'; s++) {
			switch (*s) {
				case 'u':
					flags |= LIST_UPPER;
					break;

				case 'l':
					flags |= LIST_LOWER;
					break;

				case 'n':
					flags |= LIST_NUMER;
					break;

				case 's':
					flags |= LIST_SYMBL;
					break;

				default:
					printf("Illegal option %c\n", *s);
					argc = -1;
					break;
			}
		}
	}

	if (argc != 1) {
		fprintf(stderr, "Usage: %s [-u, -l, -n, -s] length\n", prog);
		flags |= ARGS_ERR;
	} else {
		*max_len = atoi(argv[0]);

		if (flags == 0) {
			flags = DEFAULT_FLAGS;
		}
	}

	return flags;
}

