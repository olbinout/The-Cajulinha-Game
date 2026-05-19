#include "eh_parede.h"
#include "movimentar_com_deslize.h"
#include "defs.h"

void movimentar_com_deslize(float *x, float *y, float dir_x, float dir_y, float vel) {
    float nova_x = *x + (dir_x * vel);
    float nova_y = *y + (dir_y * vel);

    if (pode_andar(nova_x, nova_y)) {
        *x = nova_x;
        *y = nova_y;
        return;
    }

    float centro_x = *x + HITBOX_SIZE / 2.0;
    float centro_y = *y + HITBOX_SIZE / 2.0;
    int grid_x = (int)(centro_x / TILE_SIZE);
    int grid_y = (int)(centro_y / TILE_SIZE);
    float align_x = grid_x * TILE_SIZE + (TILE_SIZE - HITBOX_SIZE) / 2.0;
    float align_y = grid_y * TILE_SIZE + (TILE_SIZE - HITBOX_SIZE) / 2.0;

    if (dir_y != 0) {
        int proximo_grid_y = (dir_y < 0) ? (int)(((*y) - vel) / TILE_SIZE) : (int)(((*y + HITBOX_SIZE) + vel) / TILE_SIZE);
        if (!eh_parede(centro_x, proximo_grid_y * TILE_SIZE + TILE_SIZE/2)) {
            if (*x < align_x) *x += vel;      
            else if (*x > align_x) *x -= vel; 
            if (fabs(*x - align_x) < vel) *x = align_x;
        }
    }
    else if (dir_x != 0) {
        int proximo_grid_x = (dir_x < 0) ? (int)(((*x) - vel) / TILE_SIZE) : (int)(((*x + HITBOX_SIZE) + vel) / TILE_SIZE);
        if (!eh_parede(proximo_grid_x * TILE_SIZE + TILE_SIZE/2, centro_y)) {
            if (*y < align_y) *y += vel;      
            else if (*y > align_y) *y -= vel; 
            if (fabs(*y - align_y) < vel) *y = align_y;
        }
    }
}
