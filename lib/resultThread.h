/* Copyright (C) 2021 Simon Frauenschuh - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */
#include "resultThread.c"

#pragma once

// A class that has it's own db-connection to write every second the result to the db
// The class is thread-safe because it only is active if the gamemode is activated.
// Otherwise critical: resultThread.c, line 19

// Thread to update the result in the db
void *threadproc(void *arg);

// Check if we're still in gamemode (only use inside of the thread!)
int checkMode();