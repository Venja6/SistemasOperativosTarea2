#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// persona B
int buscar_marco(int npv);      // devuelve índice de marco o -1 si fallo
int asignar_o_reemplazar(int npv, int verbose); // asigna o reemplaza, devuelve marco
void marcar_uso(int marco);     // marca bit de referencia
void init_mem(int nmarcos);     // inicializa estructuras internas
void end_mem(void);             // libera estructuras internas


// DV en texto -> unsigned long
unsigned long parse_dv(const char *s) {
    // strtoul se encarga de base 10 o 16 automáticamente si usamos base 0
    char *end;
    errno = 0;
    unsigned long dv = strtoul(s, &end, 0);
    if (errno != 0 || *end != '\0') {
        fprintf(stderr, "DV inválida: '%s'\n", s);
        exit(1);
    }
    return dv;
}

// npv = DV >> b
unsigned long get_npv(unsigned long dv, int b) {
    return dv >> b;
}

// offset = DV & ((1<<b)-1)
unsigned long get_offset(unsigned long dv, int b) {
    unsigned long mask = (1UL << b) - 1;
    return dv & mask;
}

// DF = (marco << b) | offset
unsigned long make_df(unsigned long marco, unsigned long offset, int b) {
    return (marco << b) | offset;
}

// Calcula b tal que PAGE_SIZE = 2^b (= tamMarco)
int calcular_b(unsigned long page_size) {
    int b = 0;
    while ((1UL << b) < page_size) b++;
    if ((1UL << b) != page_size) {
        fprintf(stderr, "ERROR: tamMarco (%lu) no es potencia de 2\n", page_size);
        exit(1);
    }
    return b;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr,
            "Uso: %s Nmarcos tamMarco [--verbose] traza.txt\n", argv[0]);
        return 1;
    }

    int argi = 1;
    int Nmarcos = atoi(argv[argi++]);
    unsigned long page_size = strtoul(argv[argi++], NULL, 10);

    int verbose = 0;
    const char *tracefile = NULL;

    if (strcmp(argv[argi], "--verbose") == 0) {
        verbose = 1;
        argi++;
    }
    tracefile = argv[argi];

    int b = calcular_b(page_size);
    unsigned long MASK = (1UL << b) - 1;

    printf("Nmarcos=%d, PAGE_SIZE=%lu, b=%d, MASK=0x%lx, verbose=%d\n",
           Nmarcos, page_size, b, MASK, verbose);

    FILE *f = fopen(tracefile, "r");
    if (!f) {
        perror("fopen traza");
        return 1;
    }

    init_mem(Nmarcos);  // B 

    char line[256];
    unsigned long total_refs = 0;
    unsigned long fallos = 0;

    while (fgets(line, sizeof(line), f)) {
        // saltar líneas vacías
        if (line[0] == '\n' || line[0] == '#') continue;

        // quitar \n
        line[strcspn(line, "\r\n")] = '\0';

        unsigned long dv = parse_dv(line);
        unsigned long offset = get_offset(dv, b);
        unsigned long npv    = get_npv(dv, b);

        total_refs++;

        int marco = buscar_marco((int)npv);
        int hit = (marco >= 0);

        if (!hit) {
            fallos++;
            marco = asignar_o_reemplazar((int)npv, verbose);
        }

        marcar_uso(marco);

        unsigned long df = make_df((unsigned long)marco, offset, b);

        if (verbose) {
            printf("DV=%#lx npv=%lu offset=%lu -> %s marco=%d DF=%#lx\n",
                   dv, npv, offset,
                   hit ? "HIT " : "FALLO",
                   marco, df);
        }
    }

    fclose(f);
    end_mem();

    double tasa = (total_refs == 0) ? 0.0 : (double)fallos / total_refs;

    printf("Referencias: %lu\n", total_refs);
    printf("Fallos de página: %lu\n", fallos);
    printf("Tasa de fallos: %.4f\n", tasa);

    return 0;
}
