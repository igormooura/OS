#ifndef CENTRAL_H
#define CENTRAL_H

#include <pthread.h>
#include <vector>
#include <string>
#include "defs.h"
#include "Forest.h"
using namespace std;

class Central {
private:
    Forest* floresta;
    pthread_t thread_central;
    bool ativa;

    vector<MensagemIncendio> fila_mensagens;
    pthread_mutex_t mutex_fila;

    vector<Coordenada> incendios_atendidos;

    static void* threadHelper(void* context);
    void cicloDeVida();
    void logarIncendio(MensagemIncendio msg);
    
    static void* rotinaBombeiro(void* arg); 

public:
    Central(Forest* f);
    ~Central();

    void iniciar();
    void aguardar();

    void receberMensagem(MensagemIncendio msg);
};

struct DadosBombeiro {
    Forest* floresta;
    int x, y;
};

#endif