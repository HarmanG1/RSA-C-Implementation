# Assignment 4

The files all implement different aspects of rsa in c

## Files

### decrypt.c

This contains the implementation and main() function for the decrypt program.

- <code>-i</code>: specifies the input file to decrypt (default: stdin).
- <code>-o</code>: specifies the output file to decrypt (default: stdout).
- <code>-n</code>: specifies the file containing the private key (default: rsa.priv).
- <code>-v</code>: enables verbose output.
- <code>-h</code>: displays program synopsis and usage.

### encrypt.c

This contains the implementation and main() function for the encrypt program.

- <code>-i</code>: specifies the input file to decrypt (default: stdin).
- <code>-o</code>: specifies the output file to decrypt (default: stdout).
- <code>-n</code>: specifies the file containing the public key (default: rsa.pub).
- <code>-v</code>: enables verbose output.
- <code>-h</code>: displays program synopsis and usage.

### keygen.c

This contains the implementation and main() function for the keygen program.

- <code>-b</code>: specifies the minimum bits needed for the public modulus n (default: 1024).
- <code>-i</code>: specifies the number of Miller-Rabin iterations for testing primes (default: 50).
- <code>-n pbfile</code>: specifies the public key file (default: rsa.pub).
- <code>-d pvfile</code>: specifies the private key file (default: rsa.priv).
- <code>-s</code>: specifies the random seed for the random state initialization (default: the seconds since the UNIX epoch, given by time(NULL)).
- <code>-v</code>: enables verbose output.
- <code>-h</code>: displays program synopsis and usage.

### numtheory.c, numtheory.h

This contains the implementations of the number theory functions. / This specifies the interface for the number theory functions.

### randstate.c, randstate.h

This contains the implementation of the random state interface for the RSA library and number theory functions. / This specifies the interface for initializing and clearing the random state.

### rsa.c, rsa.h

This contains the implementation of the RSA library. / This specifies the interface for the RSA library.

### Makefile

Makefile contains the commands to complie and clean

### DESIGN.pdf

This file has the infromation regarding the design of the code with supporting psuedocode 

### WRITEUP.pdf

This file contains explanation of the testing process


