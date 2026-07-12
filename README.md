# The Cajulinha Game 🐧🧊🌲🏜️🎮

Um jogo desenvolvido em linguagem **C** utilizando a biblioteca **Allegro 5**.

## Pré-requisitos (Linux)

Para conseguir compilar e rodar o jogo na sua máquina, você precisará ter o compilador GCC, a ferramenta Make e a biblioteca Allegro instalados no seu Linux. 

Em distribuições baseadas em Debian/Ubuntu, você pode instalar as dependências rodando o seguinte comando no terminal:

```bash
sudo apt update
sudo apt install build-essential gcc make
sudo apt install liballegro5-dev liballegro-image5-dev liballegro-primitives5-dev liballegro-font5-dev liballegro-ttf5-dev liballegro-audio5-dev liballegro-acodec5-dev
```

## Pré-requisitos (Windows)

Para conseguir compilar e rodar o jogo no Windows, utilizaremos o ambiente MSYS2. Siga os passos abaixo:

1. Baixe e instale o **[MSYS2](https://www.msys2.org/)**.
2. No menu Iniciar do Windows, procure e abra o terminal **MSYS2 UCRT64**.
3. Atualize os pacotes base do sistema rodando o seguinte comando (caso o terminal feche após atualizar, abra-o novamente e repita o comando):
   ```bash
   pacman -Syu
   ```
   (Nota: O terminal pode pedir para ser fechado automaticamente durante essa primeira atualização. Se isso acontecer, feche, abra o MSYS2 UCRT64 novamente e rode pacman -Su para terminar a atualização.)  
   
4. Instale o compilador GCC, a ferramenta Make e a biblioteca Allegro 5 rodando:
   ```bash
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-allegro make
   ```

## Instruções para rodar o jogo

Após realizar o clone do repositório, abra o seu terminal (o terminal padrão no Linux ou o **MSYS2 UCRT64** no Windows) e navegue até a pasta principal do código, que é a `jogo_part1`.

Para criar o executável:
```bash
make
```

Para rodar o jogo:
```bash
make run
```