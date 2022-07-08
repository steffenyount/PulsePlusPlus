//****************************************************************************************
//
//	File:		PulseWindow.cpp
//
//	Written by:	Daniel Switkin
//
//	Copyright 1999, Be Incorporated
//
//	Revised by: Steffen Yount
//
//****************************************************************************************

#include "PulseWindow.h"
#include "PulseApp.h"
#include "Common.h"
#include "DeskbarPulseView.h"
#include <interface/Alert.h>
#include <interface/Deskbar.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

PulseWindow::PulseWindow(BRect rect) :
	BWindow(rect, "Pulse++", B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE) {

	SetPulseRate(200000);
	
	PulseApp *pulseapp = (PulseApp *)be_app;
	BRect bounds = Bounds();
	normalpulseview = new NormalPulseView(bounds, "NormalPulseView", pulseapp->prefs);
	AddChild(normalpulseview);
	
	minipulseview = new MiniPulseView(bounds, "MiniPulseView", pulseapp->prefs);
	AddChild(minipulseview);
	
	mode = pulseapp->prefs->mode;
	if (mode == MINI_WINDOW) {
		SetLook(B_MODAL_WINDOW_LOOK);
		SetFeel(B_NORMAL_WINDOW_FEEL);
		SetFlags(B_NOT_ZOOMABLE);
		normalpulseview->Hide();
		SetSizeLimits(2, 4096, 2, 4096);
		ResizeTo(rect.Width(), rect.Height());
	} else minipulseview->Hide();
	
	prefswindow = NULL;
}

void PulseWindow::MessageReceived(BMessage *message) {
	switch(message->what) {
		case PV_NORMAL_MODE:
		case PV_MINI_MODE:
		case PV_DESKBAR_MODE:
			SetMode(message->what);
			break;
		case PRV_NORMAL_FADE_COLORS:
			normalpulseview->UpdateColors();
			break;
		case PRV_COLOR_VALUE_CHANGED: {
			int32 mode = NORMAL_WINDOW;
			message->FindInt32("mode", &mode);
			if (mode == NORMAL_WINDOW)
				normalpulseview->UpdateColors();
			else 
				minipulseview->Draw(minipulseview->Bounds());
			break;
		}
		case PRV_DO_DISPLAY: {
			Prefs * prefs = ((PulseApp *)be_app)->prefs;
			int32 mode = NORMAL_WINDOW;
			message->FindInt32("mode", &mode);
			if (mode == NORMAL_WINDOW) {
				prefs->Set_Normal_Window_Rect();
				normalpulseview->FixBars();
				if (prefs->mode == NORMAL_WINDOW) {
					ResizeTo(prefs->normal_window_rect.IntegerWidth(),
						prefs->normal_window_rect.IntegerHeight());
				}
			}
			break;
		}	
		case PRV_QUIT:
			prefswindow = NULL;
			break;
		case PV_PREFERENCES: {
			// If the window is already open, bring it to the front
			if (prefswindow != NULL) {
				prefswindow->Activate(true);
				break;
			}
			// Otherwise launch a new preferences window
			PulseApp *pulseapp = (PulseApp *)be_app;
			prefswindow = new PrefsWindow(pulseapp->prefs->prefs_window_rect,
				"Pulse Preferences", new BMessenger(this), pulseapp->prefs);
			prefswindow->Show();
			break;
		}
		case PV_ABOUT: {
			BAlert *alert = new BAlert("Info", STR_ABOUT, "OK");
			// Use the asynchronous version so we don't block the window's thread
			alert->Go(NULL);
			break;
		}
		case PV_QUIT:
			PostMessage(B_QUIT_REQUESTED);
			break;
		case PV_CPU_MENU_ITEM:
			// Call the correct version based on whose menu sent the message
			if (minipulseview->IsHidden()) normalpulseview->ChangeCPUState(message);
			else minipulseview->ChangeCPUState(message);
			break;
		default:
			BWindow::MessageReceived(message);
			break;
	}
}

void PulseWindow::SetMode(int newmode) {
	PulseApp *pulseapp = (PulseApp *)be_app;
	switch (newmode) {
		case PV_NORMAL_MODE:
			if (mode == MINI_WINDOW) {
				pulseapp->prefs->mini_window_rect = Frame();
				pulseapp->prefs->mode = NORMAL_WINDOW;
				pulseapp->prefs->Save();
			}
			minipulseview->Hide();
			mode = NORMAL_WINDOW;
			SetFlags(B_NOT_RESIZABLE | B_NOT_ZOOMABLE);
			pulseapp->prefs->Set_Normal_Window_Rect();
			ResizeTo(pulseapp->prefs->normal_window_rect.IntegerWidth(),
				pulseapp->prefs->normal_window_rect.IntegerHeight());
			MoveTo(pulseapp->prefs->normal_window_rect.left,
				pulseapp->prefs->normal_window_rect.top);
			SetType(B_TITLED_WINDOW);
			normalpulseview->Show();
			break;
		case PV_MINI_MODE:
			if (mode == NORMAL_WINDOW) {
				pulseapp->prefs->normal_window_rect = Frame();
				pulseapp->prefs->mode = MINI_WINDOW;
				pulseapp->prefs->Save();
			}
			normalpulseview->Hide();
			mode = MINI_WINDOW;
			SetLook(B_MODAL_WINDOW_LOOK);
			SetFeel(B_NORMAL_WINDOW_FEEL);
			SetFlags(B_NOT_ZOOMABLE);
			SetSizeLimits(2, 4096, 2, 4096);
			MoveTo(pulseapp->prefs->mini_window_rect.left,
				pulseapp->prefs->mini_window_rect.top);
			ResizeTo(pulseapp->prefs->mini_window_rect.IntegerWidth(),
				pulseapp->prefs->mini_window_rect.IntegerHeight());
			minipulseview->Show();
			break;
		case PV_DESKBAR_MODE:
			// Do not set window's mode to DESKBAR because the
			// destructor needs to save the correct BRect. ~PulseApp()
			// will handle launching the replicant after our prefs are saved.
			pulseapp->prefs->mode = DESKBAR;
			PostMessage(B_QUIT_REQUESTED);
			break;
	}
}

PulseWindow::~PulseWindow() {
	PulseApp *pulseapp = (PulseApp *)be_app;
	if (mode == MINI_WINDOW) pulseapp->prefs->mini_window_rect = Frame();
	if (mode == NORMAL_WINDOW) pulseapp->prefs->normal_window_rect = Frame();
}

bool PulseWindow::QuitRequested() {
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}