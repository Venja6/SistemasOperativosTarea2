#include <stdio.h>
#include <stdlib.h>
#include "interface.h"


// Variables globales del algoritmo del reloj (simples)
int *tabla_paginas = NULL;   // npv -> marco
int *marcos = NULL;          // marco -> npv cargado
int *uso = NULL;             // bit de uso
int puntero = 0;
int Nmarcos = 0;
int *tabla_paginas; // ya existe, pero agrega: 

void init_clock(int cantidad_marcos) {
    Nmarcos = cantidad_marcos;

    // Espacios para las estructuras
    tabla_paginas = (int*) malloc(sizeof(int) * 100000); // npv max aprox
    marcos = (int*) malloc(sizeof(int) * Nmarcos);
    uso = (int*) malloc(sizeof(int) * Nmarcos);

    // Inicializar todo en vacío
    for (int i = 0; i < 100000; i++)
        tabla_paginas[i] = -1;

    for (int i = 0; i < Nmarcos; i++) {
        marcos[i] = -1;
        uso[i] = 0;
    }

    puntero = 0;
}


int reloj_elegir_victima() {
    while (1) {
        if (uso[puntero] == 0) {
            int victima = puntero;
            puntero = (puntero + 1) % Nmarcos;
            return victima;
        } else {
            uso[puntero] = 0;
            puntero = (puntero + 1) % Nmarcos;
        }
    }
}

void marcar_uso(int marco) {
    uso[marco] = 1;
}



int buscar_marco(int npv) {

    // Si la página ya está cargada (HIT)
    if (tabla_paginas[npv] != -1) {
        int marco = tabla_paginas[npv];
        marcar_uso(marco);
        return marco;
    }

    // FALLO: buscar marco libre
    for (int i = 0; i < Nmarcos; i++) {
        if (marcos[i] == -1) {
            // asignar por primera vez
            marcos[i] = npv;
            tabla_paginas[npv] = i;
            marcar_uso(i);
            return i;
        }
    }

    // Si no hay libres → usar algoritmo Reloj
    int victima = reloj_elegir_victima();
    int npv_viejo = marcos[victima];

    // invalidar vieja página
    tabla_paginas[npv_viejo] = -1;

    // cargar nueva página
    marcos[victima] = npv;
    tabla_paginas[npv] = victima;

    marcar_uso(victima);

    return victima;
}

unsigned long construir_df(int marco, unsigned long offset, int b) {
    return ((unsigned long)marco << b) | offset;
}




// para parte 5


void print_verbose(unsigned long dv, unsigned long npv, unsigned long offset,
                   int marco, unsigned long df, int hit) {

    if (hit)
        printf("DV=%lu | npv=%lu | offset=%lu | HIT | marco=%d | DF=%lu\n",
                dv, npv, offset, marco, df);
    else
        printf("DV=%lu | npv=%lu | offset=%lu | FALLO | marco=%d | DF=%lu\n",
                dv, npv, offset, marco, df);
}