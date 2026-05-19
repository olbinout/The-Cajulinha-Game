#include "eh_parede.h"
#include "parar_musica.h"
#include "tocar_musica_selecionada.h"
#include "defs.h"

const char *arquivos_musica[QTD_OPCOES_MUSICA] = {
    NULL,
    "musics/musica01.wav",
    "musics/musica02.wav",
    "musics/musica03.wav",
    "musics/musica04.wav",
    "musics/musica05.wav",
    "musics/musica06.wav",
    "musics/musica07.wav",
    "musics/musica08.wav",
    "musics/musica09.wav",
    "musics/musica10.wav",
    "musics/musica11.wav",
    "musics/musica12.wav"
};

void tocar_musica_selecionada() {
    parar_musica(); 

    if (indice_musica_selecionada > 0 && arquivos_musica[indice_musica_selecionada] != NULL) {
        musica_fundo = al_load_sample(arquivos_musica[indice_musica_selecionada]);
        if (!musica_fundo) {
            printf("Erro ao carregar música: %s\n", arquivos_musica[indice_musica_selecionada]);
            return;
        }
        instancia_musica = al_create_sample_instance(musica_fundo);
        al_set_sample_instance_playmode(instancia_musica, ALLEGRO_PLAYMODE_LOOP);
        al_attach_sample_instance_to_mixer(instancia_musica, al_get_default_mixer());
        al_play_sample_instance(instancia_musica);
    }
}
