# ime-embarcados-lib

# Instalação
## Ubuntu
### 1. Instalando pré-requisitos
Rode o comando abaixo para instalar os pacotes necessários
```
sudo apt-get install git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
```
### 2. Clonando ESP-IDF
Para desenvolver aplicativos para ESP32, você precisa das bibliotecas fornecidas pela Espressif no [repositório ESP-IDF](https://github.com/espressif/esp-idf).

Para clonar o ESP-IDF, navegue até o diretório de instalação e clone o repositório com git clone, seguindo as instruções abaixo.

Abra o Terminal e execute os seguintes comandos:
Rode o comando abaixo para instalar os pacotes necessários
```
mkdir -p ~/esp
cd ~/esp
git clone -b v5.2.1 --recursive https://github.com/espressif/esp-idf.git
```
