/*
 * Brian Chrzanowski
 * Wed Oct 05, 2016 21:47
 *
 * This program is meant to be a filter between the passing program, and
 * whatever password hashing program has been implemented.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

#define UPPER_NEEDED 1
#define LOWER_NEEDED 1
#define NUMBER_NEEDED 1

#define REPEATED_CHARS 1

/* declare functions */
void clear_buf (char* input, int size);

int check_for_upper (char *input, int size);
int check_for_lower (char *input, int size);
int check_for_number (char *input, int size);
int check_for_double (char *input, int size);

int main(int argc, char **argv) {

    char buffer[BUFFER_SIZE];
    clear_buf(buffer, BUFFER_SIZE);
    
    int i, c = 0;
    int num_of_chars = 0;

    for (i = 0; (c = getc(stdin)) != EOF; i++) {
        buffer[i] = (char)c;

        if (((char)c) == '\n') {
            buffer[i] = '\0'; // terminate the string
            num_of_chars++;

            if (check_for_upper(buffer, BUFFER_SIZE) && 
                        check_for_lower (buffer, BUFFER_SIZE) &&
                        check_for_number(buffer, BUFFER_SIZE) &&
                        check_for_double(buffer, BUFFER_SIZE))
                printf("%s\n", buffer);

            clear_buf(buffer, num_of_chars);

            num_of_chars = 0;
            i = -1;
        }
    }

    return 0;
}

/*
 * function : check_for_<char type>
 * purpose  : check the buffer for the aforementioned character type
 * input    : char *input <buffer>, int size <of buffer
 * output   : 1 if bad, 0 if good
 */
int check_for_upper (char *input, int size) {
    int i;

    for (i = 0; i < size && input[i] != '\0'; i++)
        if (isupper(input[i]))
            return 1;
    return 0;
}

int check_for_lower (char *input, int size) {
    int i;

    for (i = 0; i < size && input[i] != '\0'; i++)
        if (islower(input[i]))
            return 1;
    return 0;
}

int check_for_number (char *input, int size) {
    int i;

    for (i = 0; i < size && input[i] != '\0'; i++)
        if (isdigit(input[i]))
            return 1;
    return 0;
}

int check_for_double (char *input, int size) {
    int i;
    int l = strlen(input);

    for (i = 0; i < size && i < l && input[i + 1] != '\n'; i++)
        if (input[i] == input[i + 1])
            return 0;
    return 1;
}

void clear_buf (char* input, int size) {

    int i;
    for (i = 0; i < size; i++)
        input[i] = (char)0;

    return;
}
