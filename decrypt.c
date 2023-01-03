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
	//sets up the vars and default values
	mpz_t n, d;
	mpz_inits(n, d, NULL);
	FILE *fin = stdin;
	FILE *fout = stdout;
	char *prvfile = "rsa.priv";
	int verbose = 0; 

	int opt = 0;
	//Parses the command line args
	while( ((opt = getopt(argc, argv, OPTIONS)) != -1))
	{
		//Swtich statement based off the command line arg passed in
		switch (opt)
		{
		case 'i':
			//opens the file based on the passed in name and checks for errors
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
			//sets the private key location
			prvfile = optarg;
                        break;
		case 'v':
			//sets flag
			verbose = 1;
                        break;
		case 'h':
			//help message
			fprintf(stderr, "%s", "Usage: ./decrypt [options]\n"
                "  ./decrypt decrypts an input file using the specified private key file,\n"
                        "  writing the result to the specified output file.\n"
                        "    -i <infile> : Read input from <infile>. Default: standard input.\n"
                        "    -o <outfile>: Write output to <outfile>. Default: standard output.\n"
                        "    -n <keyfile>: Private key is in <keyfile>. Default: rsa.priv.\n"
                        "    -v          : Enable verbose output.\n"
                        "    -h          : Display program synopsis and usage.\n");
                        return 0;
		case '?':
			//handles unknown input
			fprintf(stderr, "%s", "Usage: ./decrypt [options]\n"
                "  ./decrypt decrypts an input file using the specified private key file,\n"
 			"  writing the result to the specified output file.\n"
                        "    -i <infile> : Read input from <infile>. Default: standard input.\n"
                        "    -o <outfile>: Write output to <outfile>. Default: standard output.\n"
                        "    -n <keyfile>: Private key is in <keyfile>. Default: rsa.priv.\n"
                        "    -v          : Enable verbose output.\n"
                        "    -h          : Display program synopsis and usage.\n");
                        return 1;
		}
	}
	//opens private key file and checks for error
	FILE *fpriv;
	fpriv = fopen(prvfile, "r");
	if(fpriv == NULL)
	{
		fprintf(stderr, "File opening failed!\n");
		return 1;
	}
	rsa_read_priv(n, d, fpriv);
	//checks the flag and prints out a formatted string
	if(verbose)
	{
		gmp_fprintf(stderr, "n - modulus (%zu bits): %Zd\n", mpz_sizeinbase(n,2), n);
		gmp_fprintf(stderr, "d - private key (%zu bits): %Zd\n", mpz_sizeinbase(d,2), d);
	}
	//decrypts the file by calling the rsa function
	rsa_decrypt_file(fin, fout, n, d);

	//closing the files and clears the vars
	fclose(fpriv);
	fclose(fin);
	fclose(fout);
	mpz_clears(n, d, NULL);
	return 0;
}





