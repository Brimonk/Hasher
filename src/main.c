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
#include "sqlite3.h"

/* maximum values */
#define BUFFER_SIZE  1024
#define MAX_WORD_LEN 14

#define TYPE_LM     0X01
#define TYPE_NTLM   0x02
#define TYPE_MD4    0X04

#define DB_NAME "passes.db"

void buff_to_upper(unsigned char *dest, unsigned char *source, int max);
void char_ptrs_clear(char **ptrs, int length);
int mk_readable(unsigned char *hash, int buflen);
sqlite3 *db_open(char *db_name);

int db_tbl_create(sqlite3 *db);
int db_trans_begin(sqlite3 *db);
int db_trans_commit(sqlite3 *db);
int db_trans_insert(sqlite3 *db, unsigned char *lm, unsigned char *nt,
		char *plain);

void parse_args(int argc, char **argv, unsigned int *args);
void print_help();

/* -------------------------------------------------------------------------- */
int main(int argc, char **argv)
{
	int len, i, max;
	unsigned int args;
	char plain[BUFFER_SIZE];
	sqlite3 *db;

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

	db = db_open(DB_NAME);

	ptrs[0] = (char *)&buffer[0];
	ptrs[1] = (char *)&dest[0];
	ptrs[2] = (char *)&plain[0];
	ptrs[3] = (char *)&lmdest[0];

	/* 
	 * the main business logic is as follows:
	 *	begin db transaction
	 *
	 *		read in lines
	 *		hash lines
	 *		insert into table
	 *		commit and rebegin if needed
	 *
	 *	commit transaction
	 */

	/* make table if it does not exist */
	if (db_tbl_create(db)) { exit(1); }

	/* begin first transaction */
	if (db_trans_begin(db)) { exit(1); }

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

		/* db insert */
		if (db_trans_insert(db, lmdest, dest, plainbuffer)) { exit(1); }

		/* clear buffers */
		char_ptrs_clear(&ptrs[0], 4);

		if (i % 10000 == 0) {
			if (db_trans_commit(db)) { exit(1); }
			if (db_trans_begin(db)) { exit(1); }

			i = 0;
		}
	}

	if (db_trans_commit(db)) { exit(1); }

	sqlite3_close(db);

	return 0;
}

int db_tbl_create(sqlite3 *db)
{
	int rc;
	char *err_msg;
	char *sql = "create table if not exists list (" \
				"id integer primary key asc," \
				"plaintext text not null," \
				"lm text not null," \
				"nt text not null" \
				");";

	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if (rc) {
		fprintf(stderr, "SQL error: %s\n", err_msg);

		sqlite3_free(err_msg);
		sqlite3_close(db);

		return 1;
	}

	return 0;
}

int db_trans_insert(sqlite3 *db, unsigned char *lm, unsigned char *nt,
		char *plain)
{
	int rc, rc1, rc2, rc3, rc4;
	char *sql = "insert into list(lm, nt, plaintext) values (?, ?, ?);";
	sqlite3_stmt *stmnt;

	rc = rc1 = rc2 = rc3 = rc4 = 0; // init all return codes
	rc = sqlite3_prepare(db, sql, -1, &stmnt, 0);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	/* bind parameters */
	rc1 = sqlite3_bind_text(stmnt, 1, (char *)lm, -1, NULL);
	rc2 = sqlite3_bind_text(stmnt, 2, (char *)nt, -1, NULL);
	rc3 = sqlite3_bind_text(stmnt, 3, (char *)plain, -1, NULL);

	if (rc1 != SQLITE_OK || rc2 != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	rc4 = sqlite3_step(stmnt);

	if (rc4 != SQLITE_DONE) {
		fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
		return 1;
	}
	

	return 0;
}

int db_trans_commit(sqlite3 *db)
{
	int rc;
	char *err_msg;
	char *sql = "COMMIT TRANSACTION;";

	rc = 0;

	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", err_msg);

		sqlite3_free(err_msg);
		sqlite3_close(db);

		return 1;
	}

	return 0;
}

int db_trans_begin(sqlite3 *db)
{
	int rc;
	char *err_msg;
	char *sql = "BEGIN TRANSACTION;";

	rc = 0;

	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", err_msg);

		sqlite3_free(err_msg);
		sqlite3_close(db);

		return 1;
	}

	return 0;
}

sqlite3 *db_open(char *db_name)
{
	sqlite3 *db;
	int rc;

	rc = sqlite3_open(db_name, &db);

	if (rc) {
		fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(1);
	}

	return db;
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
					argc = 0;
					break;
			}
		}
	}

	/* these are filters to the program */
	if (*args != (*args & -*args)) {
		printf("Error: More than one arg set\n");
		*args = 0;
	}
}

void print_help()
{
	printf("Help text should go here\n");
	printf("Preferably as one giant string, then plastered in one printf\n");
	printf("statement\n");
}
