#include "database.hpp"
#include <iostream>

sqlite3* open_database(const std::string& db_name) {
    sqlite3* db;
    int rc = sqlite3_open(db_name.c_str(), &db);
    if (rc) {
        std::cerr << "Erro ao abrir o banco de dados: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }
    return db;
}

void create_events_table(sqlite3* db) {
    std::string sql = "CREATE TABLE IF NOT EXISTS events ("
                      "id TEXT PRIMARY KEY, "
                      "pubkey TEXT, "
                      "content TEXT, "
                      "sig TEXT, "
                      "created_at INTEGER);";
    char* errmsg;
    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errmsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao criar tabela: " << errmsg << std::endl;
        sqlite3_free(errmsg);
    }
}

void store_event(sqlite3* db, const Event& event) {
    std::string sql = "INSERT INTO events (id, pubkey, content, sig, created_at) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, event.id.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, event.pubkey.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, event.content.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, event.sig.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 5, event.created_at);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Erro ao inserir evento no banco de dados: " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
}

std::vector<Event> retrieve_events(sqlite3* db) {
    std::vector<Event> events;
    std::string sql = "SELECT id, pubkey, content, sig, created_at FROM events;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao preparar consulta: " << sqlite3_errmsg(db) << std::endl;
        return events; // Retornar vetor vazio em caso de erro
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Event event;
        event.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        event.pubkey = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        event.content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        event.sig = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        event.created_at = sqlite3_column_int64(stmt, 4);
        events.push_back(event);
    }

    sqlite3_finalize(stmt);
    return events;
}
