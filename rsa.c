#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <stdbool.h>
#include "randstate.h"
#include "numtheory.h"

// Generates the components for a new public RSA key.
void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters)
{
	//Setting up vars
	mpz_t lamn, rannum, gc, temp, temp2, temp3, temp4;
	mpz_inits(lamn, rannum, gc, temp, temp2, temp3, temp4, NULL);
	uint64_t upper = (3 * nbits) / 4;
	uint64_t lower = (nbits / 4);
	//generatng a random num in a range
	uint64_t pbits = lower + random() % (upper + 1 - lower);
	//calling num theory func
	make_prime(p, pbits, iters);
	uint64_t qbits = nbits - pbits;
	make_prime(q, qbits, iters);
	mpz_mul(n, p, q);
	mpz_sub_ui(temp, p, 1);
	mpz_sub_ui(temp2, q, 1);
	mpz_mul(temp3, temp, temp2);
	gcd(temp4, temp, temp2);
	mpz_fdiv_q(lamn, temp3, temp4);
	//Loops until coprimme of lamn is found
	while(true)
	{
		mpz_urandomb(rannum, state, nbits);
		gcd(gc, rannum, lamn);
		if(mpz_cmp_ui(gc, 1) == 0)
		{
			mpz_set(e, rannum);
			break;
		}
	}
	//clears the vars
	mpz_clears(lamn, rannum, gc, temp, temp2, temp3, temp4, NULL);
}

// Generates the components for a new private RSA key.
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q)
{
	//setting up vars
	mpz_t lamn, temp, temp2, temp3, temp4;
	mpz_inits(lamn, temp, temp2, temp3, temp4, NULL);
        mpz_sub_ui(temp, p, 1); 
        mpz_sub_ui(temp2, q, 1); 
        mpz_mul(temp3, temp, temp2);
        //calling num theory funcs
        gcd(temp4, temp, temp2);
        mpz_fdiv_q(lamn, temp3, temp4);
	mod_inverse(d, e, lamn);
	//clearing vars
	mpz_clears(lamn, temp, temp2, temp3, temp4, NULL);
}	

// Encrypts a message given an RSA public exponent and modulus.
void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n)
{
	//calling pow mod from numtheory
	pow_mod(c, m, e, n);
}

// Decrypts some ciphertext given an RSA private key and public modulus.
void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n)
{
	//calling pow mod from numtheory
	pow_mod(m, c, d, n);
}

// Signs some message given an RSA private key and public modulus.
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n)
{
	//calling pow mod from numtheory
	pow_mod(s, m, d, n);
}

// Verifies some signature given an RSA public exponent and modulus.
bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n)
{
	//setting up var
	mpz_t t;
	mpz_init(t);
	//calling pow mod from numtheory
	pow_mod(t, s, e, n);
	//checks if t and m are equal
	if(mpz_cmp(t, m) == 0)
	{
		return true;
	}
	//clearing var
	mpz_clear(t);
	return false;
}

// Writes a public RSA key to a file.
void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile)
{
	//printing the vars as hexstrings
	gmp_fprintf(pbfile, "%Zx\n",n);
	gmp_fprintf(pbfile, "%Zx\n",e);
	gmp_fprintf(pbfile, "%Zx\n",s);
	gmp_fprintf(pbfile, "%s\n",username);
}

// Reads a public RSA key from a file.
void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile)
{
	//reading the hexstrings are storing them
	gmp_fscanf(pbfile, "%Zx\n", n);
	gmp_fscanf(pbfile, "%Zx\n", e);
	gmp_fscanf(pbfile, "%Zx\n", s);
	gmp_fscanf(pbfile, "%s\n", username);
}

// Writes a private RSA key to a file.
void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile)
{
	//priting vars as hexstrings
	gmp_fprintf(pvfile, "%Zx\n",n);
	gmp_fprintf(pvfile, "%Zx\n",d);
}

// Reads a private RSA key from a file.
void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile)
{
	//reading hexstrings and storing them
	gmp_fscanf(pvfile, "%Zx\n", n);
	gmp_fscanf(pvfile, "%Zx\n", d);
}

// Encrypts an entire file given an RSA public modulus and exponent.
void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e)
{
	//setting up vars
	mpz_t m, c;
	mpz_inits(m, c, NULL);
	size_t k = (mpz_sizeinbase(n, 2) - 1) / 8;
	//allocating array
	uint8_t *arr = (uint8_t *) calloc(k, sizeof(uint8_t));
	arr[0] = 0xFF;
	//Looping til EOF
	while(!feof(infile))
	{
		//parsing input and storing it in the array
		size_t j = fread(arr + 1, 1, k-1, infile);
		size_t s = sizeof(uint8_t);
		//converting input to mpt_z
		mpz_import(m, j+1, 1, s, 1, 0, arr);
		//calling encrypt
		rsa_encrypt(c, m, e, n);
		//outputing encrypted message
		gmp_fprintf(outfile, "%Zx\n",c);
	}
	//freeing array and clearing vars
	free(arr);
	mpz_clears(m, c, NULL);
}

// Decrypts an entire file given an RSA public modulus and private key.
void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d)
{
	//setting up vars
	mpz_t m, c;
        mpz_inits(m, c, NULL);
        size_t k = (mpz_sizeinbase(n, 2) - 1) / 8;
        //allocating array
        uint8_t *arr = (uint8_t *) calloc(k, sizeof(uint8_t));
        //Looping til EOF
        while(!(feof(infile)))
        {
		//parsing input and storing it in an mpt_z
		size_t s = gmp_fscanf(infile, "%Zx\n", c);
		//calling decrypt
		rsa_decrypt(m, c, d, n);
		size_t j;
		//converting input and sending to array
		mpz_export(arr, &j, 1, s, 1, 0, m);
		//Outputting decrypted message
		fwrite(arr + 1, 1, j-1, outfile);
	}
	//freeing array and clearing vars
	free(arr);
	mpz_clears(m, c, NULL);
}




		
			

				









