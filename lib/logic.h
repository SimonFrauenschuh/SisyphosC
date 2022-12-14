/* Copyright (C) 2021 Simon Frauenschuh & Philip Jessner - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */
#include "logic.c"

#pragma once

// Logic for mode single-point (define a point, where the ball should move to)
void moveToPoint(int xEst, int yEst);

// Synchronize with the angle, given by the database (the phones gyroscope)
void moveToAngle(int xEst, int yEst);