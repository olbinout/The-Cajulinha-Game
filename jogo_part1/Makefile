CC = gcc

CFLAGS = -I modu_h

LIBS = -lallegro -lallegro_image -lallegro_primitives -lallegro_font -lallegro_ttf -lallegro_audio -lallegro_acodec -lm

SRC = jogo.c modu_c/*.c

# --- Detecção automática de Sistema Operacional ---
ifeq ($(OS),Windows_NT)
    TARGET = jogo.exe
else
    TARGET = jogo
endif
# --------------------------------------------------

all: $(TARGET)

$(TARGET):
				$(CC) $(SRC) $(CFLAGS) -o $(TARGET) $(LIBS)

run: $(TARGET)
				./$(TARGET)

clean:
				rm -f $(TARGET)