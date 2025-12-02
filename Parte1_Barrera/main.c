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

int main() {
    int nhebras, num_etapas;
    
    printf("Introduzca el número de hebras: ");
    scanf("%d", &nhebras);
    
    printf("Introduzca el número de etapas: ");
    scanf("%d", &num_etapas);
    
    srand(time(NULL));
    
    // Crear el monitor
    MonitorBarrera barrera;
    monitor_init(&barrera, nhebras);
    
    // Crear arreglo de hebras
    pthread_t* hebras = (pthread_t*)malloc(nhebras * sizeof(pthread_t));
    if (hebras == NULL) {
        perror("Error al asignar memoria para hebras");
        return 1;
    }
    
    printf("\nCreando %d hebras...\n", nhebras);
    printf("Ejecutando %d etapas\n\n", num_etapas);
    
    // Crear y lanzar las hebras
    for (int i = 0; i < nhebras; ++i) {
        ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
        if (args == NULL) {
            perror("Error al asignar memoria para argumentos");
            free(hebras);
            return 1;
        }
        args->id = i;
        args->barrera = &barrera;
        args->num_etapas = num_etapas;
        
        if (pthread_create(&hebras[i], NULL, funcion_hebra, (void*)args) != 0) {
            perror("Error al crear hebra");
            free(args);
            free(hebras);
            return 1;
        }
    }
    
    // Esperar a que todas las hebras terminen
    for (int i = 0; i < nhebras; ++i) {
        if (pthread_join(hebras[i], NULL) != 0) {
            perror("Error al esperar hebra");
        }
    }
    
    // Liberar recursos
    free(hebras);
    monitor_destroy(&barrera);
    
    printf("\nTodas las hebras terminaron después de %d etapas\n", num_etapas);
    return 0;
}