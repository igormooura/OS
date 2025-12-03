#include "Forest.h"
#include <iostream>
#include <unistd.h>

using namespace std;

Forest::Forest(){
    pthread_mutex_init(&mapa_mutex, NULL);
    inicializar();
}

Forest::~Forest(){
    pthread_mutex_destroy(&mapa_mutex);
}

void Forest::inicializar(){
    for(int i = 0; i < LINHA; ++i){
        for(int j = 0; j < COLUNA; ++j){
            grid[i][j] = TIPO_LIVRE;
        }
    }

}
char Forest::getTipo(int x, int y){
    if( x < 0 || x >= LINHA || y < 0 || y >= COLUNA){
        return '\0';
    }
    pthread_mutex_lock(&mapa_mutex);
    char tipo = grid[x][y];
    pthread_mutex_unlock(&mapa_mutex);

    return tipo;
}

void Forest::setTipo(int x, int y, char tipo){
    if(x < 0 || x >= LINHA || y < 0 || y >= COLUNA){
        return;
    }

    pthread_mutex_lock(&mapa_mutex);
    grid[x][y] = tipo ;
    pthread_mutex_unlock(&mapa_mutex);
}

void Forest::imprimir(){
    pthread_mutex_lock(&mapa_mutex);

    system("clear");

    cout << " --- Monitor de Incendios Florestais --- \n";
    for (int i = 0; i < LINHA; ++i) {
        for (int j = 0; j < COLUNA; ++j) {
            char celula = grid[i][j];
            if (celula == TIPO_FOGO) {
                cout << "\033[1;31m" << celula << "\033[0m ";
            } 
            else if (celula == TIPO_LIVRE) {
                cout << "\033[32m" << celula << "\033[0m ";
            } 
            else if (celula == TIPO_SENSOR) {
                cout << "\033[1;33m" << celula << "\033[0m ";
            } 
            else {
                cout << celula << " ";
            }
        }
        cout << "\n";
    }
    pthread_mutex_unlock(&mapa_mutex);
}



