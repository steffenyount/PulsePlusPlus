//****************************************************************************************
//
//	File:		MiniPulseView.cpp
//
//	Written by:	Arve Hjonnevag and Daniel Switkin
//
//	Copyright 1999, Be Incorporated
//
//	Revised by: Steffen Yount
//
//****************************************************************************************

#include "MiniPulseView.h"
#include "Common.h"
#include <interface/Window.h>

MiniPulseView::MiniPulseView(BRect rect, const char *name, Prefs *prefs) :
	PulseView(rect, name) {
	this->prefs = prefs;
	
	mode1->SetLabel("Normal Mode");
	mode1->SetMessage(new BMessage(PV_NORMAL_MODE));
	mode2->SetLabel("Deskbar Mode");
	mode2->SetMessage(new BMessage(PV_DESKBAR_MODE));
	quit = new BMenuItem("Quit", new BMessage(PV_QUIT), 0, 0);
	popupmenu->AddSeparatorItem();
	popupmenu->AddItem(quit);
	
	// Our drawing covers every pixel in the view, so no reason to
	// take the time (and to flicker) by resetting the view color
	SetViewColor(B_TRANSPARENT_COLOR);
		
}

// These two are only used by DeskbarPulseView, and so do nothing
MiniPulseView::MiniPulseView(BRect rect, const char *name) : PulseView(rect, name) {

}

MiniPulseView::MiniPulseView(BMessage *message) : PulseView(message) {

}

// This method is used by DeskbarPulseView as well
void MiniPulseView::Draw(BRect rect) {
	
	SetDrawingMode(B_OP_COPY);

	int h = rect.IntegerHeight() - 2;
	float top = 1, left = 1;
	float bottom = top + h;
	float bar_width = (rect.Width()) / prefs->Bar_Count() - 2;
	float right = bar_width + left;
	ModeSettings * ms = &(prefs->modeset[prefs->mode]);
	
	if (prefs->Do_priority_display()) {
		float vert_len, bar_height = bottom;
		float multiplyer = ((float)(h+1))/((float)CPU_Count());
		for (int i = 0; i < IDLE_PRIORITY; i++) {
			vert_len = priority_times[i] * multiplyer;
			SetHighColor(ms->Red(i), ms->Green(i), ms->Blue(i));
			if (bar_height - vert_len > top) {
				FillRect(BRect(left, bar_height - vert_len, right, bar_height));
			} else {
				if (bar_height > top)
					FillRect(BRect(left, top, right, bar_height));
			}
			bar_height -= vert_len;
		}
		if (bar_height > top) {
			SetHighColor(ms->Red(IDLE_PRIORITY), 
				ms->Green(IDLE_PRIORITY), ms->Blue(IDLE_PRIORITY));
			FillRect(BRect(left, top, right, bar_height));	
		}
		left += bar_width + 2;
		right += bar_width + 2;
	}
	
	if (prefs->Do_standard_display()) {
		for (int x = 0; x < CPU_Count(); x++) {
			int bar_height = (int)(cpu_times[x] * (h + 1));
			if (bar_height > h) bar_height = h;
			double rem = cpu_times[x] * (h + 1) - bar_height;

			rgb_color fraction_color;
			fraction_color.red = (uint8)(ms->Red(PROC_IDLE) + 
				rem * (ms->Red(PROC_ACTIVE) - ms->Red(PROC_IDLE)));
			fraction_color.green = (uint8)(ms->Green(PROC_IDLE) + 
				rem * (ms->Green(PROC_ACTIVE) - ms->Green(PROC_IDLE)));
			fraction_color.blue = (uint8)(ms->Blue(PROC_IDLE) + 
				rem * (ms->Blue(PROC_ACTIVE) - ms->Blue(PROC_IDLE)));
			fraction_color.alpha = 0xff;

			SetHighColor(ms->Red(FRAME), ms->Green(FRAME), ms->Blue(FRAME));
			StrokeLine(BPoint(left - 1, top - 1), BPoint(left - 1, bottom + 1));

			int idle_height = h - bar_height;
			if (idle_height > 0) {
				SetHighColor(ms->Red(PROC_IDLE), 
					ms->Green(PROC_IDLE), ms->Blue(PROC_IDLE));
				FillRect(BRect(left, top, right, top + idle_height - 1));
			}
			SetHighColor(fraction_color);
			FillRect(BRect(left, bottom - bar_height, right, bottom - bar_height));
			if (bar_height > 0) {
				SetHighColor(ms->Red(PROC_ACTIVE), 
					ms->Green(PROC_ACTIVE), ms->Blue(PROC_ACTIVE));
				FillRect(BRect(left, bottom - bar_height + 1, right, bottom));
			}
			left += bar_width + 2;
			right += bar_width + 2;
		}
	}

	SetHighColor(ms->Red(FRAME), ms->Green(FRAME), ms->Blue(FRAME));
	if (!prefs->Do_priority_display() && !prefs->Do_standard_display()) {
		FillRect(rect);
	} else {
		StrokeRect(rect);
	}
}

void MiniPulseView::Pulse() {
	// Don't recalculate and redraw if this view is hidden
	if (!IsHidden()) {
		Update();
		Draw(Bounds());
	}
}

void MiniPulseView::FrameResized(float width, float height) {
	Draw(Bounds());
}

void MiniPulseView::AttachedToWindow() {
	BMessenger messenger(Window());
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
}


MiniPulseView::~MiniPulseView() {

}