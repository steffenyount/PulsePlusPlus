//****************************************************************************************
//
//	File:		Prefs.h
//
//	Written by:	Daniel Switkin
//
//	Copyright 1999, Be Incorporated
//
//	Revised by: Steffen Yount
//
//****************************************************************************************

#ifndef PREFS_H
#define PREFS_H

#include <File.h>
#include <Rect.h>
#include <Archivable.h>
#include <Message.h>
#include <interface/GraphicsDefs.h>
#include "Common.h"

#define PREFS_CLASS_NAME	"Pulse++ Prefs"

typedef struct {
	bool standard;
	bool priority;
} Do_Display;

class  ModeSettings {
	public:
		inline uint8 Red(int32 colorindex) { 
			return (uint8)(color[colorindex] >> 24); }
		inline uint8 Green(int32 colorindex) { 
			return (uint8)(color[colorindex] >> 16); }
		inline uint8 Blue(int32 colorindex) { 
			return (uint8)(color[colorindex] >> 8); }
		int32 color[COLOR_COUNT];
		Do_Display do_display;		
};

class Prefs : BArchivable {
	public:
		Prefs(BMessage *archive);
		Prefs(bool all);
		void SetValue(BMessage * archive);
		bool Save();
		~Prefs();
		status_t Archive(BMessage *archive, bool deep = true);
		BArchivable * Instantiate(BMessage *archive);
		
		int32 Max_Bar_Count();
		int32 Bar_Count(int mode);
		int32 Bar_Count();
		void Set_Normal_Window_Rect();
		int32 Deskbar_Icon_Width();
		inline bool Do_standard_display() { 
			return modeset[mode].do_display.standard; }
		inline bool Do_priority_display() { 
			return modeset[mode].do_display.priority; }

		int32 mode, deskbar_bar_width;
		BRect normal_window_rect, mini_window_rect, prefs_window_rect;
		bool normal_fade_colors;
		
		ModeSettings modeset[MODE_COUNT];

		bool fatalerror;
		
	private:

		BFile *file;
		
		BRect GetDefaultMiniWindowRect();
		BRect GetDefaultNormalWindowRect();
};

#endif
