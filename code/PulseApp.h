//****************************************************************************************
//
//	File:		PulseApp.h
//
//	Written by:	Daniel Switkin
//
//	Copyright 1999, Be Incorporated
//
//	Revised by: Steffen Yount
//
//****************************************************************************************

#ifndef PULSEAPP_H
#define PULSEAPP_H

#include <app/Application.h>
#include "Prefs.h"

bool LastEnabledCPU(int my_cpu);
bool LoadInDeskbar();
int32 CPU_Count();

class PulseApp : public BApplication {
	public:
		PulseApp(int mode);
		~PulseApp();
		
		Prefs *prefs;
};


#endif
