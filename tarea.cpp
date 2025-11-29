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
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>

class MonitorBarrera {
private:
    std::mutex mtx;
    std::condition_variable cv;
    int contador;
    int total_hebras;
    int etapa;
    bool barrera_activa;

public:
    MonitorBarrera(int n_hebras) 
        : contador(0), total_hebras(n_hebras), etapa(0), barrera_activa(true) {
        std::cout << "Barrera creada para " << n_hebras << " hebras" << std::endl;
    }
    
    // Método para que las hebras esperen en la barrera
    void esperar() {
        // LOCK - Adquirir el lock del monitor
        std::unique_lock<std::mutex> lock(mtx);
        
        // Verificar si la barrera está activa
        if (!barrera_activa) {
            return;
        }
        
        // Capturar la etapa actual en variable local
        int mi_etapa = etapa;
        
        // Incrementar el contador de hebras que han llegado
        contador++;
        std::cout << "Hebra llego a la barrera, contador: " << contador << "/" << total_hebras 
                  << " (Etapa " << mi_etapa << ")" << std::endl;
        
        // Si no han llegado todas las hebras, esperar
        if (contador < total_hebras) {
            std::cout << "Hebra esperando..." << std::endl;
            // Esperar mientras no cambie la etapa
            cv.wait(lock, [this, mi_etapa]() {
                return !barrera_activa || etapa > mi_etapa;
            });
            std::cout << "Hebra despertada (nueva etapa " << etapa << ")" << std::endl;
        } else {
            // Si es la última hebra, realizar las tres acciones
            std::cout << "--- Última hebra llegó. Liberando todas las hebras ---" << std::endl;
            
            // (i) Incrementar etapa
            etapa++;
            
            // (ii) Resetear contador
            contador = 0;
            
            // (iii) Broadcast para despertar a todas
            cv.notify_all();
            std::cout << "--- Barrera reiniciada para nueva etapa " << etapa << " ---" << std::endl;
        }
    }
    
    // Método para obtener la etapa actual
    int obtener_etapa() {
        std::unique_lock<std::mutex> lock(mtx);
        return etapa;
    }
    
    // Método para desactivar la barrera en caso necesario
    void desactivar() {
        std::unique_lock<std::mutex> lock(mtx);
        barrera_activa = false;
        cv.notify_all();
        std::cout << "Barrera desactivada" << std::endl;
    }
};

void funcion_hebra(int id, MonitorBarrera& barrera, int num_etapas) {
    std::cout << "Hebra " << id << " iniciando para " << num_etapas << " etapas" << std::endl;
    
    for (int etapa = 0; etapa < num_etapas; etapa++) {
        // Simular trabajo antes de la barrera
        int tiempo_trabajo = 300 + rand() % (2000 - 300);
        std::cout << "Hebra " << id << " esperando en la etapa " << etapa << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(tiempo_trabajo));
        
        std::cout << "Hebra " << id << " deteniendose en la barrera (etapa " << etapa << ")" << std::endl;
        barrera.esperar();
        
        std::cout << "Hebra " << id << " paso barrera en etapa " << etapa << std::endl;
        
        // Pequeña pausa después de la barrera
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "Hebra " << id << " finalizada después de " << num_etapas << " etapas" << std::endl;
}

int main() {
    int nhebras, num_etapas;
    
    std::cout << "Introduzca el número de hebras: "; 
    std::cin >> nhebras;
    
    std::cout << "Introduzca el número de etapas: ";
    std::cin >> num_etapas;
    
    srand(time(NULL));
    MonitorBarrera barrera(nhebras);
    std::vector<std::thread> hebras;
    
    std::cout << "\nCreando " << nhebras << " hebras..." << std::endl;
    std::cout << "Ejecutando " << num_etapas << " etapas\n" << std::endl;
    
    for (int i = 0; i < nhebras; ++i) {
        hebras.emplace_back(funcion_hebra, i, std::ref(barrera), num_etapas);
    }
    
    for (auto& h : hebras) {
        h.join();
    }
    
    std::cout << "\n=== Todas las hebras terminaron después de " << num_etapas << " etapas ===" << std::endl;
    return 0;
}