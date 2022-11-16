/* Copyright (C) 2021 Simon Frauenschuh - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */

#include <libpq-fe.h>

PGconn *conn;

// Create a connection to the db
void createDBconnection() {
    conn = PQconnectdb("user=pi password=BallOnPlateDSOwner dbname=ballonplateds");

    if (PQstatus(conn) == CONNECTION_BAD) {
        
        fprintf(stderr, "Connection to database failed: %s\n",
            PQerrorMessage(conn));
        PQfinish(conn);
    }
}

// End a connection to the db
void killDBconnection() {
    PQfinish(conn);
}

// To read the value with the highest index from a row
int readDatabase(char *rowName, int *result) {

    char select[70] = "SELECT ";
    char from[] = " FROM databasemodel ORDER BY id DESC LIMIT 1";
    strcat(select, rowName);
    strcat(select, from);

    PGresult *res = PQexec(conn, select);    
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {

        printf("No data retrieved\n");
    }    
    
    int rows = PQntuples(res);
    printf("%s\n", PQgetvalue(res, i, 0));

    PQclear(res);
}

// To register a value to a specific row
int writeDatabase(char *rowName, int xReal, int yReal, int error) {

}