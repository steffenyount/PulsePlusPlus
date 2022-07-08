//****************************************************************************************
//
//	File:		ConfigView.h
//
//	Written by:	Daniel Switkin
//
//	Copyright 1999, Be Incorporated
//
//	Revised by: Steffen Yount
//
//****************************************************************************************

#ifndef CONFIGVIEW_H
#define CONFIGVIEW_H

#include <interface/CheckBox.h>
#include <interface/RadioButton.h>
#include <interface/TextControl.h>
#include <interface/ColorControl.h>
#include "Prefs.h"

class RTColorControl : public BColorControl {
	public:
		RTColorControl(BPoint point, BMessage *message);
		void SetValue(int32 color);
};

class ConfigView : public BView {
	public:
		ConfigView(BRect rect, const char *name, int mode, Prefs *prefs);
		void AttachedToWindow();
		void MessageReceived(BMessage *message);
		
	private:
		void ResetDefaults();
		bool first_time_attached;
		int32 mode;
		int32 colorindex;
		
		RTColorControl *colorcontrol;
		BPopUpMenu * colormenu;
		BCheckBox *standard_display;
		BCheckBox *priority_display;

		// For Normal
		BCheckBox *fadecolors;
		// For Deskbar
		BTextControl *barwidth;
};

#endif
