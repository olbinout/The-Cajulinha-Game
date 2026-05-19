#include "desenhar_tela_ranking.h"
#include "defs.h"

void desenhar_tela_ranking(ALLEGRO_FONT *fonte, float w, float h) {
    // Fundo e Borda
    al_draw_filled_rectangle(50, 50, w - 50, h - 50, al_map_rgba(10, 10, 30, 230));
    al_draw_rectangle(50, 50, w - 50, h - 50, al_map_rgb(255, 215, 0), 3);

    // --- Cabeçalho do Título ---
    al_draw_text(fonte, al_map_rgb(255, 215, 0), w/2, 65, ALLEGRO_ALIGN_CENTRE, "=== HALL DA FAMA ===");

    // --- MOSTRADOR DE FILTRO ---
    char texto_filtro[50];
    if (filtro_ranking_inimigos == 0) {
        sprintf(texto_filtro, "< EXIBINDO: TODOS >");
    } else {
        sprintf(texto_filtro, "< EXIBINDO: %d INIMIGOS >", filtro_ranking_inimigos);
    }
    al_draw_text(fonte, al_map_rgb(0, 255, 0), w/2, 90, ALLEGRO_ALIGN_CENTRE, texto_filtro);
    al_draw_text(fonte, al_map_rgb(150, 150, 150), w/2, 105, ALLEGRO_ALIGN_CENTRE, "(Use as Setas ESQ/DIR para filtrar)");

    
    // --- Cabeçalho da Tabela (Posições Ajustadas para caber o MAPA) ---
    int y_start = 130; 
    
    // Coordenadas X ajustadas:
    // Nome (60) | Pontos (280) | Tempo (400) | Inimigos (520) | Mapa (660) | Música (800)
    
    al_draw_text(fonte, al_map_rgb(100, 255, 255), 60, y_start, 0, "NOME");
    al_draw_text(fonte, al_map_rgb(100, 255, 255), 280, y_start, ALLEGRO_ALIGN_CENTRE, "PONTOS");
    al_draw_text(fonte, al_map_rgb(100, 255, 255), 400, y_start, ALLEGRO_ALIGN_CENTRE, "TEMPO");
    al_draw_text(fonte, al_map_rgb(100, 255, 255), 520, y_start, ALLEGRO_ALIGN_CENTRE, "INIMIGOS");
    al_draw_text(fonte, al_map_rgb(0, 255, 255),   660, y_start, ALLEGRO_ALIGN_CENTRE, "MAPA"); // <--- NOVA COLUNA
    al_draw_text(fonte, al_map_rgb(100, 255, 255), 800, y_start, 0, "MÚSICA");

    al_draw_line(50, y_start + 20, w - 50, y_start + 20, al_map_rgb(255, 255, 255), 1);

    if (qtd_recordes_carregados == 0) {
        al_draw_text(fonte, al_map_rgb(200, 200, 200), w/2, h/2, ALLEGRO_ALIGN_CENTRE, "Nenhum recorde salvo ainda.");
    } else {
        int itens_exibidos_na_tela = 0; 
        
        for (int i = 0; i < qtd_recordes_carregados; i++) {
            
            // Filtro
            if (filtro_ranking_inimigos > 0 && lista_recordes[i].qtd_inimigos != filtro_ranking_inimigos) {
                continue;
            }

            if (itens_exibidos_na_tela >= 15) break;

            int y = y_start + 40 + (itens_exibidos_na_tela * 30);
            
            ALLEGRO_COLOR cor_texto = al_map_rgb(255, 255, 255);
            // Destaca o jogador atual em verde
            if (strlen(buffer_nome_jogador) > 0 && strcmp(lista_recordes[i].nome_jogador, buffer_nome_jogador) == 0) {
                cor_texto = al_map_rgb(0, 255, 0);
            }

            int min = (int)lista_recordes[i].tempo_gasto / 60;
            int seg = (int)lista_recordes[i].tempo_gasto % 60;

            // --- DESENHO DOS DADOS (Com as novas posições) ---
            al_draw_textf(fonte, cor_texto, 60, y, 0, "%d. %s", i+1, lista_recordes[i].nome_jogador);
            al_draw_textf(fonte, cor_texto, 280, y, ALLEGRO_ALIGN_CENTRE, "%d", lista_recordes[i].pontuacao);
            al_draw_textf(fonte, cor_texto, 400, y, ALLEGRO_ALIGN_CENTRE, "%02d:%02d", min, seg);
            al_draw_textf(fonte, cor_texto, 520, y, ALLEGRO_ALIGN_CENTRE, "%d", lista_recordes[i].qtd_inimigos);
            
            // --- Exibe o Mapa ---
            // Usa cor Ciano para destacar ou a cor do jogador se for ele
            ALLEGRO_COLOR cor_mapa = (strcmp(lista_recordes[i].nome_jogador, buffer_nome_jogador) == 0) ? cor_texto : al_map_rgb(0, 200, 255);
            al_draw_textf(fonte, cor_mapa, 660, y, ALLEGRO_ALIGN_CENTRE, "%s", lista_recordes[i].nome_mapa);

            // Trata a música (encurta string se for longa)
            char mus_curta[20];
            strncpy(mus_curta, lista_recordes[i].nome_musica, 14);
            mus_curta[14] = '\0'; // Garante fim da string
            al_draw_textf(fonte, cor_texto, 800, y, 0, "%s...", mus_curta);

            itens_exibidos_na_tela++;
        }

        if (itens_exibidos_na_tela == 0 && qtd_recordes_carregados > 0) {
            al_draw_text(fonte, al_map_rgb(150, 150, 150), w/2, h/2, ALLEGRO_ALIGN_CENTRE, "Nenhum recorde com essa quantidade de inimigos.");
        }
    }

    al_draw_text(fonte, al_map_rgb(255, 100, 100), w/2, h - 40, ALLEGRO_ALIGN_CENTRE, "PRESSIONE [ESC] PARA VOLTAR");
}