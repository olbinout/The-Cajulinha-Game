#ifndef DEFS_H
#define DEFS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h> 
#include <allegro5/allegro_acodec.h> 
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> 

#define QTD_MAPAS 3
#define TILE_SIZE 40
#define COLS 25
#define ROWS 18
#define SCREEN_W (COLS * TILE_SIZE)
#define SCREEN_H (ROWS * TILE_SIZE)
#define FPS 60.0
#define MAX_INIMIGOS_LIMIT 11 
#define MAX_TIROS 5
#define VEL_TIRO 8.0
#define HITBOX_SIZE 30      
#define DRAW_SIZE 60.0      
#define OFFSET_DRAW -15     
#define QTD_OPCOES_MUSICA 13 
#define ARQUIVO_RECORDES "ranking.txt"
#define MAX_RECORDES_LIDOS 100 

// Estruturas
typedef struct {
    float x, y;
} Vetor2D;

typedef struct {
    Vetor2D pos;
    Vetor2D dir;
    float velocidade;
    int pontos;
    char *nome;
    bool tem_arma;
    int municao;    
    int ultimo_dir_x; 
} Jogador;

typedef struct {
    Vetor2D pos;
    float velocidade;
    bool ativo; 
    double timer_respawn; 
} Inimigo;

typedef struct {
    Vetor2D pos;
    Vetor2D dir;
    bool ativo;
} Projetil;

typedef struct {
    char nome_jogador[50];
    double tempo_gasto;
    int pontuacao;
    int qtd_inimigos;
    char nome_musica[50];
    char nome_mapa[20];
} Recorde;

extern int **mapa_atual;
extern int indice_mapa_selecionado;
extern int filtro_ranking_inimigos;
extern Recorde lista_recordes[MAX_RECORDES_LIDOS];
extern int qtd_recordes_carregados;
extern char buffer_nome_jogador[30];
extern Jogador player;
extern Inimigo *inimigos;
extern double tempo_de_jogo;
extern int num_inimigos_selecionados;
extern int indice_musica_selecionada;
extern bool recorde_ja_salvo;
extern const char *nomes_musica_display[QTD_OPCOES_MUSICA];
extern ALLEGRO_SAMPLE *musica_fundo;
extern ALLEGRO_SAMPLE_INSTANCE *instancia_musica;
extern Vetor2D pos_item_arma;
extern bool item_ativo_no_mapa;
extern int total_pontos_no_mapa;
extern double timer_duracao_arma; 
extern double timer_respawn_arma;
extern Projetil tiros[MAX_TIROS];
#endif
