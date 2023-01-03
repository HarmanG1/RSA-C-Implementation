#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "rsa.h"
#include "randstate.h"
#include "numtheory.h"
#include <gmp.h>
#include <sys/stat.h>
#include <time.h>

#define OPTIONS "b:i:n:d:s:vh"

int main(int argc, char **argv)
{
	//sets up vars and their default values
	mpz_t p, q, n, e, d, m, s;
	mpz_inits(p, q, n, e, d, m, s, NULL);
	uint64_t nbits = 1024;
       	uint64_t iters = 50;
	char *pbfile = "rsa.pub";
	char *pvfile = "rsa.priv";
	uint64_t seed = time(NULL);
	int verbose = 0; 

	int opt = 0;
	//Parses the command line args
	while( ((opt = getopt(argc, argv, OPTIONS)) != -1))
	{
		//Swtich statement based off the command line arg passed
		switch (opt)
		{
		case 'b':
			nbits = strtoul(optarg, NULL, 10);
			//checks the size of nbits passed in
			if(nbits > 4096 || nbits < 50)
			{
				fprintf(stderr, "./keygen: Number of bits must be 50-4096, not %lu.\n", nbits);
				fprintf(stderr, "%s", "Usage: ./keygen [options]\n"
  				"  ./keygen generates a public / private key pair, placing the keys into the public and private\n"
  				"  key files as specified below. The keys have a modulus (n) whose length is specified in\n"
  				"  the program options.\n"
    				"    -s <seed>   : Use <seed> as the random number seed. Default: time()\n"
    				"    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n"
    				"    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n"
    				"    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n"
    				"    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n"
    				"    -v          : Enable verbose output.\n"
    				"    -h          : Display program synopsis and usage.\n");
				return 1;
			}
			break;
		case 'i':
			iters = strtoul(optarg, NULL, 10);
			//checks the size of itter passed in
			if(iters > 500 || iters < 1)
			{
				fprintf(stderr, "./keygen: Number of iterations must be 1-500, not %lu.\n", iters);
				fprintf(stderr, "%s", "Usage: ./keygen [options]\n"
  				"  ./keygen generates a public / private key pair, placing the keys into the public and private\n"
  				"  key files as specified below. The keys have a modulus (n) whose length is specified in\n"
  				"  the program options.\n"
    				"    -s <seed>   : Use <seed> as the random number seed. Default: time()\n"
    				"    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n"
    				"    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n"
    				"    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n"
    				"    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n"
    				"    -v          : Enable verbose output.\n"
    				"    -h          : Display program synopsis and usage.\n");
				return 1;
			}
			break;
		case 'n':
			//sets new value
			pbfile = optarg;
			break;
		case 'd':
			pvfile = optarg;
			break;
		case 's':
			seed = strtoul(optarg, NULL, 10);
			break;
		case 'v':
			//sets flag for verbose
			verbose = 1;
			break;
		case 'h':
			//help message
			fprintf(stderr, "%s", "Usage: ./keygen [options]\n"
  			"  ./keygen generates a public / private key pair, placing the keys into the public and private\n"
  			"  key files as specified below. The keys have a modulus (n) whose length is specified in\n"
  			"  the program options.\n"
    			"    -s <seed>   : Use <seed> as the random number seed. Default: time()\n"
    			"    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n"
    			"    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n"
    			"    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n"
    			"    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n"
    			"    -v          : Enable verbose output.\n"
    			"    -h          : Display program synopsis and usage.\n");
			return 0;
		case '?':
			//handles unknown input
			fprintf(stderr, "./keygen: invalid option -- '%s'\n", optarg);
			fprintf(stderr, "%s", "Usage: ./keygen [options]\n"
                        "  ./keygen generates a public / private key pair, placing the keys into the public and private\n"
  			"  key files as specified below. The keys have a modulus (n) whose length is specified in\n"
  			"  the program options.\n"
                        "    -s <seed>   : Use <seed> as the random number seed. Default: time()\n"
                        "    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n"
                        "    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n"
                        "    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n"
                        "    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n"
                        "    -v          : Enable verbose output.\n"
                        "    -h          : Display program synopsis and usage.\n");
                        return 1;
		}
	}
	//opens files and checks for errors
	FILE *fpub;
	FILE *fpriv;
	fpub = fopen(pbfile, "w");
	if(fpub == NULL)
	{
		fprintf(stderr, "File opening failed!");
		return 1;
	}
	fpriv = fopen(pvfile, "w");
	if(fpriv == NULL)
	{
		fprintf(stderr, "File opening failed!");
		return 1;
	}
	//sets file permissions
	int fno1 = fileno(fpub);
	int fno2 = fileno(fpriv);
	fchmod(fno1, S_IRUSR|S_IWUSR);
	fchmod(fno2, S_IRUSR|S_IWUSR);
	//seeds the state
	randstate_init(seed);
	//calls rsa functions
	rsa_make_pub(p, q, n, e, nbits, iters);
	rsa_make_priv(d, e, p, q);
	char *user = getenv("USER");
	mpz_set_str(m, user, 62);
	rsa_sign(s, m, d, n);
	rsa_write_pub(n, e, s, user, fpub);
	rsa_write_priv(n, d, fpriv);
	
	//checks the verbose flag and prints out formatted strings of the vars
	if(verbose)
	{
		fprintf(stderr, "username: %s\n",user);
		gmp_fprintf(stderr, "user signature (%zu bits): %Zd\n", mpz_sizeinbase(s,2), s);
		gmp_fprintf(stderr, "p (%zu bits): %Zd\n", mpz_sizeinbase(p,2), p);
		gmp_fprintf(stderr, "q (%zu bits): %Zd\n", mpz_sizeinbase(q,2), q);
		gmp_fprintf(stderr, "n - modulus (%zu bits): %Zd\n", mpz_sizeinbase(n,2), n);
		gmp_fprintf(stderr, "e - public exponent (%zu bits): %Zd\n", mpz_sizeinbase(e,2), e);
		gmp_fprintf(stderr, "d - private exponent (%zu bits): %Zd\n", mpz_sizeinbase(d,2), d);
	}

	//closes files, clears state and vars, and returns 0
	fclose(fpub);
	fclose(fpriv);
	randstate_clear();
	mpz_clears(p, q, n, e, d, m, s, NULL);
	return 0;
}
			







