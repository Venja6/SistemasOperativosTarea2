#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "interface.h"

// ------------------------------
//  parse_linea_dv()
// ------------------------------
// Convierte una DV en texto (decimal o 0xHEX)
// a un unsigned long.
unsigned long parse_linea_dv(const char *s) {
    char *end;
    errno = 0;
    unsigned long dv = strtoul(s, &end, 0);
    if (errno != 0 || *end != '\0') {
        fprintf(stderr, "DV inválida: '%s'\n", s);
        exit(1);
    }
    return dv;
}

// ------------------------------
//  get_npv()
// ------------------------------
// npv = DV >> b
unsigned long get_npv(unsigned long dv, int b) {
    return dv >> b;
}

// ------------------------------
//  get_offset()
// ------------------------------
// offset = DV & (2^b - 1)
unsigned long get_offset(unsigned long dv, int b) {
    unsigned long mask = (1UL << b) - 1;
    return dv & mask;
}

// ------------------------------
//  calcular_b()
// ------------------------------
// Calcula b tal que page_size = 2^b
int calcular_b(unsigned long page_size) {
    int b = 0;
    while ((1UL << b) < page_size)
        b++;

    if ((1UL << b) != page_size) {
        fprintf(stderr, "ERROR: page_size (%lu) no es potencia de 2\n", page_size);
        exit(1);
    }
    return b;
}