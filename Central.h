#ifndef CENTRAL_H
#define CENTRAL_H

#include <pthread.h>
#include <vector>
#include <string>
#include "defs.h"
#include "Forest.h"
using namespace std;

struct DadosBombeiro {
    Forest* floresta;
    int x, y;
    Central* central;
};

class Central {
private:
    Forest* floresta;
    pthread_t thread_central;
    bool ativa;

    vector<MensagemIncendio> fila_mensagens;
    pthread_mutex_t mutex_fila;

    vector<Coordenada> incendios_atendidos;

    static void* threadHelper(void* context);
    static void* rotinaBombeiro(void* arg);

    void cicloDeVida();
    void RegistraLog(MensagemIncendio msg);

public:
    Central(Forest* f);
    ~Central();

    void iniciar();
    void aguardar();
    void receberMensagem(MensagemIncendio msg);

    bool incendioJaAtendido(const Coordenada& coord);
    bool isIncendioDuplicado(const Coordenada& local_fogo);

    void apagarIncendio(DadosBombeiro* dados);
};

#endif
