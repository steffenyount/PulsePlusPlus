//****************************************************************************************
//
//	File:		PulseView.cpp
//
//	Written by:	David Ramsey and Daniel Switkin
//
//	Copyright 1999, Be Incorporated
//
//	Improved by: Steffen Yount
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
	BView(rect, name, B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_PULSE_NEEDED | B_FRAME_EVENTS) {

	get_system_info(&sys_info);

	// get update thread going
	if ((need_update = create_sem(0,"need_update")) < B_OK)
		printf("Error creating update semaphore!!!\n");
	keep_running = true;
	spawn_thread(Update_thread, "Update_thread", B_NORMAL_PRIORITY, this); 
		
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
	
	get_system_info(&sys_info);

	// get update thread going
	if ((need_update = create_sem(0,"need_update")) < B_OK)
		printf("Error creating update semaphore!!!\n");
	keep_running = true;
	spawn_thread(Update_thread, "Update_thread", B_NORMAL_PRIORITY, this); 
	
	popupmenu = NULL;
	cpu_menu_items = NULL;
	Init();
}

inline int PulseView::Bar_count() {
	return (((PulseApp *)be_app)->prefs->do_priority_display + 
		sys_info.cpu_count * ((PulseApp *)be_app)->prefs->do_standard_display);
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
	if (sys_info.cpu_count >= 2) {
		cpu_menu_items = new BMenuItem *[sys_info.cpu_count];
		char temp[20];
		for (int x = 0; x < sys_info.cpu_count; x++) {
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
/*	int32 thread_count;
 	get_sem_count(need_update, &thread_count);
	if (thread_count <= 0) // avoids flooding with duplicate update requests
*/		release_sem(need_update);
}

int32 Update_thread(void * pulse_view) {
	PulseView * pv = (PulseView *)pulse_view;	
	Prefs * prefs = ((PulseApp *)be_app)->prefs;
	system_info sys_info;
	get_system_info(&(sys_info));
	
	while(pv->keep_running) {
		bigtime_t now = system_time();

		if (prefs->do_standard_display) {
			// Calculate work done since last run for each CPU

			get_system_info(&(sys_info));
			for (int x = 0; x < sys_info.cpu_count; x++) {
				double cpu_time = (double)(sys_info.cpu_infos[x].active_time - pv->prev_active[x]) / (now - pv->prev_time);
				pv->prev_active[x] = sys_info.cpu_infos[x].active_time;
				if (cpu_time < 0) cpu_time = 0;
				if (cpu_time > 1) cpu_time = 1;
				pv->cpu_times[x] = cpu_time;
			
				if (sys_info.cpu_count >= 2) {
					if (!_kget_cpu_state_(x) && pv->cpu_menu_items[x]->IsMarked())
						pv->cpu_menu_items[x]->SetMarked(false);
					if (_kget_cpu_state_(x) && !pv->cpu_menu_items[x]->IsMarked())
						pv->cpu_menu_items[x]->SetMarked(true);
				}
			}
		}
		
		if (prefs->do_priority_display) {
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
				pv->priority_times[i] = (float)(priority_active[i] - pv->prev_priority_active[i]) / (now - pv->prev_time);
				if (pv->priority_times[i] < 0) pv->priority_times[i] = 0;
				if (pv->priority_times[i] > sys_info.cpu_count) pv->priority_times[i] = sys_info.cpu_count;
				pv->prev_priority_active[i] = priority_active[i];
			}
		}
		
		pv->prev_time = now;
		acquire_sem(pv->need_update);
	}
	return B_OK;
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
	keep_running = false;
	delete_sem(need_update);
}