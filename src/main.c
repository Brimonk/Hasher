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

#include "DES.h"
#include "MD4.h"
#include "MD5.h"
#include "LMhash.h"

#define USAGE "USAGE : %s [-l -n] [-h] [plain, packed]\nUse -h for more help\n"

/* maximum values */
#define BUFFER_SIZE  1024
#define MAX_WORD_LEN 14

#define TYPE_LM     0X01
#define TYPE_NTLM   0x02
#define TYPE_MD4    0X04

void buff_to_upper(unsigned char *dest, unsigned char *source, int max);
void char_ptrs_clear(char **ptrs, int length);
int mk_readable(unsigned char *hash, int buflen);

void parse_args(int argc, char **argv, unsigned int *args);
void print_help();

/* -------------------------------------------------------------------------- */
int main(int argc, char **argv)
{
	int len, i, max;
	unsigned int args;
	char plain[BUFFER_SIZE];

	char *ptrs[16] = {0};
	
	char plainbuffer[BUFFER_SIZE];
	unsigned char buffer[BUFFER_SIZE];
	unsigned char dest[BUFFER_SIZE];
	unsigned char lmdest[BUFFER_SIZE];

	args = 0;
	parse_args(argc, argv, &args);

	if (!args) {
		return 0;
	}

	ptrs[0] = (char *)&buffer[0];
	ptrs[1] = (char *)&dest[0];
	ptrs[2] = (char *)&plain[0];
	ptrs[3] = (char *)&lmdest[0];

	char_ptrs_clear(&ptrs[0], 4);
	for (i = 1; fgets((char *)buffer, BUFFER_SIZE, stdin) == (char *)buffer;
			i++) {
		if ('\n' == buffer[len - 1])
			buffer[len--] = '\0';

		memcpy(plainbuffer, buffer, BUFFER_SIZE);
		len = (strlen((char *)buffer) > 14) ? 14 : strlen((char *)buffer);

		if ('\n' == buffer[len - 1])
			buffer[len--] = '\0';

		max = (len > 14) ? 14 : len;
		buffer[max] = 0;

		/* copy to final buffer, to not lose plaintext */
		memcpy(plain, buffer, strlen((char *)buffer));

		/* do LMhashing */
		buff_to_upper(&lmdest[0], &buffer[0], max);
		auth_LMhash((unsigned char *)&lmdest[0], (unsigned char *)lmdest, len);
		mk_readable(&lmdest[0], BUFFER_SIZE);

		/* do NTLM hashing */
		max = (len > (BUFFER_SIZE / 2) ? (BUFFER_SIZE / 2) : len);
		buffer[(2 * max) - 1] = 0;

		for (i = max - 1; i > 0; i--) {
			buffer[(i * 2)] = buffer[i];
			buffer[(i * 2) - 1] = 0;
		}

		auth_md4Sum(dest, buffer, 2 * max);
		mk_readable(&dest[0], BUFFER_SIZE);

		// print out the result
		printf("%s:%s\n", lmdest, plain);

		/* clear buffers */
		char_ptrs_clear(&ptrs[0], 4);
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

int mk_readable(unsigned char *hash, int buflen)
{
	/* make minimal syscalls. Use buffers for performance! */
	char buf[BUFFER_SIZE];
	int i, j;

	if (buflen < BUFFER_SIZE) {
		return 1;
	}

	for (j = 0, i = 0; j < 16; i += 2, j++) {
		sprintf(&buf[i], "%.2X", (unsigned char)hash[j]);
	}

	memset(hash, 0, buflen);
	memcpy(hash, buf, buflen);

	return 0;
}

void buff_to_upper(unsigned char *dest, unsigned char *source, int max)
{
	/* converts all bytes in buffer to upper case */
	int i;

	for (i = 0; i < max; i++)
		dest[i] = toupper(source[i]);
}

/* parse args at the bottom because why not */
void parse_args(int argc, char **argv, unsigned int *args)
{
	char *s;
	char *prog_name;

	prog_name = argv[0];

	while (0 < --argc && (*++argv)[0] == '-') {
		for (s = argv[0] + 1; *s != '\0'; s++) {
			switch (*s) {
				case 'n':
					/* begins going into NTLM mode */
					*args |= TYPE_NTLM;
					break;
				case 'l':
					*args |= TYPE_LM;
					break;

				case 'h':
					print_help();
					break;

				default:
					fprintf(stderr, USAGE, prog_name);
					break;
			}
		}
	}
}

void print_help()
{
	fprintf(stderr, "Help Text Here\n");
	exit(1);
}

