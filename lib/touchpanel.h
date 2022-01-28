/* Copyright (C) 2021 Simon Frauenschuh & Philip Jessner - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */
#include "touchpanel.c"

#pragma once

extern int connectionTouchpanel;

// Initial function to connect to the Touchpanel
// Read from "Event 0" (Linux-Kernel)
void firstSetupTouchpanel();

// Function to check if the user is root
void checkUser();

// Function to read from the Touchpanel (driver) and write onto the given Pointers
void getTouchpanelPosition(int *posX, int *posY);
