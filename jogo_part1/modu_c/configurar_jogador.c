#include "configurar_jogador.h"
#include "defs.h"

void configurar_jogador(Jogador *p, const char *nome_digitado) {
    if (p->nome) free(p->nome);
    p->nome = (char*)malloc(50 * sizeof(char));
    
    if (strlen(nome_digitado) > 0) {
        strcpy(p->nome, nome_digitado);
    } else {
        strcpy(p->nome, "Player One");
    }
    printf("Jogador configurado: %s\n", p->nome);
}
