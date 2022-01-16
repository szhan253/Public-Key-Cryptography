#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h>

#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
    mpz_t cal, p1, q1, totient;
    mpz_inits(cal, p1, q1, totient, NULL);

    uint64_t pbits, qbits;
    pbits = (random() % (nbits / 2)) + (nbits / 4);
    qbits = nbits - pbits;

    make_prime(p, pbits, iters); //create p
    make_prime(q, qbits, iters); //create q

    mpz_mul(n, p, q); //n = pq

    mpz_sub_ui(p1, p, 1); //p1 = p - 1
    mpz_sub_ui(q1, q, 1); //q1 = q - 1
    mpz_mul(totient, p1, q1); //phi(n) = (p - 1)(q - 1)

    //check if e is compime with totient
    do {
        mpz_urandomb(e, state, nbits);
        gcd(cal, e, totient);
    } while (mpz_cmp_ui(cal, 1) != 0);

    mpz_clears(cal, p1, q1, totient, NULL);

    return;
}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n", n);
    gmp_fprintf(pbfile, "%Zx\n", e);
    gmp_fprintf(pbfile, "%Zx\n", s);
    fprintf(pbfile, "%s\n", username);

    return;
}

void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx\n", n);
    gmp_fscanf(pbfile, "%Zx\n", e);
    gmp_fscanf(pbfile, "%Zx\n", s);
    fscanf(pbfile, "%s\n", username);

    return;
}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
    mpz_t totient, temp_p, temp_q, p1, q1;
    mpz_inits(totient, temp_p, temp_q, p1, q1, NULL);

    //backup input values
    mpz_set(temp_p, p);
    mpz_set(temp_q, q);

    //phi(n) = (p - 1)(q - 1)
    mpz_sub_ui(p1, temp_p, 1);
    mpz_sub_ui(q1, temp_q, 1);
    mpz_mul(totient, p1, q1);

    //use mod inverse to get d
    mod_inverse(d, e, totient);

    mpz_clears(totient, temp_p, temp_q, p1, q1, NULL); //clear memory
    return;
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n", n);
    gmp_fprintf(pvfile, "%Zx\n", d);

    return;
}

void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx\n", n);
    gmp_fscanf(pvfile, "%Zx\n", d);

    return;
}

void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    pow_mod(c, m, e, n); //c = m ^ e (mod n)
    return;
}

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    mpz_t log2n, k, cal, m, c, k1, eight;
    mpz_inits(log2n, k, cal, m, c, k1, eight, NULL);

    uint64_t j;
    long int temp;
    //k = (mpz_sizeinbase(n, 2) - 1);
    //temp = mpz_sizeinbase(n, 2);
    j = 0;

    mpz_get_d_2exp(&temp, n); //log2n
    mpz_set_ui(log2n, temp);
    mpz_sub_ui(cal, log2n, 1); //log2n - 1
    mpz_set_ui(eight, 8);
    mpz_fdiv_q(k, cal, eight); //(log2n - 1) / 8

    uint8_t *buff = (uint8_t *) malloc(mpz_get_ui(k));

    buff[0] = 0xFF;

    mpz_sub_ui(k1, k, 1); //buff[0] is not count, thus total read is k - 1

    //j = fread(buff + 1, 1, mpz_get_ui(k1), infile);

    //printf("before while loop");

    while (!feof(infile)) {

        j = fread(buff + 1, 1, mpz_get_ui(k1), infile);
        //printf("j = %lu", j);
        mpz_import(m, j + 1, 1, sizeof(buff[0]), 1, 0, buff);

        rsa_encrypt(c, m, e, n);

        gmp_fprintf(outfile, "%Zx\n", c);

        //inti buff
        for (uint64_t i = 1; i < mpz_get_ui(k); i++) {
            buff[i] = 0;
        }

        //j = fread(buff + 1, 1, mpz_get_ui(k1), infile);
    }

    free(buff);
    mpz_clears(log2n, k, cal, m, c, eight, k1, NULL);
    return;
}

void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    pow_mod(m, c, d, n); //m = c ^ d (mod n)

    return;
}

void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    mpz_t log2n, cal, k, c, m, k1;
    mpz_inits(log2n, cal, k, c, m, k1, NULL);

    uint64_t j;
    long int temp;
    j = 0;

    //k
    mpz_get_d_2exp(&temp, n);
    mpz_set_ui(log2n, temp);
    mpz_sub_ui(cal, log2n, 1);
    mpz_div_ui(k, cal, 8);

    uint8_t *buff = (uint8_t *) malloc(mpz_get_ui(k));

    buff[0] = 0xFF;

    while (gmp_fscanf(infile, "%Zx\n", c) != EOF) {
        rsa_decrypt(m, c, d, n);
        mpz_export(buff, &j, 1, sizeof(buff[0]), 1, 0, m);
        fwrite(buff + 1, 1, j - 1, outfile);

        //inti buff
        for (uint64_t i = 1; i < mpz_get_ui(k); i++) {
            buff[i] = 0;
            //if (i < j) {
            //gmp_fprintf(outfile, "%d", buff[i]);
            //}
        }
        //fwrite(buff + 1, 1, j - 1, outfile);
        //gmp_fprintf(outfile, "\n");
    }

    free(buff);
    mpz_clears(log2n, cal, k, c, m, k1, NULL);
    return;
}

void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n); //s = m ^ d (mod n)
    return;
}

bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t t;
    mpz_init(t);

    pow_mod(t, s, e, n); //t = s ^ e (mod n)

    if (mpz_cmp(t, m) == 0) {
        mpz_clear(t);
        return true;
    } else {
        mpz_clear(t);
        return false;
    }
}
