//****************************************************************************************
//
//	File:		ConfigView.cpp
//
//	Written by:	Daniel Switkin
//
//	Copyright 1999, Be Incorporated
//
//	Revised by: Steffen Yount
//
//****************************************************************************************

#include "ConfigView.h"
#include "Common.h"
#include "PulseApp.h"
#include "PrefsWindow.h"
#include <interface/Box.h>
#include <stdlib.h>
#include <stdio.h>
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <MenuBar.h>


RTColorControl::RTColorControl(BPoint point, BMessage *message)
	: BColorControl(point, B_CELLS_32x8, 6, "ColorControl", message, false) {

}

// Send a message every time the color changes, not just
// when the mouse button is released
void RTColorControl::SetValue(int32 color) {
	BColorControl::SetValue(color);
	Invoke();
}

// A single class for all three prefs views, needs to be
// customized below to give each control the right message
ConfigView::ConfigView(BRect rect, const char *name, int mode, Prefs *prefs) :
	BView(rect, name, B_FOLLOW_NONE, B_WILL_DRAW) {

	this->mode = mode;
	colorindex = PROC_ACTIVE;
	
	first_time_attached = true;
	fadecolors = NULL;
	colormenu = NULL;
	priority_display = NULL;
	standard_display = NULL;

	barwidth = NULL;

	BRect r(6, 5, 296, 155);
	BBox *bbox = new BBox(r, "BBox");
	bbox->SetLabel("Bar Colors");
	AddChild(bbox);
	
	colorcontrol = new RTColorControl(BPoint(10, 20), 
		new BMessage(PRV_COLOR_VALUE_CHANGED));

	bbox->AddChild(colorcontrol);

	colorcontrol->SetValue(prefs->modeset[mode].color[PROC_ACTIVE]);
	
	r = colorcontrol->Frame();
	r.top = r.bottom + 10;
	r.bottom = r.top + 15;
	r.right = bbox->Bounds().right - 50;
	
	
	BMenuBar * colormenubar = new BMenuBar(r, "colormenubar", B_FOLLOW_NONE);
	bbox->AddChild(colormenubar);
	colormenu = new BPopUpMenu(STR_COLOR_NAMES[PROC_ACTIVE]);
	colormenubar->AddItem(colormenu);
	
	for (int32 color = PROC_ACTIVE; color >= REAL_TIME; color--) {
		BMessage * message = new BMessage(PRV_COLOR_SELECTION_CHANGED);
		message->AddInt32("colorindex", color);
		BMenuItem * colormenuitem = new BMenuItem(STR_COLOR_NAMES[color], message);
		colormenu->AddItem(colormenuitem);
		if (color == PROC_ACTIVE)
			colormenuitem->SetMarked(true);
	}
	
	r = colormenubar->Frame();
	r.top = r.bottom + 10;
	r.bottom = r.top + 15;
	r.right = r.left + 125;

	priority_display = new BCheckBox(r, "priority_display", "Show System Bar",
			new BMessage(PRV_DO_DISPLAY)); 	
	priority_display->SetValue(prefs->modeset[mode].do_display.priority);
	bbox->AddChild(priority_display);
	
	r.OffsetBy(0, 20);
	if (CPU_Count() > 1) {
		 standard_display = new BCheckBox(r, "standard_display", "Show Processor Bars",
			new BMessage(PRV_DO_DISPLAY)); 
	} else {
		 standard_display = new BCheckBox(r, "standard_display", "Show Processor Bar",
			new BMessage(PRV_DO_DISPLAY)); 
	}
	standard_display->SetValue(prefs->modeset[mode].do_display.standard);
	bbox->AddChild(standard_display);

	r = colormenubar->Frame();
	r.top = r.bottom + 10;
	r.bottom = r.top + 15;
	r.right = bbox->Bounds().right - 15;
	r.left = r.right - 100;
	
	if (mode == NORMAL_WINDOW) {
		r.left += 20;
		r.OffsetBy(0, 20);
		fadecolors = new BCheckBox(r, "FadeColors", "Fade colors",
			new BMessage(PRV_NORMAL_FADE_COLORS));
		bbox->AddChild(fadecolors);
		fadecolors->SetValue(prefs->normal_fade_colors);
	} else  if (mode == DESKBAR) {
		r.OffsetBy(0, 10);
		char temp[10];
		sprintf(temp, "%d", (int)prefs->deskbar_bar_width);
		barwidth = new BTextControl(r, "Width", "Width of bars:", temp,
			new BMessage(PRV_DESKBAR_BAR_WIDTH));
		bbox->AddChild(barwidth);
		barwidth->SetDivider(75);
		
		colorcontrol->SetValue(prefs->modeset[DESKBAR].color[PROC_ACTIVE]);
	} 
}

