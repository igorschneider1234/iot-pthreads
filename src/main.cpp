// main.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <algorithm>
#include <cmath>
#include <unistd.h>

using namespace std;

struct Registro {
    string device;
    string ano_mes;
    float temperatura, umidade, luminosidade, ruido, eco2, etvoc;
};

struct Estatistica {
    float maximo = -INFINITY, minimo = INFINITY, soma = 0;
    int count = 0;
};

using Chave = pair<string, string>; // (device, ano-mes)
using Tabela = map<string, Estatistica>; // sensor -> estatistica
using Resultado = map<Chave, Tabela>;

mutex mtx;

bool data_valida(const string& data) {
    if (data.size() < 7) return false;
    int ano = stoi(data.substr(0, 4));
    int mes = stoi(data.substr(5, 2));
    return (ano > 2024 || (ano == 2024 && mes >= 3));
}

string obter_ano_mes(const string& data) {
    return data.substr(0, 7);
}

void processar(const vector<Registro>& dados, size_t inicio, size_t fim, Resultado& parcial) {
    for (size_t i = inicio; i < fim; ++i) {
        const auto& r = dados[i];
        Chave chave = {r.device, r.ano_mes};

        auto& tab = parcial[chave];
        pair<string, float> sensores[] = {
            {"temperatura", r.temperatura}, {"umidade", r.umidade},
            {"luminosidade", r.luminosidade}, {"ruido", r.ruido},
            {"eco2", r.eco2}, {"etvoc", r.etvoc}
        };

        for (auto& [nome, valor] : sensores) {
            auto& est = tab[nome];
            est.maximo = max(est.maximo, valor);
            est.minimo = min(est.minimo, valor);
            est.soma += valor;
            est.count++;
        }
    }
}

void unir(Resultado& destino, const Resultado& fonte) {
    lock_guard<mutex> lock(mtx);
    for (const auto& [chave, tab] : fonte) {
        for (const auto& [sensor, estat] : tab) {
            auto& dst = destino[chave][sensor];
            dst.maximo = max(dst.maximo, estat.maximo);
            dst.minimo = min(dst.minimo, estat.minimo);
            dst.soma += estat.soma;
            dst.count += estat.count;
        }
    }
}

int main() {
    ifstream arq("data/devices.csv");
    string linha;
    getline(arq, linha); // cabeçalho

    vector<Registro> dados;

    while (getline(arq, linha)) {
        stringstream ss(linha);
        string item;
        vector<string> campos;
        while (getline(ss, item, '|')) campos.push_back(item);

        if (campos.size() < 12 || !data_valida(campos[3])) continue;

        Registro r;
        r.device = campos[1];
        r.ano_mes = obter_ano_mes(campos[3]);
        r.temperatura = stof(campos[4]);
        r.umidade = stof(campos[5]);
        r.luminosidade = stof(campos[6]);
        r.ruido = stof(campos[7]);
        r.eco2 = stof(campos[8]);
        r.etvoc = stof(campos[9]);

        dados.push_back(r);
    }

    int num_threads = sysconf(_SC_NPROCESSORS_ONLN);
    vector<thread> threads;
    vector<Resultado> parciais(num_threads);
    size_t bloco = dados.size() / num_threads;

    for (int i = 0; i < num_threads; ++i) {
        size_t ini = i * bloco;
        size_t fim = (i == num_threads - 1) ? dados.size() : ini + bloco;
        threads.emplace_back(processar, cref(dados), ini, fim, ref(parciais[i]));
    }

    Resultado final;
    for (int i = 0; i < num_threads; ++i) {
        threads[i].join();
        unir(final, parciais[i]);
    }

    ofstream saida("output/resultado.csv");
    saida << "device;ano-mes;sensor;valor_maximo;valor_medio;valor_minimo\n";
    for (const auto& [chave, tabela] : final) {
        for (const auto& [sensor, estat] : tabela) {
            float media = estat.soma / estat.count;
            saida << chave.first << ";" << chave.second << ";" << sensor << ";"
                  << estat.maximo << ";" << media << ";" << estat.minimo << "\n";
        }
    }
    cout << "Processamento concluido. Resultados em 'output/resultado.csv'.\n";
    return 0;
}