/* 
 * Wed Nov 23, 2016 00:11
 *
 * ntlm password cracking thing
 *
 * NOTES
 *
 *	If you can't malloc the memory for the buffer, you may be asking for
 *	too much. You could probably go through and change the type of the
 *	various memory sizes.
 *
 * TODO
 *
 *	buffer input from stdin
 *	dual thread buffer convert ('\n' -> '\0')
 *	multiple threads
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <stdint.h>


// #include "platform.h"

#include "DES.h"
#include "MD4.h"
#include "MD5.h"
#include "LMhash.h"

/* maximum values */
#define BUFFER_SIZE  1024
#define MAX_WORD_LEN 14

/* define functions */
int get_buffer (char *dest, int dest_size, long *used, FILE *source);
void monkey_bars (char *buffer, int buff_size, long used, int thread_num);

/* -------------------------------------------------------------------------- */
int main(int argc, char **argv)
{
	int len, i, max;
	char plain[BUFFER_SIZE];
	unsigned char buffer[BUFFER_SIZE];
	unsigned char dest[BUFFER_SIZE];

	while (fgets((char *)buffer, BUFFER_SIZE, stdin) == (char *)buffer) {
		len = (strlen((char *)buffer) > 14) ? 14 : strlen((char *)buffer);

		if ('\n' == buffer[len - 1])
			buffer[--len] = '\0';

		max = (len > 14) ? 14 : len;
		buffer[max] = 0;

		/* copy to final buffer, to not lose plaintext */
		memcpy(plain, buffer, strlen((char *)buffer));

		for (i = 0; i < max; i++)
			dest[i] = toupper(buffer[i]);

		/* do LMhashing */
		auth_LMhash((unsigned char *)dest, (unsigned char *)dest, len);

		printf("  LM:");
		for (i = 0; i < 16; i++)
			printf("%.2X", dest[i]);

		printf(":%s\n", plain);

		/* do NTLM hashing */
		max = (len > (BUFFER_SIZE / 2) ? (BUFFER_SIZE / 2) : len);
		buffer[(2 * max) - 1] = 0;

		for (i = max - 1; i > 0; i--) {
			buffer[(i * 2)] = buffer[i];
			buffer[(i * 2) - 1] = 0;
		}

		auth_md4Sum(dest, buffer, 2 * max);

		printf("NTLM:");
		for (i = 0; i < 16; i++)
			printf("%.2X", dest[i]);
		printf(":%s\n", plain);

		memset(buffer, 0, BUFFER_SIZE);
		memset(dest, 0, BUFFER_SIZE);
		memset(plain, 0, BUFFER_SIZE);
	}

	return 0;
}
