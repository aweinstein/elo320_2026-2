/*
 * karatsuba.c - Multiplicacion usando Karatsuba con radix beta = 10^19
 * Ver Algorithm 1.3 en
 * Brent, R. P., & Zimmermann, P. (2011). Modern computer arithmetic (Vol. 18).
 * Cambridge: Cambridge University Press.
 *
 * Como BETA^2 > 2^64, la multiplicacion interna requiere de un tipo de datos de
 * 128-bit. Este codigo utiliza la extension de GCC/Clang unsigned __int128,
 * por lo que no es ISO-C.
 *
 * Compilacion:  gcc -std=gnu11 -O2 -Wall -Wextra karatsuba.c -o karatsuba
 * Uso:    ./karatsuba                 ejecuta la autoprueba
 *         ./karatsuba <x> <y>         imprime x*y (entradas en decimal)
 */
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned __int128 u128;
typedef uint64_t digit;

#define BETA        10000000000000000000ULL /* 10^19 */
#define BETA_DIGITS 19                      /* digitos decimales por limb */

#define N0 32 /* umbral n0: si n < N0 se usa BasecaseMultiply */

/**************** Prototipo de Funciones ****************/

/* Suma, resta, comparacion */
void add_at(digit *r, size_t rn, const digit *x, size_t xn, size_t off);
void sub_at(digit *r, size_t rn, const digit *x, size_t xn, size_t off);
int compare(const digit *x, size_t xn, const digit *y, size_t yn);
int abs_diff(digit *d, size_t n,
                    const digit *x, size_t xn, const digit *y, size_t yn);

/* Multiplicacion */
void basecase_multiply(digit *c, const digit *a, size_t m,
                       const digit *b, size_t n);
void karatsuba_multiply(digit *c, const digit *a, const digit *b, size_t n);

/* Entrada/salida decimal */
size_t from_decimal(const char *s, digit **out);
void print_number(const digit *x, size_t n);

/* Autoprueba */
digit random_digit(void);
int check(const digit *a, const digit *b, size_t n);
int self_test(void);

/* Funciones auxiliares */
digit *alloc_digits(size_t n);
void fail(const char *msg);
digit get(const digit *x, size_t n, size_t i);
void split(u128 t, digit *hi, digit *lo);


/**************** Main ****************/
int main(int argc, char **argv)
{
    if (argc == 1)
        return self_test();

    if (argc != 3) {
        fprintf(stderr, "usage: %s [x y]\n", argv[0]);
        return EXIT_FAILURE;
    }

    digit *x, *y;
    size_t xn = from_decimal(argv[1], &x);
    size_t yn = from_decimal(argv[2], &y);

    /* El Algoritmo 3 supone que ambos operandos tienen n digitos: se rellenan con ceros */
    size_t n = xn > yn ? xn : yn;
    digit *a = alloc_digits(n), *b = alloc_digits(n);
    memcpy(a, x, xn * sizeof *a);
    memcpy(b, y, yn * sizeof *b);

    digit *c = alloc_digits(2 * n);
    karatsuba_multiply(c, a, b, n);
    print_number(c, 2 * n);

    free(x);
    free(y);
    free(a);
    free(b);
    free(c);
    return EXIT_SUCCESS;
}

/**************** Funciones auxiliares ****************/

