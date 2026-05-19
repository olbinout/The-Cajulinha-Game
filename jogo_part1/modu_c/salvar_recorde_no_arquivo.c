#include "salvar_recorde_no_arquivo.h"
#include "defs.h"

void salvar_recorde_no_arquivo() {
    if (recorde_ja_salvo) return;

    FILE *arq = fopen(ARQUIVO_RECORDES, "a"); 
    if (arq == NULL) {
        printf("Erro ao abrir arquivo de recordes!\n");
        return;
    }

    // Identifica o nome do mapa baseado no índice global
    char nome_mapa_atual[20];
    
    if (indice_mapa_selecionado == 0) {
        strcpy(nome_mapa_atual, "Gelo");
    } 
    else if (indice_mapa_selecionado == 1) {
        strcpy(nome_mapa_atual, "Floresta");
    } 
    else {
        strcpy(nome_mapa_atual, "Deserto");
    }

    // Adicionamos o %s no final para o mapa
    fprintf(arq, "%s;%.2f;%d;%d;%s;%s\n", 
        player.nome, 
        tempo_de_jogo, 
        player.pontos, 
        num_inimigos_selecionados, 
        nomes_musica_display[indice_musica_selecionada],
        nome_mapa_atual 
    );

    fclose(arq);
    printf("Recorde salvo com sucesso no mapa %s!\n", nome_mapa_atual);
    recorde_ja_salvo = true;
}