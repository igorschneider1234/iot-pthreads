# Análise de dados de sensores de IoT com Pthreads

Este projeto em C/C++ utiliza **pthreads** para processar uma base de dados de dispositivos IoT a partir de um arquivo CSV. O objetivo é distribuir a carga entre os núcleos da CPU para realizar o cálculo mensal dos valores **mínimo**, **máximo** e **médio** para cada tipo de sensor de cada dispositivo. O resultado final é exportado em um novo CSV.

---

## ✅ Instruções de compilação e execução

1. Torne o script `run.sh` executável:

```bash
chmod +x run.sh
```

2. Execute o script:

```bash
./run.sh
```

3. O resultado será salvo em:

```
output/resultado.csv
```

---

## 📅 Como o CSV é carregado

O programa espera um arquivo `input/dataset.csv` no formato delimitado por `;` contendo os seguintes campos:

```
dispositivo;data_hora;sensor;valor
```

Exemplo:

```
sirrosteste_UCS_AMV-04;2024-04-01T13:00:00;temperatura;23.4
```

Esse CSV é carregado na memória e particionado em blocos de dados para que cada thread possa processar um subconjunto específico.

---

## 🧵 Distribuição da carga entre as threads

A distribuição da carga é feita com base no número de threads disponíveis (definido automaticamente pelo número de núcleos do sistema ou manualmente no código). Os registros são divididos em blocos aproximadamente iguais entre as threads.

Cada thread é responsável por processar seus próprios registros de forma independente, filtrando e agregando os dados por:

* **Dispositivo**
* **Mês**
* **Sensor**

---

## 📊 Análise dos dados pelas threads

Cada thread percorre seu conjunto de dados e:

1. Agrupa os valores por combinação `dispositivo + ano-mês + sensor`.
2. Calcula os valores:

   * **mínimo**
   * **máximo**
   * **médio** (soma dos valores dividida pela quantidade)

Esses resultados são armazenados em uma estrutura intermediária thread-safe para posterior escrita no arquivo CSV.

---

## 📁 Geração do arquivo CSV com os resultados

Após todas as threads finalizarem o processamento, a **thread principal** coleta os resultados agregados e grava no arquivo `output/resultado.csv` no seguinte formato:

```
device;ano-mes;sensor;valor_maximo;valor_medio;valor_minimo
sirrosteste_UCS_AMV-04;2024-04;temperatura;30.3;23.3216;19.3
```

A escrita no arquivo é feita sequencialmente pela thread principal, evitando problemas de concorrência.

---

## 🧠 Execução em modo usuário ou núcleo

As threads são criadas com a biblioteca POSIX Threads (**pthreads**), executando em **modo usuário**, com suporte do sistema operacional para escalonamento entre múltiplos núcleos. Não são utilizadas threads em modo núcleo (kernel threads) diretamente pelo programa.

---

## ⚠️ Concorrência e sincronização

Como cada thread trabalha com um subconjunto exclusivo dos dados e os resultados parciais são consolidados pela thread principal, **não há acesso simultâneo às mesmas estruturas de dados**. Isso evita condições de corrida.

Para futuras melhorias com escrita paralela em um mesmo recurso compartilhado (como arquivos), recomenda-se o uso de **mutexes** ou estruturas sincronizadas.

---

## 📂 Estrutura de diretórios esperada

```
├── input/
│   └── dataset.csv        # Arquivo original com os dados
├── output/
│   └── resultado.csv      # Arquivo gerado com o resultado final
├── src/
│   └── main.c             # Código-fonte principal
├── run.sh                 # Script para compilar e executar
└── README.md              # Este arquivo
```

---
