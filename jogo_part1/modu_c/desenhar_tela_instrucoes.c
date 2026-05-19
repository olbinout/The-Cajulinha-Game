#include "desenhar_tela_instrucoes.h"
#include "defs.h"

void desenhar_tela_instrucoes(ALLEGRO_FONT *fonte, float w, float h) {
    al_draw_filled_rectangle(w/2 - 300, h/2 - 200, w/2 + 300, h/2 + 200, al_map_rgba(0, 0, 0, 240));
    al_draw_rectangle(w/2 - 300, h/2 - 200, w/2 + 300, h/2 + 200, al_map_rgb(255, 255, 255), 3);

    al_draw_text(fonte, al_map_rgb(255, 255, 0), w/2, h/2 - 160, ALLEGRO_ALIGN_CENTRE, "--- INSTRUÇÕES DE JOGO ---");

    int y = h/2 - 100;
    int espaco = 25;
      
    al_draw_text(fonte, al_map_rgb(255, 255, 255), w/2, y, ALLEGRO_ALIGN_CENTRE, "MOVIMENTAÇÃO: W, A, S, D ou SETAS");
    al_draw_text(fonte, al_map_rgb(255, 255, 255), w/2, y + espaco, ALLEGRO_ALIGN_CENTRE, "ATIRAR: Tecla 'P' (Necessário ter munição)");
    
    al_draw_text(fonte, al_map_rgb(255, 255, 255), w/2, y + espaco*2, ALLEGRO_ALIGN_CENTRE, "PAUSA / AJUDA: Tecla '*'");
    
    al_draw_text(fonte, al_map_rgb(255, 255, 255), w/2, y + espaco*3, ALLEGRO_ALIGN_CENTRE, "SAIR: Tecla 'ESC'");

    al_draw_text(fonte, al_map_rgb(100, 200, 255), w/2, y + espaco*5, ALLEGRO_ALIGN_CENTRE, "OBJETIVO:");
    al_draw_text(fonte, al_map_rgb(200, 200, 200), w/2, y + espaco*6, ALLEGRO_ALIGN_CENTRE, "* Colete todos os Cajús e Castanhas para vencer.");
    al_draw_text(fonte, al_map_rgb(200, 200, 200), w/2, y + espaco*7, ALLEGRO_ALIGN_CENTRE, "* E tome algumas cajuínas para se refrescar no deserto.");

    al_draw_text(fonte, al_map_rgb(100, 255, 100), w/2, h/2 + 120, ALLEGRO_ALIGN_CENTRE, "PRESSIONE '*' PARA VOLTAR A JOGAR");
    al_draw_text(fonte, al_map_rgb(255, 100, 100), w/2, h/2 + 150, ALLEGRO_ALIGN_CENTRE, "PRESSIONE 'M' PARA MENU PRINCIPAL");
}

