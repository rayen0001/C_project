#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

extern sqlite3* db;

void initializeDatabase();

#endif
