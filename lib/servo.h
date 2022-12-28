/* Copyright (C) 2021 Simon Frauenschuh & Philip Jessner - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */
#include "servo.c"

#pragma once

extern int connectionServo;

extern int errorCode;

// First initial setup (connection) for the Servos
void firstSetupServo();

// Calculates individually for each Servo the needed PWM-Signal for the given angle
int calculateServoPWMSignal(int channel, double degree);

// Method to set a specific servo to a specific angle; without any Smoothing
void setServoDegree(int channel, double degree);

// Moves all Servos to 0 degree (horizontal)
void setServoNull();
