#ifndef MONITOR_BARRERA_H
#define MONITOR_BARRERA_H

#include <pthread.h>

// Estructura del monitor de barrera
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t condicion;
    int contador;
    int total_hebras;
    int etapa;
    int barrera_activa;
} MonitorBarrera;

// Estructura para argumentos de hebras
typedef struct {
    int id;
    MonitorBarrera* barrera;
    int num_etapas;
} ThreadArgs;

// Funciones del monitor
void monitor_init(MonitorBarrera* monitor, int n_hebras);
void monitor_destroy(MonitorBarrera* monitor);
void monitor_esperar(MonitorBarrera* monitor, int id_hebra);

// Funcion de trabajo de la hebra
void* funcion_hebra(void* arg);

#endif // MONITOR_BARRERA_H