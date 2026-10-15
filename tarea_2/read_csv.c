/*
 * read_csv.c - Lee un archivo mult_cases.csv generado por gen_mult_csv.py.
 *
 * Cada línea tiene tres campos: a,b,p
 *   a, b : enteros sin signo de exactamente ND dígitos decimales
 *   p    : a*b, con a lo más 2*ND dígitos decimales
 *
 * Los números se almacenan como strings.
 * ND debe coincidir con NUM_DIGITS en el script de Python.
 * Se puede cambiar aquí o al compilar:
 * gcc -Wall -Wextra -O2 -DND=500 csv.c -o csv
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef ND
#define ND (400*19)                   /* digits per operand in base beta=10^19 */
#endif

#define PD (2 * ND)                /* max digits in the product */
#define LINE_LENGTH (ND + ND + PD + 8)  /* 3 fields, 2 commas, \r\n, \0, margin */
#define INITIAL_CAPACITY 10
#define N_SHOW 2

typedef struct {
    char *a;
    char *b;
    char *p;
} mult_case;

static void free_cases(mult_case *cases, size_t count)
{
    for (size_t i = 0; i < count; i++) {
        free(cases[i].a);
        free(cases[i].b);
        free(cases[i].p);
    }
    free(cases);
}

static void print_case(size_t idx, const mult_case *c)
{
    printf("%zu: a=%s\n   b=%s\n   p=%s\n", idx + 1, c->a, c->b, c->p);
}

int main(int argc, char *argv[])
{
    const char *filename = "mult_cases_400.csv";

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror(filename);
        return EXIT_FAILURE;
    }

    /* Build "%100[^,],%100[^,],%200[^,\r\n]" from ND so the widths
       always match the buffer sizes. */
    char fmt[64];
    snprintf(fmt, sizeof fmt, "%%%d[^,],%%%d[^,],%%%d[^,\r\n]", ND, ND, PD);

    size_t capacity = INITIAL_CAPACITY;
    size_t count = 0;
    mult_case *cases = malloc(capacity * sizeof *cases);
    if (cases == NULL) {
        perror("malloc");
        fclose(fp);
        return EXIT_FAILURE;
    }

    static char line[LINE_LENGTH];
    char a[ND + 1], b[ND + 1], p[PD + 1];
    size_t line_no = 0;

    while (fgets(line, sizeof line, fp) != NULL) {
        line_no++;

        /* A line that does not fit in the buffer has no '\n' (unless it is
           the last line of the file). Report it and skip the rest. */
        if (strchr(line, '\n') == NULL && !feof(fp)) {
            fprintf(stderr, "line %zu: too long (ND=%d?), skipped\n",
                    line_no, ND);
            int ch;
            while ((ch = fgetc(fp)) != '\n' && ch != EOF)
                ;
            continue;
        }

        if (sscanf(line, fmt, a, b, p) != 3) {
            fprintf(stderr, "line %zu: malformed, skipped\n", line_no);
            continue;
        }

        if (count == capacity) {
            capacity *= 2;
            mult_case *tmp = realloc(cases, capacity * sizeof *cases);
            if (tmp == NULL) {
                perror("realloc");
                free_cases(cases, count);
                fclose(fp);
                return EXIT_FAILURE;
            }
            cases = tmp;
        }

        mult_case *c = &cases[count];
        c->a = malloc(strlen(a) + 1);
        c->b = malloc(strlen(b) + 1);
        c->p = malloc(strlen(p) + 1);
        if (c->a == NULL || c->b == NULL || c->p == NULL) {
            perror("malloc");
            free(c->a);
            free(c->b);
            free(c->p);
            free_cases(cases, count);
            fclose(fp);
            return EXIT_FAILURE;
        }
        strcpy(c->a, a);
        strcpy(c->b, b);
        strcpy(c->p, p);
        count++;
    }
    fclose(fp);

    printf("Read %zu cases from %s (ND=%d)\n\n", count, filename, ND);

    /* First N_SHOW and last N_SHOW cases, without overlapping or
       underflowing when count is small. */
    size_t n_first = (count < N_SHOW) ? count : N_SHOW;
    for (size_t i = 0; i < n_first; i++)
        print_case(i, &cases[i]);

    if (count > n_first) {
        size_t start = (count - n_first > N_SHOW) ? count - N_SHOW : n_first;
        printf("...\n");
        for (size_t i = start; i < count; i++)
            print_case(i, &cases[i]);
    }

    free_cases(cases, count);
    return EXIT_SUCCESS;
}
