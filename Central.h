#ifndef CENTRAL_H
#define CENTRAL_H

#include <vector>
#include <pthread.h>
#include "Forest.h"

class Central;

struct DadosBombeiro {
    Forest* floresta;
    int x;
    int y;
    Central* central;
};

class Central {
private:
    Forest* floresta;
    bool ativa;
    pthread_t thread_central;

    pthread_mutex_t mutex_fila;
    std::vector<MensagemIncendio> fila_mensagens;
    std::vector<Coordenada> incendios_atendidos;

    bool obterProximaMensagem(MensagemIncendio& msg);
    void processarIncendio(const MensagemIncendio& msg);
    void spawnBombeiro(DadosBombeiro* dados);

    static void* threadHelper(void* context);
    static void* rotinaBombeiro(void* arg);

public:
    Central(Forest* f);
    ~Central();

    void receberMensagem(MensagemIncendio msg);
    void iniciar();
    void aguardar();

    bool incendioJaAtendido(const Coordenada& coord);
    bool isIncendioDuplicado(const Coordenada& local_fogo);

    void apagarIncendio(DadosBombeiro* dados);
    void cicloDeVida();

    void RegistraLog(MensagemIncendio msg);
};

#endif
