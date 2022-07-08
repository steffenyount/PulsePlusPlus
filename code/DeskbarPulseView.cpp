//****************************************************************************************
//
//	File:		DeskbarPulseView.cpp
//
//	Written by:	Daniel Switkin
//
//	Copyright 1999, Be Incorporated
//
//	Revised by: Steffen Yount
//
//****************************************************************************************

#include "DeskbarPulseView.h"
#include "Common.h"
#include "Prefs.h"
#include <app/Application.h>
#include <interface/Deskbar.h>
#include <interface/Alert.h>
#include <Roster.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

DeskbarPulseView::DeskbarPulseView(BRect rect) : MiniPulseView(rect, "DeskbarPulseView") {
	messagerunner = NULL;
	prefs = NULL;
	prefswindow = NULL;
}

DeskbarPulseView::DeskbarPulseView(BMessage *message) : MiniPulseView(message) {
	mode1->SetLabel("Normal Mode");
	mode1->SetMessage(new BMessage(PV_NORMAL_MODE));
	mode2->SetLabel("Mini Mode");
	mode2->SetMessage(new BMessage(PV_MINI_MODE));
	quit = new BMenuItem("Quit", new BMessage(PV_QUIT), 0, 0);
	popupmenu->AddSeparatorItem();
	popupmenu->AddItem(quit);
	
	SetViewColor(B_TRANSPARENT_COLOR);
	
	prefs = new Prefs(true);
	
	SetViewColor(prefs->modeset[DESKBAR].Red(FRAME), 
		prefs->modeset[DESKBAR].Green(FRAME), prefs->modeset[DESKBAR].Blue(FRAME));
	
	messagerunner = NULL;
	prefswindow = NULL;
}

void DeskbarPulseView::AttachedToWindow() {
	BMessenger messenger(this);
	if (prefs != NULL) prefs = new Prefs(true);
	mode1->SetTarget(messenger);
	mode2->SetTarget(messenger);
	preferences->SetTarget(messenger);
	about->SetTarget(messenger);
	quit->SetTarget(messenger);
	
	if (CPU_Count() >= 2) {
		for (int x = 0; x < CPU_Count(); x++) {
			cpu_menu_items[x]->SetTarget(messenger);
		}
	}
	
	// Use a BMessageRunner to deliver periodic messsages instead
	// of Pulse() events from the Deskbar - this is to avoid changing
	// the current pulse rate and affecting other replicants
	messagerunner = new BMessageRunner(messenger, new BMessage(PV_REPLICANT_PULSE),
		200000, -1);
}

void DeskbarPulseView::Pulse() {
	// Override and do nothing here
}

void DeskbarPulseView::MessageReceived(BMessage *message) {
	switch (message->what) {
		case PV_NORMAL_MODE:
			SetMode(true);
			Remove();
			break;
		case PV_MINI_MODE:
			SetMode(false);
			Remove();
			break;
		case PV_PREFERENCES:
			Draw(Bounds());
			if (prefswindow != NULL) {
				prefswindow->Activate(true);
				break;
			}
			prefswindow = new PrefsWindow(prefs->prefs_window_rect,	"Pulse Preferences",
				new BMessenger(this), prefs);
			prefswindow->Show();
			break;
		case PV_ABOUT: {
			BAlert *alert = new BAlert("Info", STR_ABOUT, "OK");
			alert->Go(NULL);
			break;
		}
		case PV_QUIT:
			Remove();
			break;
		case PRV_COLOR_VALUE_CHANGED: {
			int32 mode = DESKBAR;
			message->FindInt32("mode", &mode);
			if (mode == DESKBAR)
				Draw(Bounds());
			break;
		}
		case PRV_DO_DISPLAY: {
			int32 mode = DESKBAR;
			message->FindInt32("mode", &mode);
			if (mode == DESKBAR) {
				ResizeTo(prefs->Deskbar_Icon_Width() - 1, 15);
				Draw(Bounds());
			}
			break;
		}
		case PRV_DESKBAR_BAR_WIDTH: {
			ResizeTo(prefs->Deskbar_Icon_Width() - 1, 15);
			Draw(Bounds());
			break;
		}
		case PV_REPLICANT_PULSE:
			Update();
			Draw(Bounds());
			break;
		case PRV_QUIT:
			prefswindow = NULL;
			break;
		case PV_CPU_MENU_ITEM:
			ChangeCPUState(message);
			break;
		default:
				BView::MessageReceived(message);
			break;
	}
}

DeskbarPulseView *DeskbarPulseView::Instantiate(BMessage *data) {
	if (!validate_instantiation(data, "DeskbarPulseView")) return NULL;
	return new DeskbarPulseView(data);
}

status_t DeskbarPulseView::Archive(BMessage *data, bool deep) const {
	PulseView::Archive(data, deep);
	data->AddString("add_on", APP_SIGNATURE);
	data->AddString("class", "DeskbarPulseView");
	return B_OK;
}

void DeskbarPulseView::Remove() {
	// Remove ourselves from the deskbar by name
	BDeskbar *deskbar = new BDeskbar();
	status_t err = deskbar->RemoveItem("DeskbarPulseView");
	if (err != B_OK) {
		char temp[255];
		sprintf(temp, "Remove(): %s", strerror(err));
		BAlert *alert = new BAlert("Info", temp, "OK");
		alert->Go(NULL);
	}
	delete deskbar;
}

void DeskbarPulseView::SetMode(bool normal) {
	if (normal) prefs->mode = NORMAL_WINDOW;
	else prefs->mode = MINI_WINDOW;
	prefs->Save();
	be_roster->Launch(APP_SIGNATURE);
}

DeskbarPulseView::~DeskbarPulseView() {
	if (messagerunner != NULL) delete messagerunner;
	if (prefswindow != NULL && prefswindow->Lock()) prefswindow->Quit();
	if (prefs != NULL) { 
		prefs->Save();
		delete prefs;
	}
}