#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <stdbool.h>
#include "randstate.h"

//Finds the greatest common denominator of two numbers
void gcd(mpz_t d, mpz_t a, mpz_t b)
{	
	//setting up vars
	mpz_t t, aa, bb;
	mpz_inits(t, aa, bb, NULL);
	mpz_set(aa, a);
	mpz_set(bb, b);
	//Loops until b is zero
	while(mpz_cmp_ui(bb, 0) != 0)
	{
		mpz_set(t, bb);
		mpz_mod(bb, aa, bb);
		mpz_set(aa, t);
	}
	//sets output var and clears
	mpz_set(d, aa);
	mpz_clears(t, aa, bb, NULL);
}

//Finds the inverse mod of two numbers
void mod_inverse(mpz_t o, mpz_t a, mpz_t n)
{
	//setting up vars
	mpz_t r, rp, q, temp, t, tp, temp2, temp3, temp4;
	mpz_inits(r, rp, q, temp, t, tp, temp2, temp3, temp4, NULL);
	mpz_set(r, n);
	mpz_set(rp, a);
	mpz_set_ui(t, 0);
	mpz_set_ui(tp, 1);
	//sets flag to use later
	bool flag = false;
	
	//loops until rp is 0
	while(mpz_cmp_ui(rp,0) != 0)
	{
		//does various computations on the vars
		mpz_fdiv_q(q, r, rp);
		
		mpz_set(temp, r);
		mpz_set(r, rp);
		mpz_mul(temp2, q, rp);
		mpz_sub(rp, temp, temp2);
		
		mpz_set(temp3, t);
		mpz_set(t, tp);
		mpz_mul(temp4, q, tp);
		mpz_sub(tp, temp3, temp4);
	}
	
	//checks if r is 1 and sets a flag
	if(mpz_cmp_ui(r,1) == 1)
	{
		mpz_set_ui(o, 0);
		flag = true;
	}
	
	//checks if t is zero
	if(mpz_cmp_ui(t, 0) == -1)
	{
		mpz_add(t, t, n);
	}
	
	//checks if flag is set and acts accordingly
	if(!flag)
	{
		mpz_set(o, t);
	}
	//clears the vars
	mpz_clears(r, rp, q, temp, t, tp, temp2, temp3, temp4, NULL);
}

//finds the mod of the power of the two numbers ie (a ** d) % n
void pow_mod(mpz_t o, mpz_t a, mpz_t d, mpz_t n)
{
	//setting up vars
	mpz_t v, p, dd, temp1, temp2;
	mpz_inits(v, p, dd, temp1, temp2, NULL);
	mpz_set_ui(v, 1);
	mpz_set(p, a);
	mpz_set(dd, d);
	//loops until d is zero
	while(mpz_cmp_ui(dd, 0) == 1)
	{
		mpz_set(temp1, v);
		mpz_set(temp2, p);
		//checks if d is odd
		if(mpz_odd_p(dd))
		{
			mpz_mul(temp1, v, p);
			mpz_mod(v, temp1, n);
		}
		mpz_pow_ui(temp2, p, 2);
		mpz_mod(p, temp2, n);
		mpz_fdiv_q_ui(dd, dd, 2);
	}
	//sets output var and clears
	mpz_set(o, v);
	mpz_clears(v, p, dd, temp1, temp2, NULL);
}

//checks if a number is prime
bool is_prime(mpz_t n, uint64_t iters)
{
	//setting up vars
	mpz_t s, r, j, a, y, temp, temp2, temp3;
   	mpz_inits(r, s, j, a, y, temp, temp2, temp3, NULL);
	mpz_set_ui(r, 1);
	mpz_set_ui(s, 0);
	mpz_set(temp, n);
	mpz_set_ui(temp3, 2);
	//checks if temp is odd
	if(mpz_odd_p(temp))
	{
		mpz_sub_ui(temp, n, 1);
		//loops while r is odd
		while(mpz_odd_p(r))
		{
			mpz_fdiv_q_ui(j, temp, 2);
			mpz_set(r, j);
			mpz_set(temp, j);
			mpz_add_ui(j, s, 1);
			mpz_set(s, j);
		}
	}
	//if even then it clears vars and returns false
	else if(mpz_even_p(n))
	{
		mpz_clears(r, s, j, a, y, temp, temp2, temp3, NULL);
		return false;
	}
	
	mpz_sub_ui(temp, n, 2);
	mpz_sub_ui(temp2, s, 1);
	//loops iters times 
	for(int i = 1; (uint64_t) i < iters; i++)
	{
		//sets up a random var
		mpz_urandomm(a, state, temp);
		//calls pow_mod
		pow_mod(y, a, r, n);
		mpz_sub_ui(temp, n, 1);
		//checks if y is not 1 and if y and temp are not equal
		if(mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, temp) != 0)
		{
			mpz_sub_ui(temp2, s, 1);
			//loops while j is less than temp2 and y is not equal to temp
			while(mpz_cmp(j, temp2) <= 0 && mpz_cmp(y , temp) != 0)
			{
				pow_mod(y, y, temp3, n);
				//loops while y is 1
				if(mpz_cmp_ui(y, 1) == 1)
				{
					mpz_clears(r, s, j, a, y, temp, temp2, temp3, NULL);
					return false;
				}
				mpz_add_ui(j, j, 1);
			}
			//checks if y and temp are not equal and clears and returns false
			if(mpz_cmp(y, temp) != 0)
			{
				mpz_clears(r, s, j, a, y, temp, temp2, temp3, NULL);
				return false;
			}
		}
	}
	//clears vars and returns true
	mpz_clears(r, s, j, a, y, temp, temp2, temp3, NULL);
	return true;
}

//generates a random prime number
void make_prime(mpz_t p, uint64_t bits, uint64_t iters)
{
	//loops until a prime number is found
	while(true)
	{
		//sets a random number
		mpz_urandomb(p, state, bits);
		//calls is prime to test
		if(is_prime(p, iters))
		{
			break;
		}
	}
}







