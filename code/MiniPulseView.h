//****************************************************************************************
//
//	File:		MiniPulseView.h
//
//	Written by:	Arve Hjonnevag and Daniel Switkin
//
//	Copyright 1999, Be Incorporated
//
//	Revised by: Steffen Yount
//
//****************************************************************************************

#ifndef MINIPULSEVIEW_H
#define MINIPULSEVIEW_H

#include "PulseView.h"
#include "Prefs.h"

class MiniPulseView : public PulseView {
	public:
		MiniPulseView(BRect rect, const char *name, Prefs *prefs);
		MiniPulseView(BRect rect, const char *name);
		MiniPulseView(BMessage *message);
		~MiniPulseView();
		void Draw(BRect rect);
		void AttachedToWindow();
		void Pulse();
		void FrameResized(float width, float height);
		
	protected:
		BMenuItem *quit;
};

#endif
