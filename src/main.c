/* 
 * Wed Nov 23, 2016 00:11
 *
 * nt/lm password lookup creation
 *
 * TODO
 *
 *	buffer input from stdin
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

#define TYPE_LM     0X01
#define TYPE_NTLM   0x02

void print_to_stdout(char *type, unsigned char *hash, char *plaintext);
void buff_to_upper(unsigned char *dest, unsigned char *source, int max);
void char_ptrs_clear(char **ptrs, int length);

/* -------------------------------------------------------------------------- */
int main(int argc, char **argv)
{
	int len, i, max;
	unsigned int type;
	char plain[BUFFER_SIZE];

	char *ptrs[16] = {0};
	
	unsigned char buffer[BUFFER_SIZE];
	unsigned char dest[BUFFER_SIZE];

	/* check arguments */
	if (argc < 2) {
		type = TYPE_LM | TYPE_NTLM;
	} else {
		if (strcmp(argv[1], "ntlm") == 0) {
			type = TYPE_NTLM;
		}

		if (strcmp(argv[1], "lm") == 0) {
			type = TYPE_LM;
		}
	}

	ptrs[0] = (char *)&buffer[0];
	ptrs[1] = (char *)&dest[0];
	ptrs[2] = (char *)&plain[0];

	while (fgets((char *)buffer, BUFFER_SIZE, stdin) == (char *)buffer) {
		len = (strlen((char *)buffer) > 14) ? 14 : strlen((char *)buffer);

		if ('\n' == buffer[len - 1])
			buffer[--len] = '\0';

		max = (len > 14) ? 14 : len;
		buffer[max] = 0;

		/* copy to final buffer, to not lose plaintext */
		memcpy(plain, buffer, strlen((char *)buffer));

		/* do LMhashing */
		if (type & TYPE_LM) {
			buff_to_upper(&dest[0], &buffer[0], max);

			auth_LMhash((unsigned char *)dest, (unsigned char *)dest, len);
			print_to_stdout("LM", &dest[0], plain);
		}

		/* do NTLM hashing */
		if (type & TYPE_NTLM) {
			max = (len > (BUFFER_SIZE / 2) ? (BUFFER_SIZE / 2) : len);
			buffer[(2 * max) - 1] = 0;

			for (i = max - 1; i > 0; i--) {
				buffer[(i * 2)] = buffer[i];
				buffer[(i * 2) - 1] = 0;
			}

			auth_md4Sum(dest, buffer, 2 * max);
			print_to_stdout("NT", &dest[0], plain);
		}

		char_ptrs_clear(&ptrs[0], 3);
	}

	return 0;
}

void char_ptrs_clear(char **ptrs, int length)
{
	int i;

	for (i = 0; i < length; i++) {
		memset(ptrs[i], 0, BUFFER_SIZE);
	}
}

void print_to_stdout(char *type, unsigned char *hash, char *plaintext)
{
	/* make minimal syscalls. Use buffers for performance! */
	char buf[BUFFER_SIZE];
	int i, j;

	sprintf(&buf[0], "%s:", type);

	for (j = 0, i = strlen(type) + 1; j < 16; i += 2, j++) {
		sprintf(&buf[i], "%.2X", (unsigned char)hash[j]);
	}

	sprintf(&buf[i], ":%s\n", plaintext);

	printf("%s", buf);
}

void buff_to_upper(unsigned char *dest, unsigned char *source, int max)
{
	/* converts all bytes in buffer to upper case */
	int i;

	for (i = 0; i < max; i++)
		dest[i] = toupper(source[i]);
}
