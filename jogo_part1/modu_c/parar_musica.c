#include "parar_musica.h"
#include "defs.h"

void parar_musica() {
    if (instancia_musica) {
        al_stop_sample_instance(instancia_musica);
        al_detach_sample_instance(instancia_musica);
        al_destroy_sample_instance(instancia_musica);
        instancia_musica = NULL;
    }
    if (musica_fundo) {
        al_destroy_sample(musica_fundo);
        musica_fundo = NULL;
    }
}