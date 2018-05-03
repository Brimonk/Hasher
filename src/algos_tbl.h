/*
 * Brian Chrzanowski
 * Wed May 02, 2018 22:06
 *
 * included a supported algorithms table to simplify the addition of new
 * algorithms and command line arguments
 */

struct algo_tbl {
	unsigned long flag;
	char *algo_name;
	/* 
	 * ideally, we have an entry function that takes a plaintext byte pointer
	 * (null terminated) and a destination buffer with size
	 *
	 * return nothing
	 */
	// void *
};
