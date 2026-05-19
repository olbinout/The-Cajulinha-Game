#include "colide_com_outros_inimigos.h"
#include "defs.h"

bool colide_com_outros_inimigos(int meu_id, float x, float y) {
    for (int i = 0; i < num_inimigos_selecionados; i++) { 
        if (i == meu_id || !inimigos[i].ativo) continue; 
        if (fabs(x - inimigos[i].pos.x) < HITBOX_SIZE && 
            fabs(y - inimigos[i].pos.y) < HITBOX_SIZE) {
            return true; 
        }
    }
    return false;
}
