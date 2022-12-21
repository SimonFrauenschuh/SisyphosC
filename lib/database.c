/* Copyright (C) 2021 Simon Frauenschuh - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */

#include <libpq-fe.h>
#include <string.h>
#include <stdio.h>

PGconn *conn;

// Create a connection to the db
void createDBconnection() {
    conn = PQconnectdb("user=pi password=BallOnPlateDSOwner dbname=ballonplateds");

    if (PQstatus(conn) == CONNECTION_BAD) {
        errorCode = 4;
        fprintf(stderr, "---ERROR 6--- Connection to database failed: %s\n",
            PQerrorMessage(conn));
        PQfinish(conn);
        exit(4);
    }
}

// End a connection to the db
void killDBconnection() {
    PQfinish(conn);
}

// To read the value with the highest index from a row
int readDatabase(char *rowName) {

    char select[70] = "SELECT ";
    char from[] = " FROM databasemodel ORDER BY id DESC LIMIT 1";
    strcat(select, rowName);
    strcat(select, from);

    PGresult *res = PQexec(conn, select);    
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        errorCode = 8;
        fprintf(stderr, "---ERROR 8--- No data retrieved\n");
        killDBconnection(conn);
        exit(8);
    }    
    
    int rows = PQntuples(res);
    int result = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    return result;
}

// To register a value to the db
int writeDatabase(int xEst, int yEst, int xReal, int yReal, int mode, int errorCode) {
    /*PGresult *res = PQexec(conn, "BEGIN");    
    
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {

        printf("BEGIN command failed\n");        
        PQclear(res);
        killDBconnection(conn);
    }

    char select[70] = "INSERT INTO databasemodel VALUES(MAX( id ) + 1, ";
    char valueString[3];
    snprintf( xEstStr, 3, "%i", xEst );
    strcat(select, valueString);
    strcat(select, ",");
    snprintf( xEstStr, 3, "%i", yEst );
    strcat(select, valueString);
    strcat(select, ",");
    snprintf( xEstStr, 3, "%i", xReal );
    strcat(select, valueString);
    strcat(select, ",");
    snprintf( xEstStr, 3, "%i", yReal );
    strcat(select, valueString);
    strcat(select, ",");
    // CONVERT TO CHAR ARRAY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    strcat(select, mode);
    strcat(select, ",");
    strcat(select, errorCode);
    strcat(select, ")");
    res = PQexec(conn, select);
    
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {

        printf("INSERT command failed\n");        
        PQclear(res);
        killDBconnection(conn);
    }       
    
    res = PQexec(conn, "COMMIT"); 
    
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {

        printf("COMMIT command failed\n");        
        PQclear(res);
        killDBconnection(conn);
    }
    */
}