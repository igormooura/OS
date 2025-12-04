#include "Central.h"
#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace std;

Central::Central(Forest* f) {
    this->floresta = f;
    this->ativa = false;

    // inicializa o mutex pra evitar que 2 threads diferentes usem a mesma mensagem
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

// Helper estático que chama cicloDeVida
void* Central::threadHelper(void* context) {
    ((Central*)context)->cicloDeVida();
    return NULL;
}

void Central::iniciar() {
    ativa = true;
    pthread_create(&thread_central, NULL, &Central::threadHelper, this);
}

void Central::aguardar() {
    //espera a central terminar, ou seja, no final do programa
    pthread_join(thread_central, NULL);
}

bool Central::incendioJaAtendido(const Coordenada& coord) {
    for (const auto& c : incendios_atendidos) {
        if (c == coord) {
            return true;
        }
    }
    return false;
}

bool Central::isIncendioDuplicado(const Coordenada& local_fogo) {
    for (const auto& coord : incendios_atendidos) {
        if (coord == local_fogo) {
            return true;
        }
    }
    return false;
}

void Central::apagarIncendio(DadosBombeiro* dados) {
    // aqui ele apaga o fogo e diz que tá livre pro código
    dados->floresta->setTipo(dados->x, dados->y, TIPO_LIVRE);
}

bool Central::obterProximaMensagem(MensagemIncendio& msg) {

    // trava a fila, impedindo que outra thread acesse enquanto uma thread esteja acessando a fila  
    pthread_mutex_lock(&mutex_fila);

    if (fila_mensagens.empty()) {
        pthread_mutex_unlock(&mutex_fila);
        return false;
    }

    msg = fila_mensagens.front();
    fila_mensagens.erase(fila_mensagens.begin());

    pthread_mutex_unlock(&mutex_fila);
    return true;
}

void Central::processarIncendio(const MensagemIncendio& msg) {
    if (incendioJaAtendido(msg.local_fogo)) {
        return;
    }

    incendios_atendidos.push_back(msg.local_fogo);

    RegistraLog(msg);

    DadosBombeiro* dados = new DadosBombeiro{
        floresta,
        msg.local_fogo.x,
        msg.local_fogo.y,
        this
    };

    spawnBombeiro(dados);
}

void Central::spawnBombeiro(DadosBombeiro* dados) {
    pthread_t t;

    // cria o bombeiro independente
    pthread_create(&t, NULL, &Central::rotinaBombeiro, dados);

    // detach é usado porque o bombeiro é uma thread independente
    // não usamos join pra dar o NULL pq a Central não pode ficar esperando o bombeiro terminar,
    // quando a thread acabar, o detach limpa automaticamente os recursos
    pthread_detach(t);
}

void Central::cicloDeVida() {
    while (ativa) {
        MensagemIncendio msg;

        if (obterProximaMensagem(msg)) {
            processarIncendio(msg);
        }

        usleep(100000);
    }
}

void Central::RegistraLog(MensagemIncendio msg) {
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

    // dados->central porque rotinaBombeiro é static
    // e não tem acesso ao 'this'. 
    // Então o ponteiro da Central é enviado dentro de DadosBombeiro para permitir chamar
    // métodos da Central a partir da thread

    // dados->central acessa os membros da struct
    // -> apagarIncendio chama o método
    dados->central->apagarIncendio(dados);

    delete dados;
    return NULL;
}
