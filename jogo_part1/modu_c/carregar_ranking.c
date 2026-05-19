#include "ordenar_recordes.h"
#include "carregar_ranking.h"
#include "defs.h"

void carregar_ranking() {
    FILE *arq = fopen(ARQUIVO_RECORDES, "r");
    if (arq == NULL) {
        qtd_recordes_carregados = 0;
        return; // Arquivo ainda não existe
    }

    qtd_recordes_carregados = 0;
    char linha[200];

    while (fgets(linha, sizeof(linha), arq) && qtd_recordes_carregados < MAX_RECORDES_LIDOS) {
        // Parsing esperado: Nome;Tempo;Pontos;Inimigos;Musica;Mapa
        
        // 1. Nome
        char *token = strtok(linha, ";");
        if(token) strcpy(lista_recordes[qtd_recordes_carregados].nome_jogador, token);

        // 2. Tempo
        token = strtok(NULL, ";");
        if(token) lista_recordes[qtd_recordes_carregados].tempo_gasto = atof(token);

        // 3. Pontos
        token = strtok(NULL, ";");
        if(token) lista_recordes[qtd_recordes_carregados].pontuacao = atoi(token);

        // 4. Inimigos
        token = strtok(NULL, ";");
        if(token) lista_recordes[qtd_recordes_carregados].qtd_inimigos = atoi(token);
        
        // 5. Música
        token = strtok(NULL, ";");
        if(token) {
            // Remove newline caso seja um arquivo antigo onde a música era o último item
            token[strcspn(token, "\n")] = 0; 
            strcpy(lista_recordes[qtd_recordes_carregados].nome_musica, token);
        }

        // 6. Mapa (NOVO)
        token = strtok(NULL, ";");
        if(token) {
            // O newline (\n) agora estará aqui, pois é o último item da linha
            token[strcspn(token, "\n")] = 0;
            strcpy(lista_recordes[qtd_recordes_carregados].nome_mapa, token);
        } else {
            // Caso o arquivo seja antigo e não tenha o campo mapa, define um padrão para não bugar
            strcpy(lista_recordes[qtd_recordes_carregados].nome_mapa, "Gelo");
        }

        qtd_recordes_carregados++;
    }

    fclose(arq);
    ordenar_recordes(); 
}