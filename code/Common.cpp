//****************************************************************************************
//
//	File:		Common.cpp
//
//	Written by:	Steffen Yount
//
//	Copyright 1999
//
//****************************************************************************************

#include "Common.h"

const char * STR_MODE_NAMES[] = {	"Normal Mode",
								"Mini Mode",
								"Deskbar Mode" }; 

const char * STR_COLOR_NAMES[] = {	"Real Time Priority Color",
								"Urgent Priority Color",
								"Real Time Display Priority Color",
								"Urgent Display Priority Color",
								"Display Priority Color",
								"Normal Priority Color",
								"Low Priority Color",
								"Idle Priority Color",
								"Frame Color",
								"Idle Color",
								"Active Color" };

const int DEFAULT_COLORS[] = {	DEFAULT_REAL_TIME_COLOR,
								DEFAULT_URGENT_PRIORITY_COLOR,
								DEFAULT_REAL_TIME_DISPLAY_COLOR,
								DEFAULT_URGENT_DISPLAY_COLOR,
								DEFAULT_DISPLAY_PRIORITY_COLOR,
								DEFAULT_NORMAL_PRIORITY_COLOR,
								DEFAULT_LOW_PRIORITY_COLOR,
								DEFAULT_IDLE_PRIORITY_COLOR,
								DEFAULT_FRAME_COLOR,
								DEFAULT_PROC_IDLE_COLOR,
								DEFAULT_PROC_ACTIVE_COLOR };
								
const char STR_ABOUT[] = "Pulse++\n\nBy Steffen Yount\n\nBased on the classic Pulse app,\n\nBy David Ramsey, Arve Hjonnevag,\nand Daniel Switkin";

