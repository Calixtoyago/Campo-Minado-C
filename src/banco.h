#ifndef BANCO_H
#define BANCO_H

#include <sqlite3.h>

typedef struct {
    int tempo;
    char data[20];
} Registro;

sqlite3 *banco_abrir(const char *caminho);
void banco_fechar(sqlite3 *db);
sqlite3_int64 inserir_tempo(sqlite3 *db, int segundos, int concluido);
int buscar_tempos(sqlite3 *db, Registro *lista);
int mostrar_ultimo_tempo(sqlite3 *db, Registro *ultimo);

#endif