#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "../sqlite3/sqlite3.h"

class DB {

    static int callback(void *NotUsed, int argc, char **argv, char **azColName);

    public:
    DB();
    ~DB();

    int test(void);

};