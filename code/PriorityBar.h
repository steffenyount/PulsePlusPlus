//****************************************************************************************
//
//	File:		PriorityBar.h
//
//	Written by:	David Ramsey and Daniel Switkin
//
//	Copyright 1999, Be Incorporated
//
//	Revised by: Steffen Yount
//
//****************************************************************************************

#ifndef PriorityBar_H
#define PriorityBar_H

#include <interface/View.h>
#include "Prefs.h"

typedef struct {
	int 	priority;
	BRect	rect;
	bool	changed;
} pri_segment;

class PriorityBar : public BView {
	public:
		PriorityBar(BRect r, char* name);
		void Draw(BRect rect);
		void Set(float * value);
		void UpdateColors();
		void AttachedToWindow();

		enum {
			PROGRESS_WIDTH	= 146,
			PROGRESS_HEIGHT	= 20
		};

	private:
		void Render(bool all = false);
		pri_segment segments[20];
		Prefs * prefs;
		
};

#endif
