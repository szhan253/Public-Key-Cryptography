#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <limits.h>
#include <time.h>
#include <gmp.h>

#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

#define OPTIONS "hvi:o:n:"

int main(int argc, char **argv) {
    //init
    mpz_t n, e, s, user, m;
    mpz_inits(n, e, s, user, m, NULL);

    char username[_POSIX_LOGIN_NAME_MAX];
    char *pubfile = "rsa.pub";
    //char* pivfile = NULL;
    char *infl = NULL;
    char *outfl = NULL;

    //defaule
    FILE *infile = stdin;
    FILE *outfile = stdout;
    FILE *pbfile = stdin;
    
    int opt = 0;

    bool test_h = false;
    bool test_v = false;
    bool test_i = false;
    bool test_o = false;
    bool test_n = false;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': test_h = true; break;
        case 'v': test_v = true; break;
        case 'i': //infile
            test_i = true;
            infl = optarg;
            break;
        case 'o': //outfile
            test_o = true;
            outfl = optarg;
            break;
        case 'n': //pbfile
            test_n = true;
            pubfile = optarg;
            break;
        default: test_h = true; break;
        }
    }

    //helping message
    if (test_h) {
        printf("SYNOPSIS\n");
        printf("   Encrypts data using RSA encryption.\n");
        printf("   Encrypted data is decrypted by the decrypt program.\n\n");

        printf("USAGE\n");
        printf("   ./encrypt [-hv] [-i infile] [-o outfile] -n pubkey\n\n");

        printf("OPTIONS\n");
        printf("   -h              Display program help and usage.\n");
        printf("   -v              Display verbose program output.\n");
        printf("   -i infile       Input file of data to encrypt (default: stdin).\n");
        printf("   -o outfile      Output file for encrypted data (default: stdout).\n");
        printf("   -n pbfile       Public key file (default: rsa.pub).\n");
        exit(0);
    }

    //input file
    infile = fopen(infl, "r");
    if (infile == NULL) {
        fprintf(stderr, "pbfile fopen failed");
        //clear memory
        mpz_clears(n, e, s, user, m, NULL);
        return 0;
    }

    //output file
    outfile = fopen(outfl, "w");
    if (outfile == NULL) {
        fprintf(stderr, "pbfile fopen failed");
        //clear memory
        mpz_clears(n, e, s, user, m, NULL);
        return 0;
    }

    //public file
    pbfile = fopen(pubfile, "rb");
    if (pbfile == NULL) {
        fprintf(stderr, "pbfile fopen failed");
        //clear memory
        mpz_clears(n, e, s, user, m, NULL);
        return 0;
    }

    //read from pbfile
    rsa_read_pub(n, e, s, username, pbfile);

    if (test_v) {
        printf("%s\n", username);
        gmp_printf("s (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), s);
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%d bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
    }

    //convert username to mpz_t
    mpz_set_str(user, username, 62);

    //verify signature
    rsa_verify(m, s, e, n);

    rsa_encrypt_file(infile, outfile, n, e);

    //close files
    fclose(infile);
    fclose(outfile);
    fclose(pbfile);
    //clear memory
    mpz_clears(n, e, s, user, m, NULL);
    return 0;
}
