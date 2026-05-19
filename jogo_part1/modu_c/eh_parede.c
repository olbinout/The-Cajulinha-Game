#include "eh_parede.h"
#include "defs.h"

// Verifica se um ponto específico é parede
bool eh_parede(float px, float py) {
    int col = (int)(px) / TILE_SIZE;
    int row = (int)(py) / TILE_SIZE;
      
    // Verifica se está fora dos limites da tela
    if (col < 0 || col >= COLS || row < 0 || row >= ROWS) return true;

    int bloco = mapa_atual[row][col];

    // Verifica obstáculos do mapa de Gelo
    if (bloco == 1 || bloco == 2) return true;

    // Verifica obstáculos do mapa de Floresta
    if (bloco == 5 || bloco == 6 || bloco == 7) return true;

    // ALTERADO: Verifica obstáculos do mapa de Deserto (8 = Cacto)
    // Nota: 9 é Cajuína, então não é parede.
    if (bloco == 8) return true;

    return false;
}

// Verifica se o Jogador (Hitbox) pode andar
bool pode_andar(float x, float y) {
    float x_esq = x;
    float x_dir = x + HITBOX_SIZE - 1;
    float y_cima = y;
    float y_baixo = y + HITBOX_SIZE - 1;

    if (eh_parede(x_esq, y_cima))   return false;
    if (eh_parede(x_dir, y_cima))   return false;
    if (eh_parede(x_esq, y_baixo))  return false;
    if (eh_parede(x_dir, y_baixo))  return false;

    return true;
}