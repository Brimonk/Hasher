/* 
 * Brian Chrzanowski
 * Wed Nov 23, 2016 00:11
 *
 * Password Hashing via CPU Suite
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <stdint.h>

#include "common.h"
#include "DES.h"
#include "MD4.h"
#include "MD5.h"
#include "LMhash.h"

#define USAGE "USAGE : %s [-l -n] [-h] [plain, packed]\nUse -h for more help\n"

/* maximum values */
#define BUFFER_SIZE  128

#define TYPE_LM     1
#define TYPE_NTLM   2
#define TYPE_MD4    3

void buff_to_upper(char *dest, char *source, int max);
int mk_readable(unsigned char *hash, int buflen);

void parse_args(int argc, char **argv, unsigned int *args);
void lm_helper(uint8 *dst, uint8 *src, const int32 len);
void ntlm_helper(uint8 *dst, uint8 *src, const int32 len);
void print_help();

/* -------------------------------------------------------------------------- */
int main(int argc, char **argv)
{
	int len, i, max;
	unsigned int type;

	void (*hash_funcs[3]) (uint8 *dest, uint8 *src, const int32 length) = {
		lm_helper,
		ntlm_helper
	};
	
	char inbuf[BUFFER_SIZE];
	char outbuf[BUFFER_SIZE];

	type = 0;
	parse_args(argc, argv, &type);

	if (!type) { // if they selected no type
		return 0;
	}

	memset(inbuf, 0, BUFFER_SIZE);
	memset(outbuf, 0, BUFFER_SIZE);

	for (i = 1; fgets(inbuf, BUFFER_SIZE, stdin) == inbuf; i++) {

		len = strlen(inbuf);
		if ('\n' == inbuf[len - 1]) {
			inbuf[len--] = '\0';
		}

		// perform the algorithm
		hash_funcs[type]((uint8 *)outbuf, (uint8 *)inbuf, len);

		// print out the result
		printf("%s:%s\n", outbuf, inbuf);

		memset(inbuf, 0, BUFFER_SIZE);
		memset(outbuf, 0, BUFFER_SIZE);
	}

	return 0;
}

void buff_to_upper(char *dest, char *source, int max)
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

// helper functions to wrap up each algorithm in a nice bow
void ntlm_helper(uint8 *dest, uint8 *src, const int32 len)
{
}

void lm_helper(uint8 *dst, uint8 *src, const int32 len)
{
}

