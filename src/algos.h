/* char *MD5_hash (char *dest, const unsigned char *input, int len); */

/* NTLM default values */
#define INIT_A 0x67452301
#define INIT_B 0xefcdab89
#define INIT_C 0x98badcfe
#define INIT_D 0x10325476
 
#define SQRT_2 0x5a827999
#define SQRT_3 0x6ed9eba1

void NTLM_hash (char *dest, char *key);