void ConfigView::AttachedToWindow() {
	BView::AttachedToWindow();
	
	// AttachedToWindow() gets called every time this tab is brought
	// to the front, but we only want this initialization to happen once
	if (first_time_attached) {
		BMessenger messenger(this);
		colorcontrol->SetTarget(messenger);
		if (fadecolors != NULL) fadecolors->SetTarget(messenger);
		if (colormenu != NULL) colormenu->SetTargetForItems(messenger);
		if (barwidth != NULL) barwidth->SetTarget(messenger);
		if (standard_display != NULL) standard_display->SetTarget(messenger);	
		if (priority_display != NULL) priority_display->SetTarget(messenger);		
		first_time_attached = false;
	}
}

void ConfigView::MessageReceived(BMessage *message) {
	PrefsWindow *prefswindow = (PrefsWindow *)Window();
	if (prefswindow == NULL) return;
	Prefs *prefs = prefswindow->prefs;
	BMessenger *messenger = prefswindow->messenger;
	
	switch (message->what) {
		// These two send the color and the status of the fade checkbox together
		case PRV_NORMAL_FADE_COLORS: {
			bool fade_colors = (bool)fadecolors->Value();
			message->AddBool("fade", fade_colors);
			prefs->normal_fade_colors = fade_colors;
			messenger->SendMessage(message);
			break;
		}
		case PRV_DO_DISPLAY:
			prefs->modeset[mode].do_display.standard = (bool)standard_display->Value();
			prefs->modeset[mode].do_display.priority = (bool)priority_display->Value();
			prefs->Save();
			message->AddInt32("mode", mode);
			messenger->SendMessage(message);
			break;
		case PRV_COLOR_VALUE_CHANGED:
			prefs->modeset[mode].color[colorindex] = colorcontrol->Value();
			message->AddInt32("mode", mode);
			messenger->SendMessage(message);
			break;
		case PRV_COLOR_SELECTION_CHANGED:
			message->FindInt32("colorindex", &colorindex);
			colorcontrol->SetValue(prefs->modeset[mode].color[colorindex]);
			break;
		case PRV_DESKBAR_BAR_WIDTH: {
			// Make sure the width shows at least one pixel per CPU and
			// that it will fit in the tray in any Deskbar orientation
			int width = atoi(barwidth->Text());
			if (width < DESKBAR_BAR_WIDTH_MIN || width > DESKBAR_BAR_WIDTH_MAX) {
				char temp[10];
				if (width < DESKBAR_BAR_WIDTH_MIN)
					width = DESKBAR_BAR_WIDTH_MIN;
				else 
					width = DESKBAR_BAR_WIDTH_MAX;
				sprintf(temp, "%d", width);
				barwidth->SetText(temp);
			}
			prefs->deskbar_bar_width = width;
			messenger->SendMessage(message);
			break;
		}
		case PRV_BOTTOM_DEFAULTS:
			ResetDefaults();
			break;
		default:
			BView::MessageReceived(message);
			break;
	}
}

// Only reset our own controls to default
void ConfigView::ResetDefaults() {
	PrefsWindow *prefswindow = (PrefsWindow *)Window();
	if (prefswindow == NULL) return;
	Prefs *prefs = prefswindow->prefs;
	BMessenger *messenger = prefswindow->messenger;

	
	for (int i = 0; i < COLOR_COUNT; i++) {
		prefs->modeset[mode].color[i] = DEFAULT_COLORS[i];
	}
	
	colorcontrol->SetValue(prefs->modeset[mode].color[colorindex]);
	
	if (mode == NORMAL_WINDOW) fadecolors->SetValue(DEFAULT_NORMAL_FADE_COLORS);

	BMessage *message = new BMessage(PRV_COLOR_VALUE_CHANGED);
	message->AddInt32("mode", mode);
	messenger->SendMessage(message);

	if (mode == DESKBAR) {
		char temp[10];
		sprintf(temp, "%d", DEFAULT_DESKBAR_BAR_WIDTH);
		barwidth->SetText(temp);
		// Need to force the model message to be sent
		barwidth->Invoke();
	}
}