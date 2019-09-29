/*
    Questo file contiente l'implementazione del file rsa.h
*/

#include "rsa.h"


uint8_t* toArray(uint64_t  number){
    uint8_t *result = calloc(OFFSET,sizeof(char));


    result[0] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[1] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[2] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[3] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[4] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[5] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[6] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[7] = number & 0x00000000000000FF ;

    return result;
}

uint64_t toUnit(uint8_t* buffer){
 
    uint64_t value ;

    value = buffer[7] ;
    value = (value << 8 ) + buffer[6] ;
    value = (value << 8 ) + buffer[5] ;
    value = (value << 8 ) + buffer[4] ;
    value = (value << 8 ) + buffer[3] ;
    value = (value << 8 ) + buffer[2] ;
    value = (value << 8 ) + buffer[1] ;
    value = (value << 8 ) + buffer[0] ;

    return value;

}

uint64_t mul_mod(uint64_t a, uint64_t b, uint64_t m){
    uint64_t d = 0, mp2 = m >> 1;
    int i;
    if (a >= m) a %= m;
    if (b >= m) b %= m;
    for (i = 0; i < 64; ++i)
    {
        d = (d > mp2) ? (d << 1) - m : d << 1;
        if (a & 0x8000000000000000ULL)
            d += b;
        if (d >= m) d -= m;
        a <<= 1;
    }
    return d;
}

uint64_t pow_mod(uint64_t a, uint64_t b, uint64_t m){
    uint64_t r = m==1?0:1;
    while (b > 0) {
        if (b & 1)
            r = mul_mod(r, a, m);
        b = b >> 1;
        a = mul_mod(a, a, m);
    }
    return r;
}



/*
 * Miller-Rabin Primality test, ITERATION signifies the accuracy
 */
uint64_t  Miller(uint64_t  p){
    uint64_t  i;
    uint64_t     s;
    if (p < 2) return 0;
    if (p != 2 && p % 2==0) //controlla se il numero è pari
        return 0;


    s = p - 1; //s è pari


    while (s % 2 == 0) //finche non trova z tc z*2^w = p-1
    {
        s /= 2;
    }
    for (i = 0; i < ITERATION; i++)
    {
        uint64_t     a = rand() % (p - 1) + 1, temp = s;
        uint64_t     mod =(uint64_t ) pow_mod((uint64_t)a, (uint64_t)temp,(uint64_t) p);//y^z
        while (temp != p - 1 && mod != 1 && mod != p - 1) //p-1 è congruo a -1 mod p
        {
            mod =(uint64_t ) mul_mod((uint64_t)mod,(uint64_t) mod,(uint64_t) p);
            temp *= 2; //y^(2^i z)
        }
        if (mod != p - 1 && temp % 2 == 0)
        {
            return 0;
        }
    }
    return 1;
}

uint64_t   getPrime(void){
    uint64_t  num = (( uint64_t   ) rand()) % INT32_MAX;
    while(!Miller( num) || num <= 101){
        num = (( uint64_t   ) rand()) %  INT32_MAX;
    }

    return num;
}


uint64_t   gcdExtended(uint64_t   a, uint64_t   b, long long  *x, long long  *y) {
    // Base Case
    if (a == 0)
    {
        *x = 0, *y = 1;
        return b;
    }

    long long x1, y1; // To store results of recursive call
    uint64_t   gcd = gcdExtended(b%a, a, &x1, &y1);

    // Update x and y using results of recursive
    // call
    *x = y1 - ((uint64_t)b/a) * x1;
    *y = x1;

    return gcd;
}



RSA_param* init(void){
    srand((unsigned int) time(NULL) % MODULO);
    RSA_param * param = (RSA_param*) calloc(1,sizeof(RSA_param));
    long long x = 1;
    long  long y = 1;

    do {
        param->p = getPrime();
        param->q = param->p;

        while (param->p == param->q) {
            param->q = getPrime();
        }

        param->n = param->q * param->p;
    } while (param->n <= INT32_MAX);

    uint64_t  phi_n = (param->p - 1)*(param->q - 1);

    param->e = ((uint64_t )rand())%phi_n; // e < phi_n
    uint64_t t = 0;
    while(t != 1){
        t = gcdExtended(param->e,phi_n,&x,&y);
        if(t != 1) {
            param->e = ((uint64_t) rand()) % phi_n; //e coprimo con phi
            x = 1;
            y = 1;
        }
    }
    if(x < 0) x+= phi_n;
    uint64_t s = x % phi_n;
    param->d = s;

    return param;
}

uint8_t* encrypt(uint8_t *m, uint64_t n, uint64_t e){
    uint64_t tmp = toUnit(m);
    tmp = pow_mod(tmp,e,n);
    return toArray(tmp);
}

uint8_t* decipher(uint8_t * c,uint64_t n, uint64_t d) {
    uint64_t tmp = toUnit(c);
    tmp = pow_mod(tmp, d, n);
    return toArray(tmp);
}
