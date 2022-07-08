//****************************************************************************************
//
//	File:		PulseView.cpp
//
//	Written by:	David Ramsey and Daniel Switkin
//
//	Copyright 1999, Be Incorporated
//
//	Revised by: Steffen Yount
//
//****************************************************************************************

#include "PulseView.h"
#include "Common.h"
#include "PulseApp.h"
#include <interface/Alert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

PulseView::PulseView(BRect rect, const char *name) :
	BView(rect, name, B_FOLLOW_ALL_SIDES, B_FULL_UPDATE_ON_RESIZE | 
		B_WILL_DRAW | B_PULSE_NEEDED | B_FRAME_EVENTS) {

	popupmenu = NULL;
	cpu_menu_items = NULL;

	// Don't init the menus for the DeskbarPulseView, because this instance
	// will only be used to archive the replicant
	if (strcmp(name, "DeskbarPulseView") != 0) {
		Init();
	}
}

// This version will be used by the instantiated replicant
PulseView::PulseView(BMessage *message) : BView(message) {
	SetResizingMode(B_FOLLOW_ALL_SIDES);
	SetFlags(B_WILL_DRAW | B_PULSE_NEEDED);
	
	prefs = NULL;	
	popupmenu = NULL;
	cpu_menu_items = NULL;
	Init();
}

void PulseView::Init() {
	popupmenu = new BPopUpMenu("PopUpMenu", false, false, B_ITEMS_IN_COLUMN);
	popupmenu->SetFont(be_plain_font);
	mode1 = new BMenuItem("", NULL, 0, 0);
	mode2 = new BMenuItem("", NULL, 0, 0);
	preferences = new BMenuItem("Preferences...", new BMessage(PV_PREFERENCES), 0, 0);
	about = new BMenuItem("About...", new BMessage(PV_ABOUT), 0, 0);
	
	popupmenu->AddItem(mode1);
	popupmenu->AddItem(mode2);
	popupmenu->AddSeparatorItem();
		
	// Only add menu items to control CPUs on an SMP machine
	if (CPU_Count() >= 2) {
		cpu_menu_items = new BMenuItem *[CPU_Count()];
		char temp[20];
		for (int x = 0; x < CPU_Count(); x++) {
			sprintf(temp, "CPU %d", x + 1);
			BMessage *message = new BMessage(PV_CPU_MENU_ITEM);
			message->AddInt32("which", x);
			cpu_menu_items[x] = new BMenuItem(temp, message, 0, 0);
			popupmenu->AddItem(cpu_menu_items[x]);
		}
		popupmenu->AddSeparatorItem();
	}
	
	popupmenu->AddItem(preferences);
	popupmenu->AddItem(about);
}

void PulseView::MouseDown(BPoint point) {
	BPoint cursor;
	uint32 buttons;
	MakeFocus(true);
	GetMouse(&cursor, &buttons, true);
	
	if (buttons & B_SECONDARY_MOUSE_BUTTON) {
		ConvertToScreen(&point);
		// Use the asynchronous version so we don't interfere with
		// the window responding to Pulse() events
		popupmenu->Go(point, true, false, true);
	}
}

void PulseView::Update() {
	bigtime_t now = system_time();

	if (prefs->Do_priority_display()) {
		// Calculate work done per priority since last run
		bigtime_t priority_active[PRIORITY_COUNT];
		team_info team_inf; 
		int32 team_cookie = 0; 
		thread_info thread_inf; 
		int32 thread_cookie; 
		for (int i = 0; i < PRIORITY_COUNT; i++)
			priority_active[i] = 0;
		while (get_next_team_info(&team_cookie, &team_inf) == B_OK) {
			thread_cookie = 0;
			while (get_next_thread_info(team_inf.team, &thread_cookie, &thread_inf) == B_OK) {
				if (thread_inf.priority >= B_URGENT_DISPLAY_PRIORITY) {
					if (thread_inf.priority >= B_URGENT_PRIORITY) {
						if (thread_inf.priority >= B_REAL_TIME_PRIORITY) {
							priority_active[0] += thread_inf.user_time + 
								thread_inf.kernel_time;
						} else {
							priority_active[1] += thread_inf.user_time + 
								thread_inf.kernel_time;
						}
					} else {
						if (thread_inf.priority >= B_REAL_TIME_DISPLAY_PRIORITY) {
							priority_active[2] += thread_inf.user_time + 
								thread_inf.kernel_time;
						} else {
							priority_active[3] += thread_inf.user_time + 
								thread_inf.kernel_time;
						}
					}
				} else {
					if (thread_inf.priority >= B_NORMAL_PRIORITY) {
						if (thread_inf.priority >= B_DISPLAY_PRIORITY) {
							priority_active[4] += thread_inf.user_time + 
								thread_inf.kernel_time;
						} else {
							priority_active[5] += thread_inf.user_time + 
								thread_inf.kernel_time;
						}
					} else {
						if (thread_inf.priority > 0) {
							priority_active[6] += thread_inf.user_time + 
								thread_inf.kernel_time;
						}
					}
				}
		
			}
		} 
		for (int i = 0; i < PRIORITY_COUNT; i++)
		{
			float prev_priority_times;
			prev_priority_times = priority_times[i];
			priority_times[i] = (float)(priority_active[i] - 
				prev_priority_active[i]) / (now - prev_time);
			if (priority_times[i] < 0) 
				priority_times[i] = prev_priority_times;
			if (priority_times[i] > CPU_Count()) 
				priority_times[i] = prev_priority_times;
			prev_priority_active[i] = priority_active[i];
		}
	}

	if (prefs->Do_standard_display()) {
		// Calculate work done since last run for each CPU
		for (int x = 0; x < CPU_Count(); x++) {
			system_info sys_inf;
			get_system_info(&sys_inf);
			double cpu_time = (double)(sys_inf.cpu_infos[x].active_time - 
				prev_active[x]) / (now - prev_time);
			prev_active[x] = sys_inf.cpu_infos[x].active_time;
			if (cpu_time < 0) cpu_time = 0;
			if (cpu_time > 1) cpu_time = 1;
			cpu_times[x] = cpu_time;
			
			if (CPU_Count() >= 2) {
				if (!_kget_cpu_state_(x) && cpu_menu_items[x]->IsMarked())
					cpu_menu_items[x]->SetMarked(false);
				if (_kget_cpu_state_(x) && !cpu_menu_items[x]->IsMarked())
					cpu_menu_items[x]->SetMarked(true);
			}
		}
	}
		
	prev_time = now;
}

void PulseView::ChangeCPUState(BMessage *message) {
	int which = message->FindInt32("which");
	
	if (!LastEnabledCPU(which)) {
		_kset_cpu_state_(which, (int)!cpu_menu_items[which]->IsMarked());
	} else {
		BAlert *alert = new BAlert(NULL, "You can't disable the last active CPU.", "OK");
		alert->Go(NULL);
	}
}

PulseView::~PulseView() {
	if (popupmenu != NULL) delete popupmenu;
	if (cpu_menu_items != NULL) delete cpu_menu_items;
}