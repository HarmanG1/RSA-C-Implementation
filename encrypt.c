#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "rsa.h"
#include "randstate.h"
#include "numtheory.h"
#include <gmp.h>

#define OPTIONS "i:o:n:vh"

int main(int argc, char **argv)
{
	//sets up the vars and the default values
	mpz_t n, e, s, m;
	mpz_inits(n, e, s, m, NULL);
	FILE *fin = stdin;
	FILE *fout = stdout;
	char *pbfile = "rsa.pub";
	int verbose = 0; 

	int opt = 0;
	//Parses the command line args
	while( ((opt = getopt(argc, argv, OPTIONS)) != -1))
	{
		//Swtich statement based off the command line arg passed
		switch (opt)
		{
		case 'i':
			//opens the file based on the passed in name
			fin = fopen(optarg, "r");
			if(fin == NULL)
			{
				fprintf(stderr, "File opening failed!\n");
				return 1;
			}
			break;
		case 'o':
			fout = fopen(optarg, "w");
			if(fout == NULL)
			{
				fprintf(stderr, "File opening failed!\n");
				return 1;
			}
			break;
		case 'n':
			//sets public key file 
			pbfile = optarg;
                        break;
		case 'v':
			//sets flag
			verbose = 1;
                        break;
		case 'h':
			//help message
			fprintf(stderr, "%s", "Usage: ./encrypt [options]\n"
  		"  ./encrypt encrypts an input file using the specified public key file,\n"
	       		"  writing the result to the specified output file.\n"
    			"    -i <infile> : Read input from <infile>. Default: standard input.\n"
    			"    -o <outfile>: Write output to <outfile>. Default: standard output.\n"
    			"    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n"
    			"    -v          : Enable verbose output.\n"
    			"    -h          : Display program synopsis and usage.\n");
                        return 0;
		case '?':
			//handles unknown messages
			fprintf(stderr, "%s", "Usage: ./encrypt [options]\n"
                "  ./encrypt encrypts an input file using the specified public key file,\n"
 			"  writing the result to the specified output file.\n"
                        "    -i <infile> : Read input from <infile>. Default: standard input.\n"
                        "    -o <outfile>: Write output to <outfile>. Default: standard output.\n"
                        "    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n"
                        "    -v          : Enable verbose output.\n"
                        "    -h          : Display program synopsis and usage.\n");
                        return 1;
		}
	}
	//opens file and checks for errors
	FILE *fpub;
	fpub = fopen(pbfile, "r");
	if(fpub == NULL)
	{
		fprintf(stderr, "File opening failed!\n");
		return 1;
	}
	//calls rsa functions
	char username[100];
	rsa_read_pub(n, e, s, username, fpub);
	//checks flag and prints formatted strings of the vars
	if(verbose)
	{
		fprintf(stderr, "username: %s\n",username);
		gmp_fprintf(stderr, "user signature (%zu bits): %Zd\n", mpz_sizeinbase(s,2), s);
		gmp_fprintf(stderr, "n - modulus (%zu bits): %Zd\n", mpz_sizeinbase(n,2), n);
		gmp_fprintf(stderr, "e - public exponent (%zu bits): %Zd\n", mpz_sizeinbase(e,2), e);
	}
	
	mpz_set_str(m, username, 62);
	//checks the signature
	if(rsa_verify(m, s, e, n) == false)
	{
		fprintf(stderr, "Error: Signature couldn't be verified!\n");
		return 1;
	}
	//ecnrypts the file
	rsa_encrypt_file(fin, fout, n, e);
	//closes the files and clears the vars
	fclose(fpub);
	fclose(fin);
	fclose(fout);
	mpz_clears(n, e, m, s, NULL);
	return 0;
}
