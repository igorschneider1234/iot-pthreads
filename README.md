Este projeto em C++ utiliza **pthreads** para processar uma base de dados de dispositivos IoT, calculando mensalmente os valores **mínimo**, **médio** e **máximo** de diferentes sensores por dispositivo.

## ⚙️ Requisitos

- Linux com suporte a pthreads
- g++
- make
- VSCode (opcional)

## 🛠️ Compilação e Execução

### 1. Dê permissão de execução ao script:

```bash
chmod +x run.sh

2. Execute o programa:
bash
./run.sh

Resultado:
Um arquivo chamado resultado.csv será criado na pasta output/.

📌 Formato do resultado
Cada linha representa os dados de um sensor por mês:

device;ano-mes;sensor;valor_maximo;valor_medio;valor_minimo
Exemplo:

sirrosteste_UCS_AMV-04;2024-04;temperatura;30.3;23.3216;19.3

📅 Filtro de Data
Somente registros a partir de março de 2024 são considerados.