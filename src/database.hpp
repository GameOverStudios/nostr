#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <sqlite3.h>
#include "event.hpp"

// Funções de banco de dados
sqlite3* open_database(const std::string& db_name);
void create_events_table(sqlite3* db);
void store_event(sqlite3* db, const Event& event);
std::vector<Event> retrieve_events(sqlite3* db);

#endif // DATABASE_HPP
