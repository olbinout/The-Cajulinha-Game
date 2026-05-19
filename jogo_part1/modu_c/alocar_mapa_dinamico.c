#include "defs.h"
#include "alocar_mapa_dinamico.h"

void alocar_mapa_dinamico() {
    if (mapa_atual != NULL) return;
    mapa_atual = (int**)malloc(ROWS * sizeof(int*));
    for (int i = 0; i < ROWS; i++) mapa_atual[i] = (int*)malloc(COLS * sizeof(int));
}
