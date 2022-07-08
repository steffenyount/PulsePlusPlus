//****************************************************************************************
//
//	File:		PulseApp.cpp
//
//	Written by:	Daniel Switkin
//
//	Copyright 1999, Be Incorporated
//
//	Revised by: Steffen Yount
//
//****************************************************************************************

#include "PulseApp.h"
#include "Common.h"
#include "PulseWindow.h"
#include "DeskbarPulseView.h"
#include <interface/Alert.h>
#include <interface/Rect.h>
#include <interface/Deskbar.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// This function allows testing the display for cpu counts != mine
int32 CPU_Count() {
	static int32 CPU_Count = 0;
	if (!CPU_Count) {
		system_info sys_info;
		get_system_info(&sys_info);
		CPU_Count = sys_info.cpu_count;
	}
	return CPU_Count;
}

// Make sure we don't disable the last CPU - this is needed by
// descendants of PulseView for the popup menu and for CPUButton
// both as a replicant and not
bool LastEnabledCPU(int my_cpu) {
	if (CPU_Count() == 1) return true;
	
	for (int x = 0; x < CPU_Count(); x++) {
		if (x == my_cpu) continue;
		if (_kget_cpu_state_(x) == 1) return false;
	}
	return true;
}

// Ensure that the mini mode and deskbar mode always show an indicator
// for each CPU, at least one pixel wide
int GetMinimumViewWidth(Prefs * prefs, int mode) {
	return prefs->Bar_Count(mode) * 3 + 1;
}


bool LoadInDeskbar() {
	PulseApp *pulseapp = (PulseApp *)be_app;
	BDeskbar 	deskbar;								//BJF
//	BDeskbar *deskbar = new BDeskbar();
	// Don't allow two copies in the Deskbar at once
	if (deskbar.HasItem("DeskbarPulseView")) {			//BJF
//	if (deskbar->HasItem("DeskbarPulseView")) {
//		delete deskbar;
		return false;
	}
	
	// Must be 16 pixels high, the width is retrieved from the Prefs class
	int width = pulseapp->prefs->Deskbar_Icon_Width();
			
	BRect rect(0, 0, width - 1, 15);
	DeskbarPulseView *replicant = new DeskbarPulseView(rect);
	status_t err = deskbar.AddItem(replicant);
//	status_t err = deskbar->AddItem(replicant);
	delete replicant;
//	delete deskbar;
	if (err != B_OK) {
		BAlert *alert = new BAlert(NULL, strerror(err), "OK");
		alert->Go(NULL);
		return false;
	} else return true;
}

PulseApp::PulseApp(int mode) : BApplication(APP_SIGNATURE) {

	prefs = new Prefs(true);
	if (mode != -1) prefs->mode = mode;
	
	PulseWindow *pulsewindow = NULL;
	if (prefs->mode == NORMAL_WINDOW) {
		pulsewindow = new PulseWindow(prefs->normal_window_rect);
	} else if (prefs->mode == MINI_WINDOW) {
		pulsewindow = new PulseWindow(prefs->mini_window_rect);
	// Remove this case for Deskbar add on API
	} else if (prefs->mode == DESKBAR) {
		if (LoadInDeskbar()) {
			PostMessage(new BMessage(B_QUIT_REQUESTED));
			return;
		} else {
			// If loading the replicant fails, launch the app instead
			// This allows having the replicant and the app open simultaneously
			prefs->mode = NORMAL_WINDOW;
			pulsewindow = new PulseWindow(prefs->normal_window_rect);
		}
	}
	
	pulsewindow->Show();
}

PulseApp::~PulseApp() {
	// Load the replicant after we save our preferences so they don't
	// get overwritten by DeskbarPulseView's instance
	prefs->Save();
	if (prefs->mode == DESKBAR) LoadInDeskbar();
	delete prefs;
}

int main(int argc, char **argv) {
	// Parse arguments for backward compatibility - this mode will override
	// the setting stored in the user's settings directory
	// Color and width information is no longer accepted though
	int mode = -1;
	for (int x = 1; x < argc; x++) {
		if (strstr(argv[x], "deskbar")) {
			mode = DESKBAR;
			break;
		} else if (strstr(argv[x], "mini")) mode = MINI_WINDOW;
		else if (strstr(argv[x], "normal")) mode = NORMAL_WINDOW;
		else if (strstr(argv[x], "help")) {
			printf("usage: Pulse++ [-normal | -mini | -deskbar]\n");
			return 0;
		}
	}

	PulseApp *pulseapp = new PulseApp(mode);
	pulseapp->Run();
	delete pulseapp;
	return 0;
}