#include "defs.h"
#include "parar_musica.h"

void liberar_memoria() {
    if (mapa_atual) {
        for (int i = 0; i < ROWS; i++) free(mapa_atual[i]);
        free(mapa_atual);
        mapa_atual = NULL;
    }
    if (inimigos) {
        free(inimigos);
        inimigos = NULL;
    }
    if (player.nome) {
        free(player.nome);
        player.nome = NULL;
    }
    parar_musica();
}