#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <sqlite3.h>
#include "event.hpp"

sqlite3* open_database(const std::string& db_name);
void create_events_table(sqlite3* db);
void store_event(sqlite3* db, const Event& event);

#endif // DATABASE_H
