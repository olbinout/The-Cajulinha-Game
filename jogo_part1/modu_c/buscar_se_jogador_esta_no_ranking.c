#include "buscar_se_jogador_esta_no_ranking.h"
#include "defs.h"

bool buscar_se_jogador_esta_no_ranking(const char* nome_buscado) {
    for(int i = 0; i < qtd_recordes_carregados; i++) {
        if (strcmp(lista_recordes[i].nome_jogador, nome_buscado) == 0) {
            return true; 
        }
    }
    return false;
}


