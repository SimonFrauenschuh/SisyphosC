/* Copyright (C) 2021 Simon Frauenschuh - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */
#include "database.c"

#pragma once

// Create a connection to the db
void createDBconnection();

// End a connection to the db
void killDBconnection();

// To read the value with the highest index from a row
int readDatabase(char *rowName);

// To register a value to a specific row
int writeDatabase(int xReal, int yReal);