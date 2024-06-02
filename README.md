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
### 3. Configurando Tools
Além do ESP-IDF, é necessário instalar as tools utilizadas pelo ESP-IDF, como compilador, debugger, pacotes Python, etc, para projetos que suportam ESP32.

Rode o comando:
```
cd ~/esp/esp-idf
./install.sh esp32
```

### 4. Configurando variáveis ambientais
As tools instaladas ainda não foram adicionadas à variável de ambiente PATH. Para tornar as ferramentas utilizáveis na linha de comando, algumas variáveis de ambiente devem ser definidas. ESP-IDF fornece outro script que faz isso.
No terminal, onde vai usar o ESP-IDF, execute:
```
. $HOME/esp/esp-idf/export.sh
```

Para facilitar o uso frequente do ESP-IDF, recomenda-se a criação de um alias.
1. Copie e cole o seguinte comando no perfil do seu shell (.profile, .bashrc, .zprofile, etc.)
```
alias get_idf='. $HOME/esp/esp-idf/export.sh'
```
2. Reinicie o terminal

### 5. Testando programa de exemplo
Crie uma pasta que usará para seus projetos, navegue até ela  e clone nosso repositório com o seguinte comando
1. Copie e cole o seguinte comando no perfil do seu shell (.profile, .bashrc, .zprofile, etc.)
```
git clone git@github.com:viniciusfersil123/ime-embarcados-lib.git
```
2. Rode
```
. $HOME/esp/esp-idf/export.sh
```
ou o alias correspondente

3. Ligue o ESP32 via USB em seu conmputador, rode 
```
ls /dev/tty*
```
e guarde o nome da porta USB

4. Rode
```
idf.py set-target esp32
```

5. Rode
```
idf.py build
```

6. Rode
```
idf.py -p PORT flash
```
substituindo PORT pelo nome da porta USB em que o ESP32 está conectado, por ex: `dev/ttyUSB0`. Se PORT não estiver disponível o ESP-IDF tentará fazer o flash nas portas USB disponíveis.

Obs: Caso seu usário não esteja autorizado a ler e escrever na porta USB referida, recomenda-se rodar:
```
sudo adduser <username> dialout
sudo chmod a+rw /dev/ttyUSB0
```
substituindo <username> pelo seu nome de usuário.

# Ligação ESP32 com DAC
Ligue os pinos do ESP32 e DAC seguindo a tabela abaixo:

| ESP32  |DAC |
| ------------- | ------------- |
| 3V3  | 3V0  |
| GND | GND  |
| GPIO26 | WSEL  |
| GPIO25 | DIN|
| GPIO27 |BCLK  |
