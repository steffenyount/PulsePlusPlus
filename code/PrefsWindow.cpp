//****************************************************************************************
//
//	File:		PrefsWindow.cpp
//
//	Written by:	Daniel Switkin
//
//	Copyright 1999, Be Incorporated
//
//	Revised by: Steffen Yount
//
//****************************************************************************************

#include "PrefsWindow.h"
#include "Common.h"
#include "PulseApp.h"
#include "BottomPrefsView.h"
#include "ConfigView.h"
#include <interface/TabView.h>
#include <interface/TextControl.h>
#include <interface/Box.h>
#include <stdlib.h>
#include <stdio.h>

PrefsWindow::PrefsWindow(BRect rect, const char *name, BMessenger *messenger, Prefs *prefs) :
	BWindow(rect, name, B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE |
		B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS) {

	this->messenger = messenger;
	this->prefs = prefs;

	// This gives a nice look, and sets the background color correctly
	BRect bounds = Bounds();
	BBox *parent = new BBox(bounds, "ParentView", B_FOLLOW_NONE, B_PLAIN_BORDER);
	AddChild(parent);

	bounds.top += 10;
	bounds.bottom -= 45;
	BTabView *tabview = new BTabView(bounds, "TabView");
	tabview->SetFont(be_plain_font);
	tabview->SetViewColor(parent->ViewColor());
	tabview->ContainerView()->SetViewColor(parent->ViewColor());
	parent->AddChild(tabview);
	
	BRect viewsize = tabview->ContainerView()->Bounds();
	viewsize.InsetBy(5, 5);
	
	for(int32 mode = NORMAL_WINDOW; mode < MODE_COUNT; mode++) {	
		configview[mode] = new ConfigView(viewsize, STR_MODE_NAMES[mode], mode, prefs);
		configview[mode]->SetViewColor(tabview->ViewColor());
		tabview->AddTab(configview[mode]);
	}

	tabview->Select(NORMAL_WINDOW);
	
	bounds.top = bounds.bottom + 1;
	bounds.bottom += 45;
	BottomPrefsView *bottomprefsview = new BottomPrefsView(bounds, "BottomPrefsView");
	parent->AddChild(bottomprefsview);
}

void PrefsWindow::MessageReceived(BMessage *message) {
	switch (message->what) {
		case PRV_BOTTOM_OK:
			if (Lock()) Quit();
			break;
		case PRV_BOTTOM_DEFAULTS: {
			BTabView *tabview = (BTabView *)FindView("TabView");
			BTab *tab = tabview->TabAt(tabview->Selection());
			BView *view = tab->View();
			view->MessageReceived(message);
			break;
		}			
		default:
			BWindow::MessageReceived(message);
			break;
	}
}

void PrefsWindow::Quit() {
	messenger->SendMessage(new BMessage(PRV_QUIT));
	BWindow::Quit();
}

PrefsWindow::~PrefsWindow() {
	prefs->prefs_window_rect = Frame();
	prefs->Save();
	delete messenger;
}