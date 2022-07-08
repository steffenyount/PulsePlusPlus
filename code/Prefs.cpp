//****************************************************************************************
//
//	File:		Prefs.cpp
//
//	Written by:	Daniel Switkin
//
//	Copyright 1999, Be Incorporated
//
//	Revised by: Steffen Yount
//
//****************************************************************************************

#include "Prefs.h"
#include "Common.h"
#include "PulseApp.h"
#include <storage/FindDirectory.h>
#include <storage/Path.h>
#include <interface/Screen.h>
#include <kernel/OS.h>
#include <stdio.h>
#include <string.h>



Prefs::Prefs(BMessage *archive) {
	SetValue(archive);
}

Prefs::Prefs(bool all) {
	fatalerror = false;
	BPath path;
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append("Pulse++_settings");
	file = new BFile(path.Path(), B_READ_WRITE | B_CREATE_FILE);
	BMessage * m = new BMessage();	
	m->Unflatten(file);
	SetValue(m);
	delete m;
	// These two prefs require a connection to the app_server,
	// and so are only called when all is true
	if (all) {
		if (normal_window_rect == BRect(0,0,0,0))
			normal_window_rect = GetDefaultNormalWindowRect();
		Set_Normal_Window_Rect();

		if (mini_window_rect == BRect(0,0,0,0))	
			mini_window_rect = GetDefaultMiniWindowRect();
	}
}

void Prefs::SetValue(BMessage * archive) {
	if (archive->FindInt32("mode", &mode) != B_OK) 
		mode = NORMAL_WINDOW;
	if (archive->FindInt32("deskbar_bar_width", &deskbar_bar_width) != B_OK) 
		deskbar_bar_width = DEFAULT_DESKBAR_BAR_WIDTH;
	
	// these two need app_server to set proper defaults
	if (archive->FindRect("normal_window_rect", &normal_window_rect) != B_OK) 
		normal_window_rect = BRect(0,0,0,0);
	if (archive->FindRect("mini_window_rect", &mini_window_rect) != B_OK) 
		mini_window_rect = BRect(0,0,0,0);
		
	if (archive->FindRect("prefs_window_rect", &prefs_window_rect) != B_OK) 
		prefs_window_rect = BRect(100, 100, 415, 349);
	if (archive->FindBool("normal_fade_colors", &normal_fade_colors) != B_OK)
		normal_fade_colors = DEFAULT_NORMAL_FADE_COLORS;
	for (int i = 0; i < MODE_COUNT; i++) {
		char temp[80];
		sprintf(temp, "%s %s", STR_MODE_NAMES[i], "Do Standard Display");
		if (archive->FindBool(temp, &modeset[i].do_display.standard) != B_OK)
			modeset[i].do_display.standard = DEFAULT_DO_STANDARD_DISPLAY;
		sprintf(temp, "%s %s", STR_MODE_NAMES[i], "Do Priority Display");	
		if (archive->FindBool(temp, &modeset[i].do_display.priority) != B_OK)
			modeset[i].do_display.priority = DEFAULT_DO_PRIORITY_DISPLAY;
		for (int j = 0; j < COLOR_COUNT; j++) {
			sprintf(temp, "%s %s", STR_MODE_NAMES[i], STR_COLOR_NAMES[j]);
			if (archive->FindInt32(temp, &modeset[i].color[j]) != B_OK)
				modeset[i].color[j] = DEFAULT_COLORS[j];
		}
	}
}

status_t Prefs::Archive(BMessage *archive, bool deep) { 
// status_t Prefs::Archive(BMessage *archive, bool deep = true) { 
	status_t err = B_OK;
	err = BArchivable::Archive(archive, deep);
	err |= archive->AddString("class", PREFS_CLASS_NAME); 
	err |= archive->AddInt32("mode", mode);
	err |= archive->AddInt32("deskbar_bar_width", deskbar_bar_width);
	err |= archive->AddRect("normal_window_rect", normal_window_rect);
	err |= archive->AddRect("mini_window_rect", mini_window_rect);
	err |= archive->AddRect("prefs_window_rect", prefs_window_rect);
	err |= archive->AddBool("normal_fade_colors", normal_fade_colors);
	for (int i = 0; i < MODE_COUNT; i++) {
		char temp[80];
		sprintf(temp, "%s %s", STR_MODE_NAMES[i], "Do Standard Display");
		err |= archive->AddBool(temp, modeset[i].do_display.standard);
		sprintf(temp, "%s %s", STR_MODE_NAMES[i], "Do Priority Display");
		err |= archive->AddBool(temp, modeset[i].do_display.priority);
		for (int j = 0; j < COLOR_COUNT; j++) {
			sprintf(temp, "%s %s", STR_MODE_NAMES[i], STR_COLOR_NAMES[j]);
			err |= archive->AddInt32(temp, modeset[i].color[j]);
		}
	}
	return err;
}

BArchivable * Prefs::Instantiate(BMessage *archive) { 
	if ( validate_instantiation(archive, PREFS_CLASS_NAME)) 
		return new Prefs(archive); 
	return NULL; 
}



int32 Prefs::Max_Bar_Count() {
	return CPU_Count() + 1;
}

int32 Prefs::Bar_Count(int mode) {
	return CPU_Count() * modeset[mode].do_display.standard + 
		modeset[mode].do_display.priority;
}

int32 Prefs::Bar_Count() { 
	return Bar_Count(mode);
}

int32 Prefs::Deskbar_Icon_Width() {
	return Bar_Count(DESKBAR) * deskbar_bar_width + 1;
}

void Prefs::Set_Normal_Window_Rect() {	
	float height = PROGRESS_MTOP + PROGRESS_MBOTTOM + 
		Bar_Count(NORMAL_WINDOW) * ITEM_OFFSET;
	if (PULSEVIEW_MIN_HEIGHT > height) {
		height = PULSEVIEW_MIN_HEIGHT;
	}
	
	normal_window_rect.Set(normal_window_rect.left, normal_window_rect.top, 
		normal_window_rect.left + PULSEVIEW_WIDTH, normal_window_rect.top + height);
}

BRect Prefs::GetDefaultNormalWindowRect() {	
	float height = PROGRESS_MTOP + PROGRESS_MBOTTOM + 
		Bar_Count(NORMAL_WINDOW) * ITEM_OFFSET;
	if (PULSEVIEW_MIN_HEIGHT > height) {
		height = PULSEVIEW_MIN_HEIGHT;
	}
	
	// Dock the window in the lower right hand corner just like the original
	BRect r(0, 0, PULSEVIEW_WIDTH, height);
	BRect screen_rect = BScreen(B_MAIN_SCREEN_ID).Frame();
	r.OffsetTo(screen_rect.right - r.Width() - 10, screen_rect.bottom - r.Height() - 30);
	return r;
}

BRect Prefs::GetDefaultMiniWindowRect() {
	// Lower right hand corner by default
	BRect screen_rect = BScreen(B_MAIN_SCREEN_ID).Frame();
	screen_rect.left = screen_rect.right - 30;
	screen_rect.top = screen_rect.bottom - 150;
	screen_rect.OffsetBy(-10, -30);
	return screen_rect;
}

bool Prefs::Save() {
	file->Seek(0, SEEK_SET);
	BMessage * m = new BMessage();
	Archive(m);	
	m->Flatten(file);
	delete m;
	return true;
}

Prefs::~Prefs() {
	delete file;
}