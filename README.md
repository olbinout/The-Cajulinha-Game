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

## Instruções para rodar o jogo

Após o git clone, você deve entrar no diretório gerado e chamar o Make dessa forma:

Para criar o executável:
```bash
make
```

Para rodar o jogo:
```bash
make run
```