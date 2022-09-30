#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <ostream>

#include "../inc/db.hpp"
#include "../sqlite3/sqlite3.h"

DB::DB(){};
DB::~DB(){};

int DB::test(void){
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    sqlite3_stmt * stmt;

    rc = sqlite3_open("test.db", &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }

    /* Create SQL statement */
    sql = "CREATE TABLE COMPANY("  \
        "ID INT PRIMARY KEY     NOT NULL," \
        "NAME           TEXT    NOT NULL," \
        "AGE            INT     NOT NULL," \
        "ADDRESS        CHAR(50)," \
        "SALARY         REAL );";

    sql = "INSERT INTO COMPANY VALUES (3, 'VASYA', 5, 'TEST', 9000)";
    /* Execute SQL statement */
    sqlite3_prepare( db, sql, -1, &stmt, NULL );//preparing the statement
    sqlite3_step( stmt );//executing the statement
    sqlite3_finalize(stmt);
    
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table created successfully\n");
    }
    sqlite3_close(db);
    return 0;
};

