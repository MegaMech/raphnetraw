/* mupen64plus-input-raphnetraw
 *
 * Copyright (C) 2016 Raphael Assenat
 *
 * An input plugin that lets the game under emulation communicate with
 * the controllers directly using the direct controller communication
 * feature of raphnet V3 adapters[1].
 *
 * [1] http://www.raphnet.net/electronique/gcn64_usb_adapter_gen3/index_en.php
 *
 * Based on the Mupen64plus-input-sdl plugin (original banner below)
 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus-input-sdl - plugin.c                                      *
 *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
 *   Copyright (C) 2008-2011 Richard Goedeken                              *
 *   Copyright (C) 2008 Tillin9                                            *
 *   Copyright (C) 2002 Blight                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//#define M64P_PLUGIN_PROTOTYPES 1
//#include "config.h"
// #include "m64p_common.h"
// #include "m64p_config.h"
// #include "m64p_plugin.h"
// #include "m64p_types.h"
//#include "osal_dynamiclib.h"
#include "plugin_front.h"
#include "plugin_back.h"
#include "version.h"


#ifdef __linux__
#endif /* __linux__ */

#include <errno.h>

#define MAX_CONTROLLERS	4

#ifdef PORTS_1_AND_4
static int emu2adap_portmap[MAX_CONTROLLERS] = { 0, 2, 3, 1 };
#undef PLUGIN_NAME
#define PLUGIN_NAME "raphnetraw ports 1 and 4"
#else
static int emu2adap_portmap[MAX_CONTROLLERS] = { 0, 1, 2, 3 };
#endif

#define EMU_2_ADAP_PORT(a)	((a) == -1 ? -1 : emu2adap_portmap[a])

#if 0
/* definitions of pointers to Core config functions */
ptr_ConfigOpenSection      ConfigOpenSection = NULL;
ptr_ConfigDeleteSection    ConfigDeleteSection = NULL;
ptr_ConfigSaveSection      ConfigSaveSection = NULL;
ptr_ConfigListParameters   ConfigListParameters = NULL;
ptr_ConfigSaveFile         ConfigSaveFile = NULL;
ptr_ConfigSetParameter     ConfigSetParameter = NULL;
ptr_ConfigGetParameter     ConfigGetParameter = NULL;
ptr_ConfigGetParameterHelp ConfigGetParameterHelp = NULL;
ptr_ConfigSetDefaultInt    ConfigSetDefaultInt = NULL;
ptr_ConfigSetDefaultFloat  ConfigSetDefaultFloat = NULL;
ptr_ConfigSetDefaultBool   ConfigSetDefaultBool = NULL;
ptr_ConfigSetDefaultString ConfigSetDefaultString = NULL;
ptr_ConfigGetParamInt      ConfigGetParamInt = NULL;
ptr_ConfigGetParamFloat    ConfigGetParamFloat = NULL;
ptr_ConfigGetParamBool     ConfigGetParamBool = NULL;
ptr_ConfigGetParamString   ConfigGetParamString = NULL;

ptr_ConfigGetSharedDataFilepath ConfigGetSharedDataFilepath = NULL;
ptr_ConfigGetUserConfigPath     ConfigGetUserConfigPath = NULL;
ptr_ConfigGetUserDataPath       ConfigGetUserDataPath = NULL;
ptr_ConfigGetUserCachePath      ConfigGetUserCachePath = NULL;
#endif

/* static data definitions */
static void (*l_DebugCallback)(void *, int, const char *) = NULL;
static void *l_DebugCallContext = NULL;
static int l_PluginInit = 0;

/* Global functions */
static void DebugMessage(int level, const char *message, ...)
{
	char msgbuf[1024];
	va_list args;

	if (!l_DebugCallback) {
		printf("No debug callback!\n");
		return;
	}

	va_start(args, message);
	vsnprintf(msgbuf, sizeof(msgbuf), message, args);

	(*l_DebugCallback)(l_DebugCallContext, level, msgbuf);

	va_end(args);
}

void MyDebugCallback(void *ctx, int level, const char *msg)
{
    (void)ctx; // unused
    printf("[%d] %s\n", level, msg);
}

void RaphnetInitialize(void) {
    l_DebugCallback = MyDebugCallback;
	  pb_init(DebugMessage);
    l_PluginInit = 1;
}

void RaphnetShutdown(void) {
    if (!l_PluginInit) {
		return;
	}

	/* reset some local variables */
	l_DebugCallback = NULL;
	l_DebugCallContext = NULL;

	pb_shutdown();

    l_PluginInit = 0;
}

int ScanControllers(void) {
	int num = pb_scanControllers();

	pb_romOpen();

  printf("Found %d controllers\n", num);

	return num;
}

/******************************************************************
  Function: InitiateControllers
  Purpose:  This function initialises how each of the controllers
            should be handled.
  input:    - The handle to the main window.
            - A controller structure that needs to be filled for
              the emulator to know how to handle each controller.
  output:   none
*******************************************************************/
void InitiateControllers(CONTROL_INFO ControlInfo)
{
}


/******************************************************************
  Function: ReadController
  Purpose:  To process the raw data in the pif ram that is about to
            be read.
  input:    - Controller Number (0 to 3) and -1 signalling end of
              processing the pif ram.
            - Pointer of data to be processed.
  output:   none
  note:     This function is only needed if the DLL is allowing raw
            data.
*******************************************************************/
void ReadController(int Control, unsigned char *Command)
{
	pb_readController(EMU_2_ADAP_PORT(Control), Command);
}

void ControllerCommand(int Control, unsigned char *Command)
{
	pb_controllerCommand(EMU_2_ADAP_PORT(Control), Command);
}

void RomClosed(void)
{
	pb_romClosed();
}

int RomOpen(void)
{
	pb_romOpen();
	return 1;
}
