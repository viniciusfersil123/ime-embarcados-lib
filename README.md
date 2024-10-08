# ime-embarcados-lib

## Instalação

### Ubuntu

#### 1. Instalando pré-requisitos

Execute o comando abaixo para instalar as dependências necessárias

``` bash
sudo apt-get install \
  git \
  wget \
  flex \
  bison \
  gperf \
  python3 \
  python3-pip \
  python3-venv \
  cmake \
  ninja-build \
  ccache \
  libffi-dev \
  libssl-dev \
  dfu-util \
  libusb-1.0-0
```

#### 2. Clonando ESP-IDF

Para desenvolver aplicativos para ESP32, é necessário instalar as bibliotecas fornecidas pela **Espressif** no [repositório ESP-IDF](https://github.com/espressif/esp-idf).

Para clonar o ESP-IDF, navegue até o diretório de instalação e clone o repositório com git clone, seguindo as instruções abaixo.

Abra o Terminal e execute os seguintes comandos:

``` bash
mkdir -p ~/esp
cd ~/esp
git clone -b v5.2.1 --recursive https://github.com/espressif/esp-idf.git
```

#### 3. Configurando Tools

Além do ESP-IDF, é necessário instalar as tools utilizadas pelo ESP-IDF para projetos que suportam ESP32 (como compilador, debugger, pacotes Python, etc.)

Execute o comando

``` bash
cd ~/esp/esp-idf
./install.sh esp32
```

para instalar as tools necessárias para o ESP32.

#### 4. Configurando variáveis ambientais

As tools instaladas ainda não foram adicionadas à variável de ambiente PATH. Para tornar as ferramentas utilizáveis na linha de comando, algumas variáveis de ambiente devem ser definidas. ESP-IDF fornece outro script que faz isso.

<!-- No terminal, onde vai usar o ESP-IDF, execute: -->
No terminal, execute o comando abaixo para configurar as variáveis de ambiente:

``` bash
. $HOME/esp/esp-idf/export.sh
```

Para facilitar o uso frequente do ESP-IDF, recomenda-se a criação de um `alias`. O comando abaixo cria um atalho para o comando acima.

Copie e cole o seguinte comando no perfil do seu shell *(.profile, .bashrc, .zprofile, etc.)*

``` bash
alias get_idf='. $HOME/esp/esp-idf/export.sh'
```

e reinicie o terminal.

#### 5. Testando um programa de exemplo

Crie uma pasta que usará para seus projetos, navegue até ela  e clone nosso repositório com o seguinte comando

Clone o repositorio `ime-embarcados-lib` com o comando abaixo

``` bash
git clone --recursive git@github.com:viniciusfersil123/ime-embarcados-lib.git
```

e execute

``` bash
. $HOME/esp/esp-idf/export.sh
```

**OU** o alias correspondente

``` bash
get_idf
```

para configurar as variáveis de ambiente.

Ligue o ESP32 via USB em seu computador.

Execute

``` bash
ls /dev/tty*
```

e guarde o nome da porta USB em que o ESP32 está conectado.

Execute

``` bash
idf.py set-target esp32
idf.py build
idf.py -p PORT flash
```

substituindo `PORT` pelo nome da porta USB em que o ESP32 está conectado, por ex: `dev/ttyUSB0`. Se `PORT` não estiver disponível, o ESP-IDF tentará fazer o flash nas portas USB disponíveis.

Obs: Caso seu usuário não esteja autorizado a ler e escrever na porta USB referida, recomenda-se executar

``` bash
sudo adduser <username> dialout
sudo chmod a+rw /dev/ttyUSB0
```

substituindo `<username>` pelo seu nome de usuário.

## Ligação ESP32 com DAC

Ligue os pinos do ESP32 e DAC seguindo a tabela abaixo:

| ESP32  |DAC |
| ------------- | ------------- |
| 3V3  | 3V0  |
| GND | GND  |
| GPIO26 | WSEL  |
| GPIO25 | DIN|
| GPIO27 |BCLK  |
