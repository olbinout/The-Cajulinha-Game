#include "defs.h"
#include "configurar_jogador.h"
#include "desenhar_tela_ranking.h"
#include "salvar_recorde_no_arquivo.h"
#include "desenhar_tela_instrucoes.h"
#include "ordenar_recordes.h"
#include "buscar_se_jogador_esta_no_ranking.h"
#include "carregar_ranking.h"
#include "movimentar_com_deslize.h"
#include "eh_parede.h"
#include "parar_musica.h"
#include "tocar_musica_selecionada.h"
#include "liberar_memoria.h"
#include "verificar_colisao.h"
#include "alocar_mapa_dinamico.h"
#include "spawnar_item_arma.h"
#include "carregar_mapa.h"
#include "colide_com_outros_inimigos.h"
#include "atirar.h"

// ESTRUTURA DE DADOS: FILA (QUEUE) DINÂMICA

typedef struct No {
    int id_mapa;       
    struct No *prox;   
} No;

typedef struct Fila {
    No *inicio;
    No *fim;
} Fila;

void inicializar_fila(Fila *f) {
    f->inicio = NULL;
    f->fim = NULL;
}

void enfileirar(Fila *f, int valor) {
    No *novo = (No*) malloc(sizeof(No));
    novo->id_mapa = valor;
    novo->prox = NULL;

    if (f->fim == NULL) {
        f->inicio = novo;
        f->fim = novo;
    } else {
        f->fim->prox = novo;
        f->fim = novo;
    }
}

int desenfileirar(Fila *f) {
    if (f->inicio == NULL) return -1; 

    No *temp = f->inicio;
    int valor = temp->id_mapa;

    f->inicio = temp->prox;
    
    if (f->inicio == NULL) {
        f->fim = NULL;
    }

    free(temp); 
    return valor;
}

void limpar_fila(Fila *f) {
    while (f->inicio != NULL) {
        desenfileirar(f);
    }
}

// ==========================================

const char *nomes_musica_display[QTD_OPCOES_MUSICA] = {
    "Sem música",
    "Spektrem - Shine",
    "Cartoon - On & On",
    "DEAF KEV - Invincible",
    "Syn Cole - Feel Good",
    "Different Heaven - My Heart",
    "Jo Cohen & Sex Whales - We Are",
    "Electro Light - Symbolism",
    "Cartoon - Why We Lose",
    "Julius Dreisig - Invisible",
    "TULE & Chris Linton - Fearless",
    "unknown artist 01",
    "unknown artist 02"
};

enum Estado {
    MENU,
    MENU_INSTRUCOES,
    TELA_RANKING, 
    JOGANDO,
    GAME_OVER,
    VITORIA
};

int vidas = 0;
int **mapa_atual = NULL;
Inimigo *inimigos = NULL; 
Jogador player;
Projetil tiros[MAX_TIROS]; 
int total_pontos_no_mapa = 0;

// --- Globais ---
bool item_ativo_no_mapa = false;
bool mostrando_instrucoes = false; 
Vetor2D pos_item_arma;

double timer_duracao_arma = 0; 
double timer_respawn_arma = 0;
double tempo_de_jogo = 0; 

// --- Globais de Configuração ---
int num_inimigos_selecionados = 5; 
int indice_musica_selecionada = 0; 
char buffer_nome_jogador[30] = ""; 
bool exibir_aviso_limite = false; 
bool recorde_ja_salvo = false; 

// --- LISTA DE RECORDES ---
Recorde lista_recordes[MAX_RECORDES_LIDOS];
int qtd_recordes_carregados = 0;
int filtro_ranking_inimigos = 0;

// --- Globais de Áudio ---
ALLEGRO_SAMPLE *musica_fundo = NULL;
ALLEGRO_SAMPLE_INSTANCE *instancia_musica = NULL;

// --- NOVO: Variáveis para o Modo Infinito ---
bool modo_infinito_ativo = false;
Fila fila_fases; 

