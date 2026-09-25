#include <stdio.h>
#include "sqlite3.h"
#include "banco.h"

sqlite3 *banco_abrir(const char *caminho) {
    sqlite3 *db;
    sqlite3_open(caminho, &db);

    const char *sql = "CREATE TABLE IF NOT EXISTS tempos ("
                      "  id        INTEGER PRIMARY KEY,"
                      "  tempo     INTEGER NOT NULL,"
                      "  concluido INTEGER NOT NULL DEFAULT 0,"
                      "  data      TEXT DEFAULT (datetime('now', 'localtime'))"
                      ");";

    char *err = NULL;
    if (sqlite3_exec(db, sql, NULL, NULL, &err) != SQLITE_OK) {
        fprintf(stderr, "Erro ao criar tabela: %s\n", err);
        sqlite3_free(err);
        sqlite3_close(db);
        return NULL;
    }
    return db;
}

void banco_fechar(sqlite3 *db) {
    sqlite3_close(db);
}

sqlite3_int64 inserir_tempo(sqlite3 *db, int segundos, int concluido) {
    sqlite3_stmt *stmt;
    long long id = -1;
    if (sqlite3_prepare_v2(db, "INSERT INTO tempos (tempo, concluido) VALUES (?, ?);", -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Erro: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, segundos);
    sqlite3_bind_int(stmt, 2, concluido);

    if (sqlite3_step(stmt) == SQLITE_DONE)
        id = sqlite3_last_insert_rowid(db);
    else
        fprintf(stderr, "Erro: %s\n", sqlite3_errmsg(db));

    sqlite3_finalize(stmt);
    return id;
}

int buscar_tempos(sqlite3 *db, Registro *lista) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT tempo, data "
                      "FROM tempos "
                      "WHERE concluido = 1 "
                      "ORDER BY tempo ASC "
                      "LIMIT 10;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Erro: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    int n = 0, rc;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        lista[n].tempo = sqlite3_column_int(stmt, 0);
        const unsigned char *data = sqlite3_column_text(stmt, 1);
        snprintf(lista[n].data, sizeof(lista[n].data), "%s",
                 data ? (const char *)data : "");
        n++;
    }

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Erro: %s\n", sqlite3_errmsg(db));
        n = -1;
    }

    sqlite3_finalize(stmt);
    return n;
}

int mostrar_ultimo_tempo(sqlite3 *db, Registro *ultimo) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT  tempo, data "
                      "FROM tempos "
                      "WHERE id == ? "
                      "LIMIT 1;";

    sqlite3_int64 id = sqlite3_last_insert_rowid(db);
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Erro: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, id);

    int resultado;
    int rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        ultimo->tempo = sqlite3_column_int(stmt, 0);
        const unsigned char *data = sqlite3_column_text(stmt, 1);
        snprintf(ultimo->data, sizeof(ultimo->data), "%s",
                 data ? (const char *)data : "");
        resultado = 1;
    } else if (rc == SQLITE_DONE) {
        resultado = 0;
    } else {
        fprintf(stderr, "Erro: %s\n", sqlite3_errmsg(db));
        resultado = -1;
    }

    sqlite3_finalize(stmt);
    return resultado;
}
