#include <libpq-fe.h>

PGconn *connThread;

// Check if we're still in gamemode (only use inside of the thread!)
int checkMode() {
    char select[70] = "SELECT mode FROM postouchpanel ORDER BY id DESC LIMIT 1";

    PGresult *res = PQexec(connThread, select);    
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        errorCode = 8;
        fprintf(stderr, "---ERROR 8--- No data retrieved\n");
        PQfinish(connThread);
        exit(8);
    }    
    
    int rows = PQntuples(res);
    int result = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    return result;
}

// Thread to update the result in the db
void *threadproc(void *arg) {
	connThread = PQconnectdb("user=pi password=BallOnPlateDSOwner dbname=ballonplateds");

	if (PQstatus(connThread) == CONNECTION_BAD) {
		errorCode = 4;
		fprintf(stderr, "---ERROR 6--- Connection to database failed: %s\n",
			PQerrorMessage(connThread));
		PQfinish(connThread);
		exit(4);
	}

	int result = 100;
	while(checkMode() == 1) {
		sleep(1);
		int xReal, yReal;
		getTouchpanelPositionADC(&xReal, &yReal);

		double xRealD = xReal;
		double yRealD = yReal;
		if ((xRealD / 179) > 1) {
			xRealD = 179 - (xRealD - 179);
		}
		if ((yRealD / 106) > 1) {
			yRealD = 106 - (yRealD - 106);
		}
		// Max. 3% verlieren pro Sekunde
		result = result - 0.03 * result + 10 * (xRealD / 179) * (yRealD / 106);
		if (result > 100) {
			result = 100;
		} else if (result < 0) {
			result = 0;
		}

		PGresult *res = PQexec(connThread, "BEGIN");    
    
		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			printf("BEGIN command failed\n");        
			PQclear(res);
			PQfinish(connThread);
		}

		// Modify the real x-value of the highest id
		char updateX[85] = "UPDATE result SET result=";
		char valueString[4];
		snprintf(valueString, 4, "%i", result);
		strcat(updateX, valueString);
		strcat(updateX, " WHERE id=(select max(id) from result)");
		res = PQexec(connThread, updateX);
		
		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			printf("UPDATE command failed\n");        
			PQclear(res);
			PQfinish(connThread);
		} 
			
		res = PQexec(connThread, "COMMIT"); 
			
		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			printf("COMMIT command failed\n");        
			PQclear(res);
			PQfinish(connThread);
		}
	}
}