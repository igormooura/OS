#include <iostream>
#include <vector>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <pthread.h>

#include "defs.h"
#include "Forest.h"
#include "Sensor.h"
#include "Central.h"

using namespace std;

Forest floresta;
Central central(&floresta);
bool simulacao_ativa = true;

void* thread_gerador_fogo(void* args) {
    srand(time(NULL));
    while (simulacao_ativa) {
        sleep(5);
        
        int x = rand() % LINHA;
        int y = rand() % COLUNA;
        
        floresta.setTipo(x, y, TIPO_FOGO);
    }
    return NULL;
}

int main() {
    vector<Sensor*> lista_sensores;
    int id_contador = 0;

    for (int i = 1; i < LINHA - 1; i += 3) {
        for (int j = 1; j < COLUNA - 1; j += 3) {
            
            floresta.setTipo(i, j, TIPO_SENSOR);
            
            Sensor* s = new Sensor(id_contador++, i, j, &floresta, &central);
            lista_sensores.push_back(s);
            
            s->iniciar();
        }
    }

    central.iniciar();

    pthread_t t_fogo;
    pthread_create(&t_fogo, NULL, thread_gerador_fogo, NULL);

    while (simulacao_ativa) {
        floresta.imprimir();
        cout << "Monitorando... (Pressione Ctrl+C para sair)\n";
        sleep(1);
    }

    return 0;
}