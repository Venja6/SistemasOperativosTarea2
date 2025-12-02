#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"

void leer_argumentos(int argc, char *argv[], int *Nmarcos, int *page_size, char **traza);
void leer_traza(const char *archivo, int b);

// para usar init_clock
extern void init_clock(int cantidad_marcos);
extern unsigned long construir_df(int marco, unsigned long offset, int b);
extern int calcular_b(unsigned long page_size);
extern unsigned long parse_linea_dv(const char *s);

// parte 2, Punto 5
int total_referencias = 0;
int total_fallos = 0;
int modo_verbose = 1;


void procesar_dv(unsigned long dv, int b);
void imprimir_estadisticas();

int main(int argc, char *argv[]) {

    printf("Simulador iniciado...\n");

    // // -------(Test Parte 2 de la Tarea y punto 2. Solo para probar clock)--------
    // // por ahora probar CLOCK nomás
    // init_clock(3); // 3 marcos por ejemplo
    // printf("Cargando página 10 = marco %d\n", buscar_marco(10));
    // printf("Cargando página 11 = marco %d\n", buscar_marco(11));
    // printf("Cargando página 12 = marco %d\n", buscar_marco(12)); // debería reemplazar
    // printf("Accediendo página 10 = marco %d (HIT si sigue cargada)\n", buscar_marco(10));
    // unsigned long df = construir_df(0, 5, 12); // ((marco 0, offset 5, tamaño pagina 2^12 = 4096))
    // printf("DF de prueba = %lu\n", df);


    // // ----- (Test Punto 4 de la parte 2)  ------
    // int b = 12;        // por ahora tamaño de página 2^12 = 4096
    // init_clock(3);     // por ahora usar 3 marcos para pruebas

    // // pruebas usando procesar_dv 
    // procesar_dv(100, b);
    // procesar_dv(200, b);
    // procesar_dv(300, b);
    // procesar_dv(100, b);

    // // mostrar resultados
    // imprimir_estadisticas();
    
    int Nmarcos = 0;
    int page_size = 0;
    char *archivo_traza = NULL;

    leer_argumentos(argc, argv, &Nmarcos, &page_size, &archivo_traza);

    // Calcular b como log2(page_size)
    int b = calcular_b(page_size);

    init_clock(Nmarcos);

    printf("Simulador iniciado...\n");
    printf("Marcos = %d | PageSize = %d | b = %d | verbose = %d\n",
            Nmarcos, page_size, b, modo_verbose);

    leer_traza(archivo_traza, b);

    imprimir_estadisticas();

    return 0;
}

void procesar_dv(unsigned long dv, int b) {

    total_referencias++;

    // Aquí cuando A termine, reemplazas estas dos líneas:
    unsigned long npv = get_npv(dv, b);
    unsigned long offset = get_offset(dv, b);

    // Por ahora A usa stubs que retornan 0, pero igual funciona el flujo.
    
    // Preguntar si HIT o FALLO
    int marco_anterior = tabla_paginas[npv] != -1; // 1 = HIT, 0 = FALLO

    int marco = buscar_marco(npv);

    if (!marco_anterior)
        total_fallos++;

    unsigned long df = construir_df(marco, offset, b);

    if (modo_verbose) {
        print_verbose(dv, npv, offset, marco, df, marco_anterior);
    }
}

void imprimir_estadisticas() {
    printf("\n--- RESULTADOS ---\n");
    printf("Total referencias: %d\n", total_referencias);
    printf("Fallos de página: %d\n", total_fallos);

    double tasa = 0.0;
    if (total_referencias > 0)
        tasa = (double) total_fallos / total_referencias;

    printf("Tasa de fallos: %.4f\n", tasa);
}


void leer_argumentos(int argc, char *argv[], int *Nmarcos, int *page_size, char **traza) {

    if (argc < 4) {
        printf("Uso: %s Nmarcos tamanioMarco [--verbose] traza.txt\n", argv[0]);
        exit(1);
    }

    *Nmarcos = atoi(argv[1]);
    *page_size = atoi(argv[2]);

    // detectar verbose
    if (argc == 5 && strcmp(argv[3], "--verbose") == 0) {
        modo_verbose = 1;
        *traza = argv[4];
    } else {
        modo_verbose = 0;
        *traza = argv[3];
    }
}

void leer_traza(const char *archivo, int b) {
    FILE *f = fopen(archivo, "r");
    if (!f) {
        printf("Error abriendo archivo: %s\n", archivo);
        exit(1);
    }

    char linea[256];

    while (fgets(linea, sizeof(linea), f)) {

        if (linea[0] == '\n' || linea[0] == '#') continue;

        linea[strcspn(linea, "\r\n")] = '\0';

        unsigned long dv = parse_linea_dv(linea);

        procesar_dv(dv, b);
    }

    fclose(f);
}