#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <time.h>

#include "numtheory.h"
#include "randstate.h"

void gcd(mpz_t d, mpz_t a, mpz_t b) {
    //backup value
    mpz_t temp_a;
    mpz_init_set(temp_a, a);

    mpz_t temp_b;
    mpz_init_set(temp_b, b);

    mpz_t temp;
    mpz_init(temp);

    while (mpz_cmp_ui(temp_b, 0) != 0) {
        mpz_set(temp, temp_b);
        mpz_mod(temp_b, temp_a, temp_b);
        mpz_set(temp_a, temp);
    }

    mpz_set(d, temp_a);

    mpz_clears(temp_a, temp_b, temp, NULL);
    return;
}

void mod_inverse(mpz_t i, mpz_t a, mpz_t n) {
    //backup value
    mpz_t temp_a;
    mpz_init_set(temp_a, a);
    mpz_t temp_n;
    mpz_init_set(temp_n, n);

    //declear new variable
    mpz_t t, t1, r, r1, q, q1, temp, cal;
    mpz_inits(t, t1, r, r1, q, q1, temp, cal, NULL);

    mpz_set(r, temp_n);
    mpz_set(r1, temp_a);
    mpz_set_ui(t, 0);
    mpz_set_ui(t1, 1);

    while (mpz_cmp_ui(r1, 0) != 0) {
        mpz_fdiv_q(q, r, r1);

        mpz_set(temp, r);
        mpz_set(r, r1); // r = r'
        mpz_mul(cal, q, r1); //cal = q * r'
        mpz_sub(cal, temp, cal); //cal = r - cal = r - q * r'; temp = r
        mpz_set(r1, cal);

        mpz_set(temp, t); //temp = t
        mpz_set(t, t1); // t = t'
        mpz_mul(cal, q, t1); //cal = q * t'
        mpz_sub(cal, temp, cal); //cal = temp - cal = t - cal = t - q * t'
        mpz_set(t1, cal); // t' = t - q * t'
    }

    if (mpz_cmp_ui(r, 1) > 0) {
        mpz_set_ui(t, 0);
    } else if (mpz_cmp_ui(t, 0) < 0) {
        mpz_add(cal, t, temp_n);
        mpz_set(t, cal);
    }

    mpz_set(i, t);

    mpz_clears(temp_a, temp_n, t, t1, r, r1, q, q1, temp, cal, NULL);
    return;
}

void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {
    //backup input
    mpz_t temp_base; //a
    mpz_init_set(temp_base, base);
    mpz_t temp_exp; //d
    mpz_init_set(temp_exp, exponent);
    mpz_t temp_mod; //n
    mpz_init_set(temp_mod, modulus);

    mpz_t v, p, cal;
    mpz_inits(v, p, cal, NULL);

    mpz_set_ui(v, 1);
    mpz_set(p, temp_base);

    while (mpz_cmp_ui(temp_exp, 0) > 0) { //while exp > 0
        mpz_mod_ui(cal, temp_exp, 2);
        if (mpz_cmp_ui(cal, 1) == 0) { //check if exp is odd
            mpz_mul(cal, v, p); //cal = v * p
            mpz_mod(v, cal, temp_mod); //cal = cal mod n = (v * p) mod n
            //mpz_set(v, cal);
        }
        mpz_mul(cal, p, p);
        mpz_mod(p, cal, temp_mod);
        //mpz_set(p, cal);
        mpz_fdiv_q_ui(temp_exp, temp_exp, 2);
    }
    mpz_set(out, v);

    mpz_clears(temp_base, temp_exp, temp_mod, v, p, cal, NULL);
    return;
}

bool is_prime(mpz_t n, uint64_t iters) {
    //special cases
    if ((mpz_cmp_ui(n, 1) == 0) || (mpz_cmp_ui(n, 0)) == 0) { //if n == 1 OR n == 0
        return false;
    }

    if (mpz_cmp_ui(n, 3) == 0) { //if n == 3; since later has a sub 3, 3 is also a special case
        return true;
    }

    //backup value
    mpz_t temp_n, n_min, r, a, upper_bound, y, cal, j, cal2;
    mpz_init_set(temp_n, n);
    //mpz_init_set(temp_i, iters);
    mpz_inits(n_min, r, a, upper_bound, y, cal, j, cal2, NULL);

    //write n - 1 = 2^r such that r is odd
    mpz_sub_ui(n_min, temp_n, 1); //n_min = n - 1

    mp_bitcnt_t s = 2; // s = 2
    while (mpz_divisible_2exp_p(n_min, s)) {
        s += 1;
    }
    s -= 1;

    mpz_tdiv_q_2exp(r, n_min, s);

    //for i <- 1 to k
    for (uint64_t i = 0; i < iters; i++) {
        mpz_sub_ui(upper_bound, temp_n, 3); //upper_bound = n - 3; make 3 as a special case
        mpz_urandomm(a, state, upper_bound); //state is in the randstate.h
        mpz_add_ui(a, a, 2);
        pow_mod(y, a, r, temp_n);

        mpz_sub_ui(cal, temp_n, 1); //cal = n - 1
        if ((mpz_cmp_ui(y, 1) != 0) && (mpz_cmp(y, cal) != 0)) { //if y != 1 && y != n - 1
            mpz_set_ui(j, 1);

            mpz_sub_ui(cal2, temp_n, 1); //cal2 = n - 1
            while ((mpz_cmp_ui(j, (s - 1)) <= 0)
                   && (mpz_cmp(y, cal2) != 0)) { //while j <= s -1 && y != n - 1
                mpz_set_ui(cal, 2);
                pow_mod(y, y, cal, temp_n);

                if (mpz_cmp_ui(y, 1) == 0) { // if y == 1
                    mpz_clears(temp_n, n_min, r, a, upper_bound, y, cal, j, cal2, NULL);
                    return false;
                }
                mpz_add_ui(j, j, 1);
            }

            mpz_sub_ui(cal, temp_n, 1); // cal = n - 1
            if (mpz_cmp(y, cal) != 0) { //if y != n - 1
                mpz_clears(temp_n, n_min, r, a, upper_bound, y, cal, j, cal2, NULL);
                return false;
            }
        }
    }

    mpz_clears(temp_n, n_min, r, a, upper_bound, y, cal, j, cal2, NULL);
    return true;
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {

    //offset 2^n
    mpz_t cal;
    mpz_init_set_ui(cal, 2);
    mpz_pow_ui(cal, cal, bits);

    do {
        mpz_urandomb(p, state, bits);
        mpz_add(p, p, cal); //add 2^n to do offset
    } while (!is_prime(p, iters));

    mpz_clear(cal);
    return;
}
