//****************************************************************************************
//
//	File:		ProgressBar.h
//
//	Written by:	David Ramsey and Daniel Switkin
//
//	Copyright 1999, Be Incorporated
//
//	Revised by: Steffen Yount
//
//****************************************************************************************

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <interface/View.h>
#include "Prefs.h"

typedef struct {
	rgb_color color;
	BRect	rect;
} segment;

#define ltgray 216
#define dkgray 80

class ProgressBar : public BView {
	public:
		ProgressBar(BRect r, char* name);
		void Draw(BRect rect);
		void Set(int32 value);
		void UpdateColors();
		void AttachedToWindow();

		enum {
			PROGRESS_WIDTH	= 146,
			PROGRESS_HEIGHT	= 20
		};

	private:
		void Render(bool all = false);
		
		Prefs * prefs;
		segment segments[20];
		int32 current_value, previous_value;
};

#endif
