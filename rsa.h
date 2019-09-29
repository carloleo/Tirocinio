#ifndef RSA_LIBRARY_H
#define RSA_LIBRARY_H
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <stdint.h>
#define ITERATION 30
#define OFFSET 8
#define MODULO 4037

/*
 * @brief: struct utilizzata per la memorizzazione dei parametri del cifrario RSA
 */
typedef struct param{
    uint64_t  p;
    uint64_t  q;
    uint64_t  n;
    uint64_t  e;
    uint64_t  d;

}RSA_param;
/*
 * @brief: dato un array di 8 byte restutisce l'intero a 64 bit corrispondente 
 *
 */
uint64_t toUnit(uint8_t* byteArray);
/*
 * @brief: dato un intero a 64 bit resituise l'array di 8 byte corrispondente
 */
uint8_t* toArray(uint64_t  n);
/*
 * @brief: esegue la moltiplicazione a * b modulo m
 */
uint64_t mul_mod(uint64_t a, uint64_t b, uint64_t m);
/*
 * @brief: calcola a^b modulo m
 */
uint64_t pow_mod(uint64_t a, uint64_t b, uint64_t m);
/*
 * @brief: test di primalità di Miller-Rabin
 */
uint64_t Miller(uint64_t  p);
/*
 * @brief: restituisce un numero primo a 32 bit con probabilità di errore uguale a (1/4)^30  
 */
uint64_t getPrime(void);
/*
 * @brief: restituisce il massimo comun divisore tra a e b, risolvendo l'equazione ax + by = mcd(a,b)
 *         implementa l'algoritmo di euclide esteso
 */
uint64_t gcdExtended(uint64_t   a, uint64_t   b, long long  *x, long long *y);
/*
* @brief: genera i parametri per il cifrario RSA
*/

RSA_param* init(void);
/*
 * @brief: cifra un messaggio di 8 byte ottendo l' uint64_t corrispondente. Restituisce il crittogramma come un array di 8 byte
 */
uint8_t *encrypt(uint8_t *m, uint64_t n, uint64_t e);
/*
 * @brief: decifra un crittogramma di 8 byte ottendo l' uint64_t corrispondente. Retstituisce il messaggio come un array di 8 byte
 */
uint8_t* decipher(uint8_t * c,uint64_t n, uint64_t d);


#endif //RSA_LIBRARY_H
