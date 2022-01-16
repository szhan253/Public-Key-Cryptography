#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <time.h>
#include <gmp.h>

#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

#define OPTIONS "hvi:o:n:"

int main(int argc, char **argv) {
    //init
    mpz_t n, d;
    mpz_inits(n, d, NULL);

    //defaule
    FILE *infile = stdin;
    FILE *outfile = stdout;
    FILE *pvfile; //= fopen("rsa.priv", "w+");

    char *infl = NULL;
    char *outfl = NULL;
    char *pivfile = "rsa.priv";

    int opt = 0;

    bool test_h = false;
    bool test_v = false;
    bool test_i = false;
    bool test_o = false;
    bool test_d = false;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': test_h = true; break;
        case 'v': test_v = true; break;
        case 'i': //infile
            test_i = true;
            infl = optarg;
            infile = fopen(infl, "r");
            if (infile == NULL) {
                printf("pbfile fopen failed");
                //clear memory

                return 0;
            }

            break;
        case 'o': //outfile
            test_o = true;
            outfl = optarg;
            outfile = fopen(outfl, "w");
            if (outfile == NULL) {
                printf("pbfile fopen failed");
                //clear memory

                return 0;
            }

            break;
        case 'n': //pvfile
            test_d = true;
            pivfile = optarg;
            break;
        default: test_h = true; break;
        }
    }

    //helping message
    if (test_h) {
        printf("SYNOPSIS\n");
        printf("   Decrypts data using RSA decryption.\n");
        printf("   Encrypted data is encrypted by the encrypt program.\n\n");

        printf("USAGE\n");
        printf("   ./decrypt [-hv] [-i infile] [-o outfile] -n pivkey\n\n");

        printf("OPTIONS\n");
        printf("   -h              Display program help and usage.\n");
        printf("   -v              Display verbose program output.\n");
        printf("   -i infile       Input file of data to decrypt (default: stdin).\n");
        printf("   -o outfile      Output file for decrypted data (default: stdout).\n");
        printf("   -n pvfile       Private key file (default: rsa.priv).\n");
        exit(0);
    }

    //private file
    pvfile = fopen(pivfile, "rb");
    if (pvfile == NULL) {
        printf("pbfile fopen failed");
        //clear memory

        return 0;
    }

    //read private key
    rsa_read_priv(n, d, pvfile);

    if (test_v) {
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("d (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    rsa_decrypt_file(infile, outfile, n, d);

    //close files
    fclose(infile);
    fclose(outfile);
    fclose(pvfile);
    //clear memory
    mpz_clears(n, d, NULL);
    return 0;
}
