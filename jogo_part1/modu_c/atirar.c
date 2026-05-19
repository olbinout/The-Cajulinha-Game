#include "defs.h"
#include "atirar.h"

void atirar() {
    if (!player.tem_arma || player.municao <= 0) return;

    for(int i=0; i<MAX_TIROS; i++) {
        if (!tiros[i].ativo) {
            tiros[i].ativo = true;
            tiros[i].pos.x = player.pos.x + TILE_SIZE/2 - 5; 
            tiros[i].pos.y = player.pos.y + TILE_SIZE/2 - 5;
             
            if (player.dir.x != 0 || player.dir.y != 0) {
                tiros[i].dir = player.dir; 
            } else {
                tiros[i].dir.x = (player.ultimo_dir_x == -1) ? -1 : 1;
                tiros[i].dir.y = 0;
            }

            player.municao--; 
            
            if (player.municao <= 0) {
                player.tem_arma = false;
                timer_respawn_arma = 15.0; 
            }
            
            break;
        }
    }
}