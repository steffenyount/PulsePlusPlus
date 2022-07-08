//****************************************************************************************
//
//	File:		PulseView.h
//
//	Written by:	David Ramsey and Daniel Switkin
//
//	Copyright 1999, Be Incorporated
//
//	Improved by: Steffen Yount
//
//****************************************************************************************

#ifndef PULSEVIEW_H
#define PULSEVIEW_H

#include <interface/View.h>
#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>
#include "Common.h"


extern "C" int  _kget_cpu_state_(int cpu);
extern "C" int  _kset_cpu_state_(int cpu, int enabled);

int32 Update_thread(void * pulse_view);

class PulseView : public BView {
	public:
		PulseView(BRect rect, const char *name);
		PulseView(BMessage *message);
		~PulseView();
		virtual void MouseDown(BPoint point);
		void ChangeCPUState(BMessage *message);
		
	protected:
		void Init();
		void Update();
		inline int Bar_count();	
			
		BPopUpMenu *popupmenu;
		BMenuItem *mode1, *mode2, *preferences, *about;
		BMenuItem **cpu_menu_items;
		
		double cpu_times[B_MAX_CPU_COUNT];
		bigtime_t prev_active[B_MAX_CPU_COUNT];
		bigtime_t prev_time;
		
		system_info sys_info;

		float priority_times[PRIORITY_COUNT];
		bigtime_t prev_priority_active[PRIORITY_COUNT];

		sem_id need_update;
		bool keep_running;
		
		friend int32 Update_thread(void * pulse_view);
		
};
#endif