# Public Key Cryptography

This is an assignment for CSE13S in Fall 2021. Please do not use the code for any class assignments. 
The program uses rsa algorithm to encrypt and decrypt the input file. There are public keys and privite keys we need to use to do the encryption and decryption. There are three main functions in the program. One is called Keygen, which is used for generate the public and private key pair. The second one is called Encrypt, which is for encrypt files by using the public key. The last one is called Decrypt, which is used for decrypting by using the corresponding private key.

### Building
Building the program with:

```
$ make
```

### Running
Running the program with:

###### keygen.c
```
./keygen [-hv] [-b bits] -n pbfile -d pvfile
```

###### encrypt.c
```
./encrypt [-hv] [-i infile] [-o outfile] -n pubkey
```

###### decrypt.c
```
./decrypt [-hv] [-i infile] [-o outfile] -n pivkey
```

### Help Message
###### keygen.c   
    -h              Display program help and usage.   
    -v              Display verbose program output.   
    -b bits         Minimum bits needed for public key n (default: 256).   
    -i confidence   Miller-Rabin iterations for testing primes (default: 50).   
    -n pbfile       Public key file (default: rsa.pub).   
    -d pvfile       Private key file (default: rsa.priv).   
    -s seed         Random seed for testing.   

###### encrypt.c   
    -h              Display program help and usage.   
    -v              Display verbose program output.   
    -i infile       Input file of data to encrypt (default: stdin).   
    -o outfile      Output file for encrypted data (default: stdout).   
    -n pbfile       Public key file (default: rsa.pub).   

###### decrypt.c   
    -h              Display program help and usage.   
    -v              Display verbose program output.   
    -i infile       Input file of data to decrypt (default: stdin).   
    -o outfile      Output file for decrypted data (default: stdout).   
    -n pvfile       Private key file (default: rsa.priv).   
