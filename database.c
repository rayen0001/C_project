#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#include "database.h"

sqlite3* db;

void initializeDatabase() {
    if (sqlite3_open("stock.db", &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    char* errMsg;
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS stock (name TEXT, quantity INT, price REAL, alert_threshold INT);", NULL, NULL, &errMsg);
}
