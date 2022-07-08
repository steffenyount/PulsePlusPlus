//****************************************************************************************
//
//	File:		NormalPulseView.h
//
//	Written by:	Daniel Switkin
//
//	Copyright 1999, Be Incorporated
//
//	Revised by: Steffen Yount
//
//****************************************************************************************

#ifndef NORMALPULSEVIEW_H
#define NORMALPULSEVIEW_H

#include "PulseView.h"
#include "ProgressBar.h"
#include "PriorityBar.h"
#include "CPUButton.h"

class NormalPulseView : public PulseView {
	public:
		NormalPulseView(BRect rect, const char *name, Prefs * prefs);
		~NormalPulseView();
		void Draw(BRect rect);
		void Pulse();
		void AttachedToWindow();
		void UpdateColors();
		void FixBars();

	private:
		int CalculateCPUSpeed();
		void DetermineVendorAndProcessor();
		
		char vendor[32], processor[32];
		bigtime_t prev_time;
		ProgressBar **progress_bars;
		CPUButton **cpu_buttons;
		BDragger **draggers;
		BBitmap *cpu_logo;
		PriorityBar * priority_bar;
};

#endif
