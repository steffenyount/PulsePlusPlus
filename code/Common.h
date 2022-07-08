//****************************************************************************************
//
//	File:		Common.h
//
//	Written by:	Daniel Switkin
//
//	Copyright 1999, Be Incorporated
//
//	Revised by: Steffen Yount
//
//****************************************************************************************

#ifndef COMMON_H
#define COMMON_H


enum {
	PULSEVIEW_WIDTH = 263,
	PULSEVIEW_MIN_HEIGHT = 82,
	PROGRESS_MLEFT = 101,
	PROGRESS_MTOP = 20,
	PROGRESS_MBOTTOM = 12,
	CPUBUTTON_MLEFT = 79,
	CPUBUTTON_MTOP = 20,
	CPUBUTTON_WIDTH = 16,
	CPUBUTTON_HEIGHT = 16,
	ITEM_OFFSET = 27
};


enum {
	REAL_TIME,
	URGENT_PRIORITY,
	REAL_TIME_DISPLAY,
	URGENT_DISPLAY,
	DISPLAY_PRIORITY,
	NORMAL_PRIORITY,
	LOW_PRIORITY,
	IDLE_PRIORITY,
	FRAME,
	PROC_IDLE,
	PROC_ACTIVE,
	COLOR_COUNT,
	PRIORITY_COUNT = FRAME
};

extern const char * STR_COLOR_NAMES[];

#define DEFAULT_REAL_TIME_COLOR			0xff5555ff
#define DEFAULT_URGENT_PRIORITY_COLOR	0xffcc00ff
#define DEFAULT_REAL_TIME_DISPLAY_COLOR	0xffff55ff
#define DEFAULT_URGENT_DISPLAY_COLOR	0x00ee00ff
#define DEFAULT_DISPLAY_PRIORITY_COLOR	0x55ffaaff
#define	DEFAULT_NORMAL_PRIORITY_COLOR	0x00aaffff
#define DEFAULT_LOW_PRIORITY_COLOR		0x5555ffff
#define DEFAULT_IDLE_PRIORITY_COLOR		0x4b4b4bff
#define DEFAULT_FRAME_COLOR				0x20202000
#define DEFAULT_PROC_IDLE_COLOR			0x4b4b4bff
#define DEFAULT_PROC_ACTIVE_COLOR		0x00f00000

extern const int DEFAULT_COLORS[];

#define APP_SIGNATURE				"application/x-vnd.BeDTS-Pulse++"

#define PV_NORMAL_MODE				'pvnm'
#define PV_MINI_MODE				'pvmm'
#define PV_DESKBAR_MODE				'pvdm'
#define PV_PREFERENCES				'pvpr'
#define PV_ABOUT					'pvab'
#define PV_QUIT						'pvqt'
#define PV_CPU_MENU_ITEM			'pvcm'
#define PV_REPLICANT_PULSE			'pvrp'


enum {
	NORMAL_WINDOW = 0,
	MINI_WINDOW,
	DESKBAR,
	MODE_COUNT
};

extern const char * STR_MODE_NAMES[]; 

/* 	last byte is to be filled with a value from 0 to MODE_COUNT when \0
	second to last byte is to be filled with a value from 0 to 
	COLOR_COUNT when - */
#define PRV_COLOR_SELECTION_CHANGED	'prcs'
#define PRV_COLOR_VALUE_CHANGED		'prcv'
#define PRV_DO_DISPLAY				'prdd'
#define PRV_NORMAL_FADE_COLORS		'prnf'
#define PRV_DESKBAR_BAR_WIDTH		'prdw'
#define PRV_BOTTOM_OK				'prbo'
#define PRV_BOTTOM_DEFAULTS			'prbd'
#define PRV_QUIT					'prvq'


#define DEFAULT_DESKBAR_BAR_WIDTH		5
#define DESKBAR_BAR_WIDTH_MIN			3
#define DESKBAR_BAR_WIDTH_MAX			8

										
#define DEFAULT_NORMAL_FADE_COLORS		false
#define DEFAULT_DO_STANDARD_DISPLAY		true
#define DEFAULT_DO_PRIORITY_DISPLAY		true


extern const char STR_ABOUT[];
#endif
