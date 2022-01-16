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

#define OPTIONS "hvb:c:n:d:s:"
//change
int main(int argc, char **argv) {
    mpz_t p, q, n, e, d, user, s;
    mpz_inits(p, q, n, e, d, user, s, NULL);

    //default
    uint64_t b = 256;
    uint64_t c = 50;

    char *pubfile = "rsa.pub";
    char *pivfile = "rsa.priv";
    int piv = 0;
    //int pub = 0;
    FILE *pbfile; // = fopen("rsa.pub", "w+");
    FILE *pvfile; // = fopen("rsa.priv", "w+");
    //int vfile = 0;

    uint64_t seed = (int) time(NULL);
    //srandom(seed);

    //command line
    int opt = 0;

    bool test_h = false;
    bool test_v = false;
    bool test_b = false;
    bool test_c = false;
    bool test_n = false;
    bool test_d = false;
    bool test_s = false;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': test_h = true; break;
        case 'v': test_v = true; break;
        case 'b':
            test_b = true;
            b = atoi(optarg);
            break;
        case 'c': //iterations
            c = atoi(optarg);
            test_c = true;
            break;
        case 'n': //public
            test_n = true;
            pubfile = optarg;
            break;
        case 'd': //private
            test_d = true;
            pivfile = optarg;
            break;
        case 's':
            test_s = true;
            seed = atoi(optarg);
            break;
        default: test_h = true; break;
        }
    }

    if (test_h == true) {
        printf("SYNOPSIS\n");
        printf("   Generates an RSA public/private key pair.\n\n");

        printf("USAGE\n");
        printf("   ./keygen [-hv] [-b bits] -n pbfile -d pvfile\n\n");

        printf("OPTIONS\n");
        printf("   -h              Display program help and usage.\n");
        printf("   -v              Display verbose program output.\n");
        printf("   -b bits         Minimum bits needed for public key n (defaule: 256).\n");
        printf("   -c confidence   Miller-Rabin iterations for testing primes (default: 50).\n");
        printf("   -n pbfile       Public key file (default: rsa.pub).\n");
        printf("   -d pvfile       Private key file (default: rsa.priv).\n");
        printf("   -s seed         Random seed for testing.\n");
    }

    //open pbfile
    pbfile = fopen(pubfile, "wb");
    if (pbfile == NULL) {
        printf("pbfile fopen failed");
        //clear memory
        randstate_clear();
        mpz_clears(p, q, n, e, d, user, s, NULL);
        return 0;
    }

    //open pvfile
    pvfile = fopen(pivfile, "wb");
    if (pvfile == NULL) {
        printf("pvfile fopen failed");
        //clear memory
        randstate_clear();
        mpz_clears(p, q, n, e, d, user, s, NULL);
        return 0;
    }

    //change file to int
    //pub = fileno(pvfile);
    piv = fileno(pbfile);
    fchmod(piv, 0600);

    //open file
    //fopen(pbfile, "w+");
    //fopen(pvfile, "r+");

    //fchmod(pvfile, 0600);//change permission

    randstate_init(seed);

    //generate pub and priv keys
    rsa_make_pub(p, q, n, e, b, c);
    rsa_make_priv(d, e, p, q);

    //get username
    char *username = getenv("USER");

    //convert username
    mpz_set_str(user, username, 62);
    rsa_sign(s, user, d, n);

    //write into files
    rsa_write_pub(n, e, s, username, pbfile);
    rsa_write_priv(n, d, pvfile);

    if (test_v) {
        printf("%s\n", username);
        gmp_printf("s (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), s);
        gmp_printf("p (%d bits) = %Zd\n", mpz_sizeinbase(e, 2), p);
        gmp_printf("q (%d bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%d bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
        gmp_printf("d (%d bits) = %Zd\n", mpz_sizeinbase(q, 2), d);
    }

    //close file
    fclose(pbfile);
    fclose(pvfile);

    //clear memory
    randstate_clear();
    mpz_clears(p, q, n, e, d, user, s, NULL);
    return 0;
}
