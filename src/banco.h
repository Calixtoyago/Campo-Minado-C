#ifndef BANCO_H
#define BANCO_H

#include <sqlite3.h>

sqlite3 *banco_abrir(const char *caminho);
void banco_fechar(sqlite3 *db);
sqlite3_int64 inserir_tempo(sqlite3 *db, int segundos, int concluido);

#endif