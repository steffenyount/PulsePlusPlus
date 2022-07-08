//****************************************************************************************
//
//	File:		DeskbarPulseView.h
//
//	Written by:	Daniel Switkin
//
//	Copyright 1999, Be Incorporated
//
//	Revised by: Steffen Yount
//
//****************************************************************************************

#ifndef DESKBARPULSEVIEW_H
#define DESKBARPULSEVIEW_H

#include "MiniPulseView.h"
#include "PrefsWindow.h"
#include <app/MessageRunner.h>

class DeskbarPulseView : public MiniPulseView {
	public:
		DeskbarPulseView(BRect rect);
		DeskbarPulseView(BMessage *message);
		virtual ~DeskbarPulseView();	//made virtual
		void AttachedToWindow();
		void Pulse();
		
		void MessageReceived(BMessage *message);
		static DeskbarPulseView *Instantiate(BMessage *data);
		virtual	status_t Archive(BMessage *data, bool deep = true) const;
		
	private:
		void Remove();
		void SetMode(bool normal);
		
		PrefsWindow *prefswindow;
		BMessageRunner *messagerunner;
};

#endif