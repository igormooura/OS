#include "Central.h"
#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace std;

Central::Central(Forest* f) {
    this->floresta = f;
    this->ativa = false;
    pthread_mutex_init(&mutex_fila, NULL);
}

Central::~Central() {
    pthread_mutex_destroy(&mutex_fila);
}

void Central::receberMensagem(MensagemIncendio msg) {
    pthread_mutex_lock(&mutex_fila);
    fila_mensagens.push_back(msg);
    pthread_mutex_unlock(&mutex_fila);
}

void* Central::threadHelper(void* context) {
    ((Central*)context)->cicloDeVida();
    return NULL;
}

void Central::iniciar() {
    ativa = true;
    pthread_create(&thread_central, NULL, &Central::threadHelper, this);
}

void Central::aguardar() {
    pthread_join(thread_central, NULL);
}

void Central::cicloDeVida() {
    while (ativa) {
        MensagemIncendio msgAtual;
        bool temMensagem = false;

        pthread_mutex_lock(&mutex_fila);
        if (!fila_mensagens.empty()) {
            msgAtual = fila_mensagens.front();
            fila_mensagens.erase(fila_mensagens.begin());
            temMensagem = true;
        }
        pthread_mutex_unlock(&mutex_fila);

        if (temMensagem) {
            bool duplicado = false;
            for (const auto& coord : incendios_atendidos) {
                if (coord == msgAtual.local_fogo) {
                    duplicado = true;
                    break;
                }
            }

            if (!duplicado) {
                incendios_atendidos.push_back(msgAtual.local_fogo);
                logarIncendio(msgAtual);

                pthread_t t_bombeiro;
                DadosBombeiro* dados = new DadosBombeiro{floresta, msgAtual.local_fogo.x, msgAtual.local_fogo.y};
                
                pthread_create(&t_bombeiro, NULL, &Central::rotinaBombeiro, dados);
                pthread_detach(t_bombeiro); 
            }
        }
        usleep(100000);
    }
}

void Central::logarIncendio(MensagemIncendio msg) {
    ofstream arquivo("incendios.log", ios::app);
    
    if (arquivo.is_open()) {
        arquivo << "Incendio detectado pelo Sensor " << msg.sensor_id << "\n";
        arquivo << "Local: [" << msg.local_fogo.x << ", " << msg.local_fogo.y << "]\n";
        arquivo << "Horario: " << msg.horario << "\n";
        arquivo << "--------------------------------\n";
        arquivo.close();
    }
}

void* Central::rotinaBombeiro(void* arg) {
    DadosBombeiro* dados = (DadosBombeiro*)arg;
    
    sleep(2);
    dados->floresta->setTipo(dados->x, dados->y, TIPO_LIVRE);

    delete dados;
    return NULL;
}