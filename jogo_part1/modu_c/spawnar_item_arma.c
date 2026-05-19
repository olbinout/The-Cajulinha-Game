#include "defs.h"
#include "spawnar_item_arma.h"

void spawnar_item_arma() {
    int r, c;
    
    do {
        r = rand() % ROWS;
        c = rand() % COLS;
    } while (mapa_atual[r][c] == 1 || mapa_atual[r][c] == 2 || mapa_atual[r][c] == 5 || mapa_atual[r][c] == 6 || mapa_atual[r][c] == 7 || mapa_atual[r][c] == 8); 

    pos_item_arma.x = c * TILE_SIZE;
    pos_item_arma.y = r * TILE_SIZE;
    item_ativo_no_mapa = true;
}