int main() {
    srand(time(NULL)); 
    if (!al_init()) return -1;
    al_init_image_addon();
    al_install_keyboard();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    if(!al_install_audio()){
        printf("Falha ao inicializar áudio.\n");
        return -1;
    }
    if(!al_init_acodec_addon()){
        printf("Falha ao inicializar codecs de áudio.\n");
        return -1;
    }
    al_reserve_samples(1); 

    inicializar_fila(&fila_fases);

    player.nome = NULL;

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);
    ALLEGRO_DISPLAY *display = al_create_display(SCREEN_W, SCREEN_H);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    ALLEGRO_FONT *fonte = al_create_builtin_font(); 

    // Jogador (Padrão)
    ALLEGRO_BITMAP *img_arma_direita = al_load_bitmap("imagens/arma_direita.png");
    ALLEGRO_BITMAP *img_arma_esquerda = al_load_bitmap("imagens/arma_esquerda.png");
    ALLEGRO_BITMAP *img_player_descendo = al_load_bitmap("imagens/descendo.png");
    ALLEGRO_BITMAP *img_player_direita = al_load_bitmap("imagens/indo_direita.png");
    ALLEGRO_BITMAP *img_player_esquerda = al_load_bitmap("imagens/indo_esquerda.png");
    ALLEGRO_BITMAP *img_player_parado = al_load_bitmap("imagens/parado.png");
    ALLEGRO_BITMAP *img_player_subindo = al_load_bitmap("imagens/subindo.png");

    // Jogador (Cangaceiro - Deserto) -> NOVO
    ALLEGRO_BITMAP *img_cap_normal = al_load_bitmap("imagens/cap_normal.png");
    ALLEGRO_BITMAP *img_cap_left = al_load_bitmap("imagens/cap_left.png");
    ALLEGRO_BITMAP *img_cap_right = al_load_bitmap("imagens/cap_right.png");
    ALLEGRO_BITMAP *img_cap_up = al_load_bitmap("imagens/cap_up.png");
    ALLEGRO_BITMAP *img_cap_down = al_load_bitmap("imagens/cap_down.png");
    ALLEGRO_BITMAP *img_cap_gun_right = al_load_bitmap("imagens/cap_gun_right.png");
    ALLEGRO_BITMAP *img_cap_gun_left = al_load_bitmap("imagens/cap_gun_left.png");

    // Projéteis e Itens
    ALLEGRO_BITMAP *img_tiro_baixo = al_load_bitmap("imagens/pa_tiro_baixo.png");
    ALLEGRO_BITMAP *img_tiro_cima = al_load_bitmap("imagens/pa_tiro_cima.png");
    ALLEGRO_BITMAP *img_tiro_direita = al_load_bitmap("imagens/pa_tiro.png");
    ALLEGRO_BITMAP *img_tiro_esquerda = al_load_bitmap("imagens/pa_tiro_esquerda.png");
    ALLEGRO_BITMAP *img_cajuina = al_load_bitmap("imagens/cajuina.png");
    ALLEGRO_BITMAP *img_cashew = al_load_bitmap("imagens/cashew.png");
    ALLEGRO_BITMAP *img_castanha = al_load_bitmap("imagens/castanha.png");
    ALLEGRO_BITMAP *img_item_arma = al_load_bitmap("imagens/anti_janelas.png");

    // Inimigos e Interface
    ALLEGRO_BITMAP *img_inimigo = al_load_bitmap("imagens/inimigo.png");
    ALLEGRO_BITMAP *img_windows10 = al_load_bitmap("imagens/windows_10.png");
    ALLEGRO_BITMAP *img_windows11 = al_load_bitmap("imagens/windows_11.png");
    ALLEGRO_BITMAP *img_busted = al_load_bitmap("imagens/busted.png");

    // Mapas
    ALLEGRO_BITMAP *img_block8 = al_load_bitmap("imagens/block_8.png");
    ALLEGRO_BITMAP *img_mapa_deserto = al_load_bitmap("imagens/mapa_de_deserto.png");
    ALLEGRO_BITMAP *img_block5 = al_load_bitmap("imagens/block_5.png");
    ALLEGRO_BITMAP *img_block6 = al_load_bitmap("imagens/block_6.png");
    ALLEGRO_BITMAP *img_block7 = al_load_bitmap("imagens/block_7.png");
    ALLEGRO_BITMAP *img_mapa_floresta = al_load_bitmap("imagens/mapa_de_floresta.png");
    ALLEGRO_BITMAP *img_block3 = al_load_bitmap("imagens/block_3.png");
    ALLEGRO_BITMAP *img_mapa_de_gelo = al_load_bitmap("imagens/mapa_de_gelo.png");

    // Verificação de Erro Atualizada (incluindo os novos do cangaceiro)
    if (!img_player_direita || !img_arma_direita || !img_item_arma || !img_tiro_direita || !img_tiro_esquerda || 
        !img_tiro_cima || !img_tiro_baixo || !img_block3 || !img_castanha || !img_busted || !img_windows11 || !img_windows10 ||
        !img_mapa_de_gelo || !img_mapa_floresta || !img_block5 || !img_block6 || !img_block7 ||
        !img_mapa_deserto || !img_block8 || !img_cajuina ||
        !img_cap_normal || !img_cap_left || !img_cap_right || !img_cap_up || !img_cap_down || !img_cap_gun_right || !img_cap_gun_left) { 
        printf("Erro: Faltam imagens.\n");
        return -1;
    }

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    al_start_timer(timer);
      
    int estado_atual = MENU;
    bool running = true;
    bool redraw = true;

    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) running = false;
        
        else if (ev.type == ALLEGRO_EVENT_KEY_CHAR) {
            if (estado_atual == MENU) {
                if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    modo_infinito_ativo = false; 
                    configurar_jogador(&player, buffer_nome_jogador);
                    carregar_mapa(num_inimigos_selecionados);
                    tocar_musica_selecionada(); 
                    estado_atual = JOGANDO;
                    mostrando_instrucoes = false;
                }
                else if (ev.keyboard.keycode == ALLEGRO_KEY_F3) {
                    if (strlen(buffer_nome_jogador) > 0) {
                        modo_infinito_ativo = true;
                        vidas = 3;

                        limpar_fila(&fila_fases);
                        inicializar_fila(&fila_fases);
                        enfileirar(&fila_fases, 0); 
                        enfileirar(&fila_fases, 1);
                        enfileirar(&fila_fases, 2); 

                        indice_mapa_selecionado = desenfileirar(&fila_fases);

                        configurar_jogador(&player, buffer_nome_jogador);
                        carregar_mapa(num_inimigos_selecionados);
                        tocar_musica_selecionada();
                        estado_atual = JOGANDO;
                        mostrando_instrucoes = false;
                        printf("Modo Infinito Iniciado (F3)!\n");

                    } else {
                        exibir_aviso_limite = true; 
                    }
                }
                else if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    running = false;
                }
                else if (ev.keyboard.keycode == ALLEGRO_KEY_TAB) { 
                    carregar_ranking();
                    estado_atual = TELA_RANKING;
                }
                else if (ev.keyboard.keycode == ALLEGRO_KEY_F5) {
                    indice_mapa_selecionado++;
                    if (indice_mapa_selecionado >= QTD_MAPAS) {
                        indice_mapa_selecionado = 0;
                    }
                }
                else if (ev.keyboard.keycode == ALLEGRO_KEY_UP) {
                    if (num_inimigos_selecionados < MAX_INIMIGOS_LIMIT) {
                        num_inimigos_selecionados++; exibir_aviso_limite = false;
                    } else exibir_aviso_limite = true;
                }
                else if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                    if (num_inimigos_selecionados > 1) {
                        num_inimigos_selecionados--; exibir_aviso_limite = false;
                    }
                }
                else if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                    indice_musica_selecionada++;
                    if (indice_musica_selecionada >= QTD_OPCOES_MUSICA) indice_musica_selecionada = 0;
                }
                else if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) {
                    indice_musica_selecionada--;
                    if (indice_musica_selecionada < 0) indice_musica_selecionada = QTD_OPCOES_MUSICA - 1;
                }
                else if (ev.keyboard.unichar == '*') {
                    estado_atual = MENU_INSTRUCOES;
                }
                else {
                    char c = ev.keyboard.unichar;
                    if (c >= 32 && c <= 126 && strlen(buffer_nome_jogador) < 15) {
                        if (c != '*' && c != '/' && c != '/' && 
                            ev.keyboard.keycode != ALLEGRO_KEY_LEFT && ev.keyboard.keycode != ALLEGRO_KEY_RIGHT && 
                            ev.keyboard.keycode != ALLEGRO_KEY_UP && ev.keyboard.keycode != ALLEGRO_KEY_DOWN &&
                            ev.keyboard.keycode != ALLEGRO_KEY_F5 && ev.keyboard.keycode != ALLEGRO_KEY_F3) { 
                            int len = strlen(buffer_nome_jogador);
                            buffer_nome_jogador[len] = c;
                            buffer_nome_jogador[len+1] = '\0';
                        }
                    }
                    else if (ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
                        int len = strlen(buffer_nome_jogador);
                        if (len > 0) buffer_nome_jogador[len-1] = '\0';
                    }
                }
            }
            else if (estado_atual == MENU_INSTRUCOES) {
                if (ev.keyboard.unichar == '*' || 
                    ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE || 
                    ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    estado_atual = MENU; 
                }
            }
            else if (estado_atual == TELA_RANKING) {
                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    estado_atual = MENU;
                    filtro_ranking_inimigos = 0; 
                }
                else if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                    filtro_ranking_inimigos++;
                    if (filtro_ranking_inimigos > MAX_INIMIGOS_LIMIT) filtro_ranking_inimigos = 0; 
                }
                else if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) {
                    filtro_ranking_inimigos--;
                    if (filtro_ranking_inimigos < 0) filtro_ranking_inimigos = MAX_INIMIGOS_LIMIT; 
                }
            }
            else if (estado_atual == JOGANDO) {
                if (ev.keyboard.unichar == '*') {
                    mostrando_instrucoes = !mostrando_instrucoes;
                }

                if (mostrando_instrucoes) {
                    if (ev.keyboard.keycode == ALLEGRO_KEY_M) { 
                        parar_musica();
                        estado_atual = MENU;
                        mostrando_instrucoes = false;
                        if (modo_infinito_ativo) limpar_fila(&fila_fases);
                    }
                }
            }
        }

        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN && estado_atual == JOGANDO) {
            if (!mostrando_instrucoes) {
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_UP:
                    case ALLEGRO_KEY_W:
                        player.dir.y = -1; player.dir.x = 0; break;
                    
                    case ALLEGRO_KEY_DOWN:
                    case ALLEGRO_KEY_S:
                        player.dir.y = 1;  player.dir.x = 0; break;
                    
                    case ALLEGRO_KEY_LEFT:
                    case ALLEGRO_KEY_A:
                        player.dir.x = -1; player.dir.y = 0; break;
                    
                    case ALLEGRO_KEY_RIGHT:
                    case ALLEGRO_KEY_D:
                        player.dir.x = 1;  player.dir.y = 0; break;

                    case ALLEGRO_KEY_SPACE:     atirar(); break; 
                    case ALLEGRO_KEY_ESCAPE: 
                        parar_musica(); 
                        estado_atual = MENU; 
                        if (modo_infinito_ativo) limpar_fila(&fila_fases);
                        break;
                }
            }
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN && (estado_atual == GAME_OVER || estado_atual == VITORIA)) {
            if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                parar_musica(); 
                estado_atual = MENU;
                limpar_fila(&fila_fases);
            }
            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) running = false;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_UP && estado_atual == JOGANDO && !mostrando_instrucoes) {
            if (((ev.keyboard.keycode == ALLEGRO_KEY_UP || ev.keyboard.keycode == ALLEGRO_KEY_W) && player.dir.y == -1) ||
                ((ev.keyboard.keycode == ALLEGRO_KEY_DOWN || ev.keyboard.keycode == ALLEGRO_KEY_S) && player.dir.y == 1)) {
                player.dir.y = 0;
            }
            if (((ev.keyboard.keycode == ALLEGRO_KEY_LEFT || ev.keyboard.keycode == ALLEGRO_KEY_A) && player.dir.x == -1) ||
                ((ev.keyboard.keycode == ALLEGRO_KEY_RIGHT || ev.keyboard.keycode == ALLEGRO_KEY_D) && player.dir.x == 1)) {
                player.dir.x = 0;
            }
        }

        // --- LÓGICA DO JOGO ---
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            if (estado_atual == JOGANDO && !mostrando_instrucoes) {
                
                tempo_de_jogo += 1.0 / FPS; 
                char texto_vidas[20] = "";
                if (modo_infinito_ativo) {
                    sprintf(texto_vidas, " | VIDAS: %d", vidas);
                }   
                if (player.tem_arma) {
                    timer_duracao_arma -= 1.0 / FPS;

                    if (timer_duracao_arma <= 0) {
                        player.tem_arma = false;
                        timer_respawn_arma = 15.0; 
                    }
                } else if (!item_ativo_no_mapa) {
                    if (timer_respawn_arma > 0) {
                        timer_respawn_arma -= 1.0 / FPS;
                    } else {
                        spawnar_item_arma(); 
                    }
                }

                if (item_ativo_no_mapa) {
                    if (verificar_colisao(player.pos.x, player.pos.y, HITBOX_SIZE, HITBOX_SIZE, 
                                          pos_item_arma.x, pos_item_arma.y, TILE_SIZE, TILE_SIZE)) {
                        item_ativo_no_mapa = false;
                        player.tem_arma = true;
                        
                        player.municao = 11; 
                        timer_duracao_arma = 30.0; 
                    }
                }

                if (player.dir.x != 0 || player.dir.y != 0) {
                    movimentar_com_deslize(&player.pos.x, &player.pos.y, player.dir.x, player.dir.y, player.velocidade);
                }
                if (player.dir.x > 0) player.ultimo_dir_x = 1;
                if (player.dir.x < 0) player.ultimo_dir_x = -1;

                int p_col = (int)(player.pos.x + HITBOX_SIZE/2) / TILE_SIZE;
                int p_row = (int)(player.pos.y + HITBOX_SIZE/2) / TILE_SIZE;
                  
                int bloco = mapa_atual[p_row][p_col];
                if (bloco == 3 || bloco == 4 || bloco == 9) {
                    if (bloco == 4) player.pontos += 20; 
                    else if (bloco == 9) player.pontos += 50; 
                    else player.pontos += 10; 

                    mapa_atual[p_row][p_col] = 0; 
                    total_pontos_no_mapa--;
                }

                if (total_pontos_no_mapa <= 0) {
                    
                    if (modo_infinito_ativo) {
                        int proximo_mapa = desenfileirar(&fila_fases);

                        if (proximo_mapa != -1) {
                            printf("Mapa concluido! Indo para o proximo mapa da fila: %d\n", proximo_mapa);
                            
                            indice_mapa_selecionado = proximo_mapa;
                            carregar_mapa(num_inimigos_selecionados);
                            
                            player.pos.x = TILE_SIZE * 1;
                            player.pos.y = TILE_SIZE * 1;
                            player.dir.x = 0; player.dir.y = 0;
                            
                            for(int i=0; i < num_inimigos_selecionados; i++) {
                                inimigos[i].ativo = false;
                                inimigos[i].timer_respawn = 2.0; 
                            }
                        } 
                        else {
                            printf("Modo Infinito completado!\n");
                            parar_musica(); 
                            salvar_recorde_no_arquivo(); 
                            estado_atual = VITORIA;
                        }
                    } 
                    else {
                        parar_musica(); 
                        salvar_recorde_no_arquivo(); 
                        estado_atual = VITORIA;
                    }
                }

                for(int t=0; t<MAX_TIROS; t++) {
                    if(tiros[t].ativo) {
                        tiros[t].pos.x += tiros[t].dir.x * VEL_TIRO;
                        tiros[t].pos.y += tiros[t].dir.y * VEL_TIRO;

                        if (eh_parede(tiros[t].pos.x + 5, tiros[t].pos.y + 5)) {
                            tiros[t].ativo = false;
                            continue;
                        }

                        for(int i=0; i < num_inimigos_selecionados; i++) {
                            if (inimigos[i].ativo && verificar_colisao(tiros[t].pos.x, tiros[t].pos.y, 10, 10, 
                                                                       inimigos[i].pos.x, inimigos[i].pos.y, HITBOX_SIZE, HITBOX_SIZE)) {
                                tiros[t].ativo = false;
                                inimigos[i].ativo = false;       
                                inimigos[i].timer_respawn = 4.5; 
                            }
                        }
                    }
                }

                for (int i = 0; i < num_inimigos_selecionados; i++) {
                    if (!inimigos[i].ativo) {
                        inimigos[i].timer_respawn -= 1.0 / FPS;
                        if (inimigos[i].timer_respawn <= 0) {
                            float spawn_x = TILE_SIZE * 12;
                            float spawn_y = TILE_SIZE * 9;
                            if (!colide_com_outros_inimigos(i, spawn_x, spawn_y)) {
                                inimigos[i].ativo = true;
                                inimigos[i].pos.x = spawn_x;
                                inimigos[i].pos.y = spawn_y;
                            }
                        }
                        continue;
                    }

                    float dx = player.pos.x - inimigos[i].pos.x;
                    float dy = player.pos.y - inimigos[i].pos.y;
                    float dir_x = 0, dir_y = 0;

                    if (fabs(dx) > fabs(dy)) {
                        dir_x = (dx > 0) ? 1 : -1;
                    } else {
                        dir_y = (dy > 0) ? 1 : -1;
                    }

                    float x_antes = inimigos[i].pos.x;
                    float y_antes = inimigos[i].pos.y;

                    movimentar_com_deslize(&inimigos[i].pos.x, &inimigos[i].pos.y, dir_x, dir_y, inimigos[i].velocidade);

                    if (inimigos[i].pos.x == x_antes && inimigos[i].pos.y == y_antes) {
                        if (dir_x != 0) { 
                            dir_x = 0; 
                            dir_y = (dy > 0) ? 1 : -1;
                        } else { 
                            dir_y = 0;
                            dir_x = (dx > 0) ? 1 : -1;
                        }
                        movimentar_com_deslize(&inimigos[i].pos.x, &inimigos[i].pos.y, dir_x, dir_y, inimigos[i].velocidade);
                    }

                    if (colide_com_outros_inimigos(i, inimigos[i].pos.x, inimigos[i].pos.y)) {
                        inimigos[i].pos.x = x_antes;
                        inimigos[i].pos.y = y_antes;
                    }

                    if (fabs(player.pos.x - inimigos[i].pos.x) < 20 && 
                        fabs(player.pos.y - inimigos[i].pos.y) < 20) {
                        
                        bool game_over = true; // Assume que perdeu, a menos que tenha vidas extras

                        // LÓGICA DE VIDAS DO MODO INFINITO
                        if (modo_infinito_ativo) {
                            vidas--; // Perde uma vida
                            
                            if (vidas > 0) {
                                game_over = false; // Ainda tem vidas, não dá Game Over
                                printf("Vida perdida! Restantes: %d\n", vidas);

                                // 1. Reseta Posição do Jogador para o início (Tile 1,1)
                                player.pos.x = TILE_SIZE * 1;
                                player.pos.y = TILE_SIZE * 1;
                                player.dir.x = 0; 
                                player.dir.y = 0;

                                // 2. Reseta TODOS os inimigos (manda para o spawn e pausa eles)
                                for(int k=0; k < num_inimigos_selecionados; k++) {
                                    inimigos[k].ativo = false;
                                    inimigos[k].timer_respawn = 2.0; // Tempo para o jogador respirar
                                    // Move eles para longe visualmente para não matar o player instantaneamente
                                    inimigos[k].pos.x = TILE_SIZE * 12; 
                                    inimigos[k].pos.y = TILE_SIZE * 9;
                                }
                            }
                        }

                        // Se for modo normal OU se acabaram as vidas no modo infinito
                        if (game_over) {
                            parar_musica(); 
                            estado_atual = GAME_OVER;

                            if (modo_infinito_ativo) {
                                limpar_fila(&fila_fases);
                                // Mantém a flag true para permitir reiniciar com ENTER se quiser, 
                                // ou pode setar false se quiser forçar menu.
                            }
                        }
                    }
                }
            }
            redraw = true;
        }

        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;
            al_clear_to_color(al_map_rgb(0, 0, 0));

            if (estado_atual == MENU) {
                 al_draw_filled_rectangle(0, 0, SCREEN_W, SCREEN_H, al_map_rgb(20, 20, 20));
                 
                 al_draw_text(fonte, al_map_rgb(255, 255, 0), SCREEN_W/2, SCREEN_H/2 - 80, ALLEGRO_ALIGN_CENTRE, "--- CONFIGURAÇÃO ---");
                 
                 al_draw_textf(fonte, al_map_rgb(255, 255, 255), SCREEN_W/2, SCREEN_H/2 - 40, ALLEGRO_ALIGN_CENTRE, 
                     "Digite seu nickname: %s_", buffer_nome_jogador);

                 char nome_mapa[20];
                 if (indice_mapa_selecionado == 0) strcpy(nome_mapa, "GELO");
                 else if (indice_mapa_selecionado == 1) strcpy(nome_mapa, "FLORESTA");
                 else strcpy(nome_mapa, "DESERTO"); 

                 al_draw_textf(fonte, al_map_rgb(255, 100, 255), SCREEN_W/2, SCREEN_H/2 - 10, ALLEGRO_ALIGN_CENTRE, 
                     "Mapa (Tecla F5): < %s >", nome_mapa);

                 al_draw_textf(fonte, al_map_rgb(255, 200, 100), SCREEN_W/2, SCREEN_H/2 + 20, ALLEGRO_ALIGN_CENTRE, 
                     "Músicas (Setas Esq/Dir): < %s >", nomes_musica_display[indice_musica_selecionada]);

                 al_draw_textf(fonte, al_map_rgb(100, 255, 100), SCREEN_W/2, SCREEN_H/2 + 60, ALLEGRO_ALIGN_CENTRE, 
                     "Quantidade de inimigos (Setas Cima/Baixo): < %d >", num_inimigos_selecionados);
                 
                 if (exibir_aviso_limite) {
                     al_draw_text(fonte, al_map_rgb(255, 50, 50), SCREEN_W/2, SCREEN_H/2 + 80, ALLEGRO_ALIGN_CENTRE, 
                         "AVISO: Máximo de 11 inimigos ou digite um nome!");
                 }
                 
                 al_draw_text(fonte, al_map_rgb(200, 200, 200), SCREEN_W/2, SCREEN_H/2 + 110, ALLEGRO_ALIGN_CENTRE, "PRESSIONE [ENTER] PARA INICIAR (Normal)");
                 
                 al_draw_text(fonte, al_map_rgb(255, 215, 0), SCREEN_W/2, SCREEN_H/2 + 130, ALLEGRO_ALIGN_CENTRE, "PRESSIONE [F3] PARA MODO INFINITO (Gelo->Flor->Des)");

                 al_draw_text(fonte, al_map_rgb(255, 215, 0), SCREEN_W/2, SCREEN_H/2 + 150, ALLEGRO_ALIGN_CENTRE, "(Insira seu nome para que possa jogar o modo infinito!)");
                 
                 al_draw_text(fonte, al_map_rgb(150, 200, 255), SCREEN_W/2, SCREEN_H/2 + 200, ALLEGRO_ALIGN_CENTRE, "[*] Instruções | [TAB] Ranking | [ESC] Sair");
            }
            else if (estado_atual == MENU_INSTRUCOES) {
                al_draw_filled_rectangle(0, 0, SCREEN_W, SCREEN_H, al_map_rgb(20, 20, 20));
                desenhar_tela_instrucoes(fonte, SCREEN_W, SCREEN_H);
            }
            else if (estado_atual == TELA_RANKING) {
                desenhar_tela_ranking(fonte, SCREEN_W, SCREEN_H);
            }
            else if (estado_atual == JOGANDO) {
                // --- DESENHAR BACKGROUND ---
                if (indice_mapa_selecionado == 0) {
                    al_draw_scaled_bitmap(img_mapa_de_gelo, 0,0, al_get_bitmap_width(img_mapa_de_gelo), al_get_bitmap_height(img_mapa_de_gelo), 0, 0, SCREEN_W, SCREEN_H, 0);
                } else if (indice_mapa_selecionado == 1) {
                    al_draw_scaled_bitmap(img_mapa_floresta, 0,0, al_get_bitmap_width(img_mapa_floresta), al_get_bitmap_height(img_mapa_floresta), 0, 0, SCREEN_W, SCREEN_H, 0);
                } else {
                    al_draw_scaled_bitmap(img_mapa_deserto, 0,0, al_get_bitmap_width(img_mapa_deserto), al_get_bitmap_height(img_mapa_deserto), 0, 0, SCREEN_W, SCREEN_H, 0);
                }

                for (int r = 0; r < ROWS; r++) {
                    for (int c = 0; c < COLS; c++) {
                        float px = c * TILE_SIZE;
                        float py = r * TILE_SIZE;
                        
                        int tipo = mapa_atual[r][c];

                        if (tipo == 1 || tipo == 2) {
                             al_draw_scaled_bitmap(img_block3, 0,0, al_get_bitmap_width(img_block3), al_get_bitmap_height(img_block3), px, py, TILE_SIZE, TILE_SIZE, 0);
                        }
                        else if (tipo == 5) {
                             al_draw_scaled_bitmap(img_block5, 0,0, al_get_bitmap_width(img_block5), al_get_bitmap_height(img_block5), px, py, TILE_SIZE, TILE_SIZE, 0);
                        }
                        else if (tipo == 6) { 
                             al_draw_scaled_bitmap(img_block6, 0,0, al_get_bitmap_width(img_block6), al_get_bitmap_height(img_block6), px, py, TILE_SIZE, TILE_SIZE, 0);
                        }
                        else if (tipo == 7) { 
                             al_draw_scaled_bitmap(img_block7, 0,0, al_get_bitmap_width(img_block7), al_get_bitmap_height(img_block7), px, py, TILE_SIZE, TILE_SIZE, 0);
                        }
                        else if (tipo == 8) { 
                             al_draw_scaled_bitmap(img_block8, 0,0, al_get_bitmap_width(img_block8), al_get_bitmap_height(img_block8), px, py, TILE_SIZE, TILE_SIZE, 0);
                        }
                        else if (tipo == 3) {
                            al_draw_scaled_bitmap(img_cashew, 0,0, al_get_bitmap_width(img_cashew), al_get_bitmap_height(img_cashew), px, py, TILE_SIZE, TILE_SIZE, 0);
                        }
                        else if (tipo == 4) {
                            al_draw_scaled_bitmap(img_castanha, 0,0, al_get_bitmap_width(img_castanha), al_get_bitmap_height(img_castanha), px, py, TILE_SIZE, TILE_SIZE, 0);
                        }
                        else if (tipo == 9) {
                            al_draw_scaled_bitmap(img_cajuina, 0,0, al_get_bitmap_width(img_cajuina), al_get_bitmap_height(img_cajuina), px, py, TILE_SIZE, TILE_SIZE, 0);
                        }
                    }
                }
                
                if (item_ativo_no_mapa) {
                      al_draw_scaled_bitmap(img_item_arma, 0,0, al_get_bitmap_width(img_item_arma), al_get_bitmap_height(img_item_arma), 
                                               pos_item_arma.x, pos_item_arma.y, TILE_SIZE, TILE_SIZE, 0);
                }

                for(int t=0; t<MAX_TIROS; t++) {
                    if(tiros[t].ativo) {
                        ALLEGRO_BITMAP *sprite_tiro_atual = img_tiro_direita; 
                        if (tiros[t].dir.x == -1) sprite_tiro_atual = img_tiro_esquerda;
                        else if (tiros[t].dir.y == -1) sprite_tiro_atual = img_tiro_cima;
                        else if (tiros[t].dir.y == 1) sprite_tiro_atual = img_tiro_baixo;

                        al_draw_scaled_bitmap(sprite_tiro_atual, 0,0, 
                            al_get_bitmap_width(sprite_tiro_atual), 
                            al_get_bitmap_height(sprite_tiro_atual), 
                            tiros[t].pos.x, tiros[t].pos.y, 20, 20, 0); 
                    }
                }

                // ====================================================
                // LÓGICA DE DESENHO DO JOGADOR (COM CANGACEIRO NO DESERTO)
                // ====================================================
                ALLEGRO_BITMAP *current_player_image;
                
                // SE ESTIVER NO MAPA DO DESERTO (ÍNDICE 2)
                if (indice_mapa_selecionado == 2) {
                    if (player.tem_arma) {
                         if (player.dir.x == -1) current_player_image = img_cap_gun_left;
                         else if (player.dir.x == 1) current_player_image = img_cap_gun_right;
                         // Se estiver parado ou movendo verticalmente com arma, usa o direito
                         else current_player_image = img_cap_gun_right;
                    } else {
                         current_player_image = img_cap_normal;
                         if (player.dir.x == 1) current_player_image = img_cap_right;
                         else if (player.dir.x == -1) current_player_image = img_cap_left;
                         else if (player.dir.y == -1) current_player_image = img_cap_up;
                         else if (player.dir.y == 1) current_player_image = img_cap_down;
                    }
                } 
                // SE ESTIVER NOS OUTROS MAPAS (GELO OU FLORESTA)
                else {
                    if (player.tem_arma) {
                        if (player.dir.x == -1) current_player_image = img_arma_esquerda;
                        else if (player.dir.x == 1) current_player_image = img_arma_direita;
                        else current_player_image = (player.ultimo_dir_x == -1) ? img_arma_esquerda : img_arma_direita;
                    } else {
                        current_player_image = img_player_parado;
                        if (player.dir.x == 1) current_player_image = img_player_direita;
                        else if (player.dir.x == -1) current_player_image = img_player_esquerda;
                        else if (player.dir.y == -1) current_player_image = img_player_subindo;
                        else if (player.dir.y == 1) current_player_image = img_player_descendo;
                    }
                }

                al_draw_scaled_bitmap(current_player_image, 
                    0, 0, al_get_bitmap_width(current_player_image), al_get_bitmap_height(current_player_image),
                    player.pos.x + OFFSET_DRAW, player.pos.y + OFFSET_DRAW,
                    DRAW_SIZE, DRAW_SIZE, 0);

                for(int i=0; i < num_inimigos_selecionados; i++) 
                    if(inimigos[i].ativo) {
                        ALLEGRO_BITMAP *sprite_inimigo_atual = img_inimigo;
                        if (i == 9) sprite_inimigo_atual = img_windows10;
                        else if (i == 10) sprite_inimigo_atual = img_windows11;

                        al_draw_scaled_bitmap(sprite_inimigo_atual, 0,0, 
                            al_get_bitmap_width(sprite_inimigo_atual), 
                            al_get_bitmap_height(sprite_inimigo_atual), 
                            inimigos[i].pos.x, inimigos[i].pos.y, TILE_SIZE, TILE_SIZE, 0);
                    }
                
                al_draw_filled_rectangle(0, 0, SCREEN_W, TILE_SIZE, al_map_rgb(0, 0, 100));
                al_draw_line(0, TILE_SIZE, SCREEN_W, TILE_SIZE, al_map_rgb(255, 255, 255), 2);

                char status_arma[50];
                if (player.tem_arma) {
                    sprintf(status_arma, "%d BALAS | %ds", player.municao, (int)timer_duracao_arma);
                } else {
                    sprintf(status_arma, "NÃO");
                }

                int minutos = (int)tempo_de_jogo / 60;
                int segundos = (int)tempo_de_jogo % 60;

                // --- NOVO CÓDIGO: Preparar texto das vidas ---
                char texto_vidas[30] = ""; // Começa vazio
                if (modo_infinito_ativo) {
                    // Se estiver no modo infinito, escreve a quantidade de vidas
                    sprintf(texto_vidas, " | VIDAS: %d", vidas);
                }
                // ---------------------------------------------

                // Note o %s extra depois de status_arma e a variável texto_vidas no final
                al_draw_textf(fonte, al_map_rgb(255, 255, 255), 10, 15, 0, 
                    "JOGADOR: %s | PONTOS: %03d | TEMPO: %02d:%02d | ARMA: %s%s | [*] AJUDA", 
                    player.nome, 
                    player.pontos,
                    minutos, segundos, 
                    status_arma,
                    texto_vidas);

                if (mostrando_instrucoes) {
                    desenhar_tela_instrucoes(fonte, SCREEN_W, SCREEN_H);
                }
            }
            else if (estado_atual == GAME_OVER) {
                al_clear_to_color(al_map_rgb(50, 0, 0));
                al_draw_text(fonte, al_map_rgb(255, 255, 255), SCREEN_W/2, SCREEN_H/2, ALLEGRO_ALIGN_CENTRE, "GAME OVER - press [ENTER] to menu");
            }
            else if (estado_atual == VITORIA) {
                al_clear_to_color(al_map_rgb(0, 0, 0));

                if (img_busted) {
                    al_draw_scaled_bitmap(img_busted,
                        0, 0, al_get_bitmap_width(img_busted), al_get_bitmap_height(img_busted), 
                        0, 0, SCREEN_W, SCREEN_H, 
                        0);
                }

                int min_final = (int)tempo_de_jogo / 60;
                int seg_final = (int)tempo_de_jogo % 60;
                al_draw_textf(fonte, al_map_rgb(255, 255, 0), SCREEN_W/2, SCREEN_H - 80, ALLEGRO_ALIGN_CENTRE, 
                    "TEMPO TOTAL: %02d:%02d", min_final, seg_final);

                al_draw_text(fonte, al_map_rgb(0, 255, 0), SCREEN_W/2, SCREEN_H - 110, ALLEGRO_ALIGN_CENTRE, "NOVO RECORDE SALVO!");

                al_draw_text(fonte, al_map_rgb(255, 255, 255), SCREEN_W/2, SCREEN_H - 50, ALLEGRO_ALIGN_CENTRE, "VITÓRIA - press [ENTER] to menu");
            }
            al_flip_display();
        }
    }
    
    limpar_fila(&fila_fases);
    
    liberar_memoria();

    al_destroy_bitmap(img_player_direita);
    al_destroy_bitmap(img_player_esquerda);
    al_destroy_bitmap(img_player_subindo);
    al_destroy_bitmap(img_player_descendo);
    al_destroy_bitmap(img_player_parado);
    al_destroy_bitmap(img_arma_direita);
    al_destroy_bitmap(img_arma_esquerda);
    al_destroy_bitmap(img_cap_normal);
    al_destroy_bitmap(img_cap_left);
    al_destroy_bitmap(img_cap_right);
    al_destroy_bitmap(img_cap_up);
    al_destroy_bitmap(img_cap_down);
    al_destroy_bitmap(img_cap_gun_right);
    al_destroy_bitmap(img_cap_gun_left);
    al_destroy_bitmap(img_windows10);
    al_destroy_bitmap(img_windows11);
    al_destroy_bitmap(img_busted);
    al_destroy_bitmap(img_item_arma);
    al_destroy_bitmap(img_tiro_direita);
    al_destroy_bitmap(img_tiro_esquerda);
    al_destroy_bitmap(img_tiro_cima);
    al_destroy_bitmap(img_tiro_baixo);
    al_destroy_bitmap(img_inimigo); 
    al_destroy_bitmap(img_block3); al_destroy_bitmap(img_cashew); 
    al_destroy_bitmap(img_castanha); 
    al_destroy_bitmap(img_mapa_de_gelo);
    al_destroy_bitmap(img_mapa_floresta);
    al_destroy_bitmap(img_block5);
    al_destroy_bitmap(img_block6);
    al_destroy_bitmap(img_block7);
    al_destroy_bitmap(img_mapa_deserto);
    al_destroy_bitmap(img_block8);
    al_destroy_bitmap(img_cajuina);
    al_destroy_display(display); al_destroy_timer(timer); al_destroy_event_queue(event_queue); 

    return 0;
}