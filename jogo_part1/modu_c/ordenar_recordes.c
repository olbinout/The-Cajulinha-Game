#include "ordenar_recordes.h"
#include "defs.h"

void ordenar_recordes() {
    for (int i = 0; i < qtd_recordes_carregados - 1; i++) {
        for (int j = 0; j < qtd_recordes_carregados - i - 1; j++) {
            bool trocar = false;
            
            // CRITÉRIO 1: PONTOS
            if (lista_recordes[j].pontuacao < lista_recordes[j+1].pontuacao) {
                trocar = true;
            }
            // Se empatar nos pontos...
            else if (lista_recordes[j].pontuacao == lista_recordes[j+1].pontuacao) {
                
                // CRITÉRIO 2: TEMPO (Quem fez mais rápido vence)
                if (lista_recordes[j].tempo_gasto > lista_recordes[j+1].tempo_gasto) {
                    trocar = true; 
                }
                // Se empatar nos pontos E no tempo...
                else if (lista_recordes[j].tempo_gasto == lista_recordes[j+1].tempo_gasto) {
                    
                    // CRITÉRIO 3: QUANTIDADE DE INIMIGOS (Quem enfrentou mais vence)
                    if (lista_recordes[j].qtd_inimigos < lista_recordes[j+1].qtd_inimigos) {
                        trocar = true;
                    }
                }
            }

            if (trocar) {
                Recorde temp = lista_recordes[j];
                lista_recordes[j] = lista_recordes[j+1];
                lista_recordes[j+1] = temp;
            }
        }
    }
}

