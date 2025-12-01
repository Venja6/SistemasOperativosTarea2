#include "monitor_barrera.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

// Inicializacion del monitor
void monitor_init(MonitorBarrera* monitor, int n_hebras) {
    pthread_mutex_init(&monitor->mutex, NULL);
    pthread_cond_init(&monitor->condicion, NULL);
    monitor->contador = 0;
    monitor->total_hebras = n_hebras;
    monitor->etapa = 0;
    monitor->barrera_activa = 1;
}

// Destruccion del monitor al final del programa
void monitor_destroy(MonitorBarrera* monitor) {
    pthread_mutex_destroy(&monitor->mutex);
    pthread_cond_destroy(&monitor->condicion);
}

// Metodo para que una hebra espere en la barrera
void monitor_esperar(MonitorBarrera* monitor, int id_hebra) {
    // LOCK - Adquirir el mutex antes de modificar el estado compartido
    pthread_mutex_lock(&monitor->mutex);
    
    // Verificar si la barrera está activa
    if (!monitor->barrera_activa) {
        pthread_mutex_unlock(&monitor->mutex);
        return;
    }
    
    // Capturar la etapa actual en variable local 
    int mi_etapa = monitor->etapa;
    
    // Incrementar el contador de hebras que han llegado
    monitor->contador++;
    
    // Si no han llegado todas las hebras, la hebra actual espera
    if (monitor->contador < monitor->total_hebras) {
        // Esperar mientras no cambie la etapa
        while (monitor->barrera_activa && monitor->etapa == mi_etapa) {
            pthread_cond_wait(&monitor->condicion, &monitor->mutex);
        }
    } else {
        // Si es la última hebra...
        printf("--- Fin de etapa %d ---\n\n", monitor->etapa);
        
        // Incrementar etapa
        monitor->etapa++;
        
        // Resetear contador
        monitor->contador = 0;
        
        // Broadcast para despertar a todas las hebras en espera
        pthread_cond_broadcast(&monitor->condicion);
    }
    
    // UNLOCK - Liberar el mutex
    pthread_mutex_unlock(&monitor->mutex);
}


// Función de la hebra
void* funcion_hebra(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int id = args->id;
    MonitorBarrera* barrera = args->barrera;
    int num_etapas = args->num_etapas;
    
    for (int etapa = 0; etapa < num_etapas; etapa++) {
        // Simular trabajo de la hebra
        int tiempo_trabajo = 300 + rand() % (2000 - 300);
        printf("Hebra %d trabajando en etapa %d\n", id, etapa);
        usleep(tiempo_trabajo * 1000);
        
        printf("Hebra %d detenida en etapa %d\n", id, etapa);
        monitor_esperar(barrera, id);
    }
    
    // liberacion de memoria 
    free(args);  
    return NULL;
}