digit *alloc_digits(size_t n)
{
    digit *p = calloc(n ? n : 1, sizeof *p); /* inicializado en cero */
    if (p == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    return p;
}

void fail(const char *msg)
{
    fprintf(stderr, "internal error: %s\n", msg);
    abort();
}

/* x[i] si i < n, si no 0: trata x como rellenado con ceros a cualquier largo. */
digit get(const digit *x, size_t n, size_t i)
{
    return i < n ? x[i] : 0;
}

/* Separa t < BETA^2 en sus dos digitos en base BETA: t = hi*BETA + lo.
   Es el paso (uv)_b: hi = floor(t / BETA), lo = t mod BETA. */
void split(u128 t, digit *hi, digit *lo)
{
    digit q = (digit)(t / BETA);
    *hi = q;
    *lo = (digit)(t - (u128)q * BETA);
}

/**************** Suma, resta, comparacion ****************/

/* r <- r + x * BETA^off, donde r tiene rn digitos y x tiene xn digitos.
   Quien llama garantiza que el resultado cabe en rn digitos. */
void add_at(digit *r, size_t rn, const digit *x, size_t xn, size_t off)
{
    digit carry = 0;
    for (size_t i = 0; i < xn || carry; i++) {
        size_t p = off + i;
        digit xi = get(x, xn, i);
        if (p >= rn) { /* mas alla de r: todo lo que queda debe ser cero */
            if (xi != 0 || carry != 0)
                fail("add_at overflow");
            continue;
        }
        /* BETA > 2^63, asi que la suma de dos digitos puede superar 64 bits */
        u128 t = (u128)r[p] + xi + carry; /* < 2*BETA */
        carry = (t >= BETA);
        r[p] = (digit)(carry ? t - BETA : t);
    }
}

/* r <- r - x * BETA^off, donde r tiene rn digitos y x tiene xn digitos.
   Quien llama garantiza que el resultado no es negativo. */
void sub_at(digit *r, size_t rn, const digit *x, size_t xn, size_t off)
{
    digit borrow = 0;
    for (size_t i = 0; i < xn || borrow; i++) {
        size_t p = off + i;
        digit xi = get(x, xn, i);
        if (p >= rn) {
            if (xi != 0 || borrow != 0)
                fail("sub_at underflow");
            continue;
        }
        digit s = xi + borrow; /* <= BETA < 2^64, sin desborde */
        if (r[p] >= s) {
            r[p] -= s;
            borrow = 0;
        } else {
            r[p] = r[p] + (BETA - s); /* se mantiene < BETA */
            borrow = 1;
        }
    }
}

/* Retorna sign(x - y), comparando desde el digito mas significativo. */
int compare(const digit *x, size_t xn, const digit *y, size_t yn)
{
    size_t n = xn > yn ? xn : yn;
    for (size_t i = n; i-- > 0;) {
        digit xi = get(x, xn, i), yi = get(y, yn, i);
        if (xi != yi)
            return xi > yi ? 1 : -1;
    }
    return 0;
}

/* d[0..n-1] <- |x - y| y retorna sign(x - y).
   El valor absoluto se obtiene restando el operando menor
   al mayor; el signo se guarda por separado. */
int abs_diff(digit *d, size_t n,
                    const digit *x, size_t xn, const digit *y, size_t yn)
{
    int s = compare(x, xn, y, yn);
    memset(d, 0, n * sizeof *d);
    if (s > 0) {
        memcpy(d, x, xn * sizeof *d);
        sub_at(d, n, y, yn, 0);
    } else if (s < 0) {
        memcpy(d, y, yn * sizeof *d);
        sub_at(d, n, x, xn, 0);
    }
    return s;
}

/**************** BasecaseMultiply ****************/                                                   */

/* c[0..m+n-1] <- a[0..m-1] * b[0..n-1] */
void basecase_multiply(digit *c, const digit *a, size_t m,
                       const digit *b, size_t n)
{
    memset(c, 0, (m + n) * sizeof *c);
    for (size_t j = 0; j < n; j++) {
        digit carry = COMPLETAR;
        for (size_t i = 0; i < m; i++) {
            u128 t = COMPLETAR;

        }
        c[j + m] = COMPLETAR;
    }
}

/***************** KaratsubaMultiply ****************/

/* c[0..2n-1] <- a[0..n-1] * b[0..n-1] */
void karatsuba_multiply(digit *c, const digit *a, const digit *b, size_t n)
{

    if (n < N0) {
        basecase_multiply(c, a, n, b, n);
        return;
    }


    size_t k = (n + 1) / 2;
    size_t h = n - k;

    const digit *a0 = a, *a1 = a + k;
    const digit *b0 = b, *b1 = b + k;


    digit *da = alloc_digits(k);
    digit *db = alloc_digits(k);
    int sa = abs_diff(COMPLETAR, COMPLETAR, COMPLETAR, COMPLETAR, COMPLETAR, h);
    int sb = abs_diff(COMPLETAR, COMPLETAR, COMPLETAR, COMPLETAR, COMPLETAR, h);

    digit *c0 = alloc_digits(2 * k);
    digit *c1 = alloc_digits(2 * h);
    digit *c2 = alloc_digits(2 * k);
    karatsuba_multiply(COMPLETAR, COMPLETAR, COMPLETAR, COMPLETAR);
    karatsuba_multiply(COMPLETAR, COMPLETAR, COMPLETAR, COMPLETAR);
    karatsuba_multiply(COMPLETAR, COMPLETAR, COMPLETAR, COMPLETAR);

    size_t mn = 2 * k + 1;
    digit *mid = alloc_digits(mn);
    add_at(mid, mn, c0, 2 * k, 0);
    add_at(mid, mn, c1, 2 * h, 0);
    if (sa * sb > 0)
        sub_at(mid, COMPLETAR, COMPLETAR, COMPLETAR, 0);
    else if (sa * sb < 0)
        add_at(mid, COMPLETAR, COMPLETAR, COMPLETAR, 0);

    memcpy(c, c0, 2 * k * sizeof *c);
    memcpy(c + 2 * k, c1, 2 * h * sizeof *c);
    add_at(c, 2 * n, mid, mn, k);

    free(da);
    free(db);
    free(c0);
    free(c1);
    free(c2);
    free(mid);
}

/**************** Entrada/salida decimal ****************/

/* Interpreta un string decimal; guarda los digitos en *out y retorna cuantos son. */
size_t from_decimal(const char *s, digit **out)
{
    size_t len = strlen(s);
    size_t n = (len + BETA_DIGITS - 1) / BETA_DIGITS;
    digit *x = alloc_digits(n);
    for (size_t i = 0; i < n; i++) {
        /* el digito i toma los caracteres [end - 19, end) contados desde la derecha */
        size_t end = len - i * BETA_DIGITS;
        size_t start = end > BETA_DIGITS ? end - BETA_DIGITS : 0;
        digit v = 0;
        for (size_t p = start; p < end; p++) {
            if (s[p] < '0' || s[p] > '9') {
                fprintf(stderr, "invalid decimal number: %s\n", s);
                exit(EXIT_FAILURE);
            }
            v = v * 10 + (digit)(s[p] - '0'); /* < 10^19 < 2^64 */
        }
        x[i] = v;
    }
    *out = x;
    return n;
}

void print_number(const digit *x, size_t n)
{
    while (n > 0 && x[n - 1] == 0)
        n--; /* omite los digitos cero iniciales */
    if (n == 0) {
        puts("0");
        return;
    }
    printf("%" PRIu64, x[n - 1]);
    for (size_t i = n - 1; i-- > 0;)
        printf("%019" PRIu64, x[i]); /* los digitos internos conservan sus ceros */
    putchar('\n');
}

/**************** Autoprueba: compara Karatsuba con el basecase ****************/

digit random_digit(void)
{
    digit x;
    do {
        x = 0;
        for (int i = 0; i < 5; i++)
            x = (x << 15) | (digit)(rand() & 0x7FFF);
    } while (x >= BETA);
    return x;
}

int check(const digit *a, const digit *b, size_t n)
{
    digit *ck = alloc_digits(2 * n), *cb = alloc_digits(2 * n);
    karatsuba_multiply(ck, a, b, n);
    basecase_multiply(cb, a, n, b, n);
    int ok = memcmp(ck, cb, 2 * n * sizeof *ck) == 0;
    free(ck);
    free(cb);
    return ok;
}

int self_test(void)
{
    int failures = 0;
    for (size_t n = 1; n <= 600; n++) {
        digit *a = alloc_digits(n), *b = alloc_digits(n);

        for (size_t i = 0; i < n; i++) { /* operandos aleatorios */
            a[i] = random_digit();
            b[i] = random_digit();
        }
        failures += !check(a, b, n);

        for (size_t i = 0; i < n; i++) /* operandos maximos: BETA^n - 1 */
            a[i] = b[i] = BETA - 1;
        failures += !check(a, b, n);

        for (size_t i = 0; i < n; i++) { /* A0 = A1: una diferencia igual a cero */
            a[i] = random_digit();
            b[i] = random_digit();
        }
        size_t k = (n + 1) / 2;
        for (size_t i = k; i < n; i++)
            a[i] = a[i - k];
        if (n % 2 == 1)
            a[k - 1] = 0; /* A0 tiene un digito mas que A1 */
        failures += !check(a, b, n);

        free(a);
        free(b);
    }
    printf("self-test (N0 = %d): %s\n", N0, failures ? "FAILED" : "all OK");
    return failures != 0;
}
