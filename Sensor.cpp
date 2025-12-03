#include "Sensor.h"
#include "Central.h"
#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <ctime>

using namespace std;

Sensor* Sensor::mapa_sensores[LINHA][COLUNA] = {NULL};

Sensor::Sensor(int id, int x, int y, Forest* floresta, Central* central) {
    this->id = id;
    this->x = x;
    this->y = y;
    this->floresta = floresta;
    this->central = central;
    this->ativo = false;

    if (x >= 0 && x < LINHA && y >= 0 && y < COLUNA) {
        mapa_sensores[x][y] = this;
    }
}

Sensor::~Sensor() {
    if (x >= 0 && x < LINHA && y >= 0 && y < COLUNA) {
        mapa_sensores[x][y] = NULL;
    }
}

void* Sensor::threadHelper(void* context) {
    Sensor* sensor = (Sensor*)context;
    sensor->cicloDeVida();
    return NULL;
}

void Sensor::iniciar() {
    this->ativo = true;
    pthread_create(&thread_id, NULL, &Sensor::threadHelper, this);
}

void Sensor::aguardar() {
    pthread_join(thread_id, NULL);
}

bool Sensor::souBorda() {
    if (x == 1 || x == LINHA - 2 || y == 1 || y == COLUNA - 2) {
        return true;
    }
    return false;
}

void Sensor::receberPropagacao(MensagemIncendio msg) {
    propagarMensagem(msg);
}

void Sensor::propagarMensagem(MensagemIncendio msg) {
    if (souBorda()) {
        central->receberMensagem(msg);
    } else {
        int dx[] = {-3, 3, 0, 0};
        int dy[] = {0, 0, -3, 3};

        int minhaDistanciaBorda = x; 
        if (LINHA - 1 - x < minhaDistanciaBorda) minhaDistanciaBorda = LINHA - 1 - x;
        if (y < minhaDistanciaBorda) minhaDistanciaBorda = y;
        if (COLUNA - 1 - y < minhaDistanciaBorda) minhaDistanciaBorda = COLUNA - 1 - y;

        for (int k = 0; k < 4; ++k) {
            int nx = x + dx[k];
            int ny = y + dy[k];

            if (nx >= 0 && nx < LINHA && ny >= 0 && ny < COLUNA) {
                Sensor* vizinho = mapa_sensores[nx][ny];
                
                if (vizinho != NULL) {
                    int distVizinho = nx;
                    if (LINHA - 1 - nx < distVizinho) distVizinho = LINHA - 1 - nx;
                    if (ny < distVizinho) distVizinho = ny;
                    if (COLUNA - 1 - ny < distVizinho) distVizinho = COLUNA - 1 - ny;
                    if (distVizinho < minhaDistanciaBorda) {
                        vizinho->receberPropagacao(msg);
                    }
                }
            }
        }
    }
}

void Sensor::cicloDeVida() {
    while (this->ativo) {
        sleep(1);

        if (floresta->getTipo(x, y) == TIPO_FOGO) {
            this->ativo = false;
            pthread_exit(NULL);
        }

        for (int i = x - 1; i <= x + 1; ++i) {
            for (int j = y - 1; j <= y + 1; ++j) {
                if (i == x && j == y) continue;

                char vizinho = floresta->getTipo(i, j);

                if (vizinho == TIPO_FOGO) {
                    MensagemIncendio msg;
                    msg.sensor_id = this->id;
                    msg.local_fogo = {i, j};
                    msg.ativo = true;

                    time_t agora;
                    struct tm * timeinfo;
                    char buffer[80];

                    time(&agora);
                    timeinfo = localtime(&agora);

                    strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
                    msg.horario = string(buffer);

                    propagarMensagem(msg);
                    usleep(500000); 
                }
            }
        }
    }
}