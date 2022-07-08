//****************************************************************************************
//
//	File:		PriorityBar.cpp
//
//	Written by:	David Ramsey and Daniel Switkin
//
//	Copyright 1999, Be Incorporated
//
//	Revised by: Steffen Yount
//
//****************************************************************************************

#include "PriorityBar.h"
#include "PulseApp.h"

#define ltgray 216
#define dkgray 80

PriorityBar::PriorityBar(BRect r, char *name) :	BView(r, name, B_FOLLOW_NONE, B_WILL_DRAW) {	
	prefs = ((PulseApp *)be_app)->prefs;
	// Create 20 segments
	float height = (r.bottom - r.top) - 8;
	for (int32 counter = 0; counter < 20; counter++) {
		segments[counter].rect.Set(r.left + (counter * 7), r.top,
			(r.left + (counter * 7) + 5), r.top + height);
		segments[counter].rect.OffsetTo(BPoint((counter * 7) + 4, 4));
		segments[counter].priority = PRIORITY_COUNT -1;
	}
	SetLowColor(255, 0, 0);
	SetViewColor(B_TRANSPARENT_COLOR);
}

void PriorityBar::UpdateColors() {
	Render(true);
}

void PriorityBar::AttachedToWindow() {
	Render(true);
}

void PriorityBar::Set(float * values) {
	static float multiplyer = 20/(float)CPU_Count();

	int32 next_value = 0;
	float nextmark = 0;
	for (int32 i = 0; i < IDLE_PRIORITY; i++)
	{
		nextmark+= (values[i] * multiplyer);
		if (nextmark > 20) nextmark = 20;
		while(next_value < floor(nextmark+0.25))
		{
			if (segments[next_value].priority != i)
			{
				segments[next_value].priority = i;
				segments[next_value].changed = true;
			}
			next_value++;
		}
	}  
	for(int32 i = next_value; i < 20; i++)
	{
		if (segments[i].priority != IDLE_PRIORITY)
		{
			segments[i].priority = IDLE_PRIORITY;
			segments[i].changed = true;
		}
	}
				
	Render(false);
}

PriorityBar::~PriorityBar(){}	//added destructor

// Draws the progress bar. If "all" is true the entire bar is redrawn rather
// than just the part that changed.
void PriorityBar::Render(bool all) {
	ModeSettings * ms = &(prefs->modeset[NORMAL_WINDOW]);
	if (all) {
		// Black border
		BRect bounds = Bounds();
		bounds.InsetBy(2, 2);
		SetHighColor(ms->Red(FRAME), ms->Green(FRAME), ms->Blue(FRAME));
		StrokeRect(bounds);
		bounds.InsetBy(1, 1);
		StrokeRect(bounds);
		
		// Black dividers
		float left = bounds.left;
		BPoint start, end;
		for (int x = 0; x < 19; x++) {
			left += 7;
			start.Set(left, bounds.top);
			end.Set(left, bounds.bottom);
			StrokeLine(start, end);
		}
				
		for (int i = 0; i < 20; i++) {
			segments[i].changed = true;
		}
	}
	for (int i = 0; i < 20 ; i++) {
		if (segments[i].changed) {
			segments[i].changed = false;
			SetHighColor(ms->Red(segments[i].priority), 
				ms->Green(segments[i].priority), ms->Blue(segments[i].priority));
			FillRect(segments[i].rect);
		}
	}
	
	
	Sync();
}

void PriorityBar::Draw(BRect rect) {
	// Add bevels
	SetHighColor(dkgray, dkgray, dkgray);
	BRect frame = Bounds();
	StrokeLine(BPoint(frame.left, frame.top), BPoint(frame.right, frame.top));
	StrokeLine(BPoint(frame.left, frame.top + 1), BPoint(frame.right, frame.top + 1));
	StrokeLine(BPoint(frame.left, frame.top), BPoint(frame.left, frame.bottom));
	StrokeLine(BPoint(frame.left + 1, frame.top), BPoint(frame.left + 1, frame.bottom));
	
	SetHighColor(ltgray, ltgray, ltgray);
	StrokeLine(BPoint(frame.right-1, frame.top + 2), BPoint(frame.right - 1, frame.bottom));
	StrokeLine(BPoint(frame.right, frame.top + 1), BPoint(frame.right, frame.bottom));
	StrokeLine(BPoint(frame.left+1, frame.bottom - 1), BPoint(frame.right - 1, frame.bottom - 1));
	StrokeLine(BPoint(frame.left, frame.bottom), BPoint(frame.right, frame.bottom));
	
	Render(true);
}
