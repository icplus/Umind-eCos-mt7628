//File: BlowFish.h
//The header of the blow fish arigorthm
#ifndef __BLOWFISH__H__
#define __BLOWFISH__H__

//#include <string.h>
#ifndef MY_RET_CODE_
#define MY_RET_CODE_

#define CODE_SUCCESS	0
#define LENGTH_ILLEGAL	1
#define ENCODE_ERROR	2
#define BUFFER_TOO_SMALL 3
#endif
////////////////////////////////////////////////
//Blowfish algorithm
//
// Usage:
// construct a instance of the class.
// then call encode or decode function and ger result.
// if no key supply the class use a default key
// 
// Note:
// source input must round to 64 bit.

#define MAXKEYBYTES 56 // 448 bits
#define ROUNDCOUNT  16
#define DEFAULT_KEY  "fgPmn2opyzqrshNRSMetuvabcdJKLElCDGOFwxABi13jkHIQTUV4"

typedef struct {
	unsigned long S[4][256],P[18];
}blf_ctx;

#if 0
class CBlowfish
{
private:
	unsigned char key[MAXKEYBYTES];
	unsigned keylen;
	blf_ctx c;

	void InitBlowfish();
	unsigned long F1(blf_ctx* bc,unsigned long x);
	void Blowfish_encipher(blf_ctx *c,unsigned long *xl,unsigned long *xr);
	void Blowfish_decipher(blf_ctx *c,unsigned long * xl, unsigned long *xr);
	void blf_enc(blf_ctx *c,unsigned long *data,int blocks);
	void blf_dec(blf_ctx *c,unsigned long *data,int blocks);

public:
	//Constructor
	CBlowfish (); // using default key to encode and decode.
	CBlowfish(const unsigned char* keybuf,unsigned int len);
	
	void SetKey(const unsigned char* keybuf,unsigned int len);
	//Remember the src length must 8 mod 0,that is round to 64 bits.

	long EnCode(const char* src,char* dest,unsigned long len) ;
	long DeCode(const char* src,char* dest,unsigned long len) ;
	//return value :
	//0 success
	//1 length is illegal
	//2 other error
};
#endif

void CBlowfish_SetKey(const unsigned char* keybuf,unsigned int len);
long CBlowfish_EnCode(const char* src,char* dest,unsigned long len);
long CBlowfish_DeCode(const char* src,char* dest,unsigned long len);

#endif


