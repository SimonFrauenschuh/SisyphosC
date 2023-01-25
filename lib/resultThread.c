#include <libpq-fe.h>
#include <time.h>
#include <wiringPi.h>

PGconn *connThread;

// Check if we're still in gamemode (only use inside of the thread!)
int checkMode() {
    char select[70] = "SELECT mode FROM result ORDER BY id DESC LIMIT 1";

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

// To register the result in the db
void registerResult(int result) {
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

// To change the pins according to the result to be read by the arduino
void setIO(int result) {
	// Calculate result, so it can be 0...7
	(result == 100) ? (result = result / 12.5 - 1) : (result /= 12.5);
	// Binary check, if we need to set the output
	// e.g. 6 --> 110 --> 110 & 0b100 = 1 --> HIGH
	digitalWrite(0, result & 0b100);
	digitalWrite(2, result & 0b010); 
	digitalWrite(3, result & 0b001);
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

	// Declaration outside of while-loop to keep previous value
	float result = 100;
	// Declaration outside of while-loop to improve performance
	int xReal, yReal;
	// Set once outside of while-loop to improve performance
	pinMode(0, OUTPUT);
    pinMode(2, OUTPUT);
	pinMode(2, OUTPUT);
	while(1) {
		// Mode 1: gamemode, try to center the ball
		if (checkMode() == 1) {
			getTouchpanelPositionADC(&xReal, &yReal);

			double xRealD = xReal;
			double yRealD = yReal;
			if ((xRealD / 179) > 1) {
				xRealD = 179 - (xRealD - 179);
			}
			if ((yRealD / 106) > 1) {
				yRealD = 106 - (yRealD - 106);
			}
			// To calculate the new value, loose a bit and be able to gain a bit
			result = result - 0.009 * result + 1.6 * (xRealD / 179) * (yRealD / 106);
			if (result > 100) {
				result = 100;
			} else if (result < 0) {
				result = 0;
			}

			setIO((int)result);
			registerResult((int)result);

			// Register a new value every 200ms
			sleep(0.2);
		} else if (checkMode() == 2) {
			int xStart, yStart;
			struct timeval begin, end;
			getTouchpanelPositionADC(&xStart, &yStart);
			
			// Start measuring time
    		gettimeofday(&begin, 0);

			// Wait for 3 Seconds because nobody is gonna to be faster than that and to allow the ball to move away
			sleep(3);

			// As long, as the right-now value isn't kind of the same as the beginning: wait until it is
			do {
				getTouchpanelPositionADC(&xReal, &yReal);
				} while((xReal < xStart - 10) || (xReal > xStart + 10) || (yReal < yStart - 10) || (yReal > yStart + 10));

			// Stop measuring time and calculate the elapsed time in seconds
			gettimeofday(&end, 0);
			int diffSeconds = end.tv_sec - begin.tv_sec;
			int diffTenthOfSeconds = (end.tv_usec - begin.tv_usec) / 100000;
			if (diffTenthOfSeconds < 0) {
				diffSeconds--;
				diffTenthOfSeconds += 10;
			}
			// Format: SSM
			// E.g. 123 gets stored, show in WebView like 12,3s (conversion in Java, not here)
			result = diffSeconds * 10 + diffTenthOfSeconds;

			registerResult((int)result);

			// To ensure that the value can be viewed at the web-view
			sleep(5);
		}
	}
}