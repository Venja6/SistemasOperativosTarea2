#ifndef INTERFACE_H
#define INTERFACE_H

// Persona A
unsigned long parse_linea_dv(const char *s);
unsigned long get_npv(unsigned long dv, int b);
unsigned long get_offset(unsigned long dv, int b);
int calcular_b(unsigned long page_size);

// Persona B - CLOCK
extern int *tabla_paginas;
int buscar_marco(int npv);
void marcar_uso(int marco);
void init_clock(int cantidad_marcos);
unsigned long construir_df(int marco, unsigned long offset, int b);

void print_verbose(unsigned long dv, unsigned long npv, unsigned long offset,int marco, unsigned long df, int hit);

#endif
