/*
** Enhanced heads up 'overlay' for fullscreen
**
**---------------------------------------------------------------------------
** Copyright 2003-2008 Christoph Oelckers
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
*/

#include "v_video.h"
#include "gi.h"
#include "filesystem.h"
#include "sc_man.h"
#include "d_net.h"
#include "cmdlib.h"
#include "vm.h"
#include "v_draw.h"
#include "printf.h"
#include "v_font.h"
#include "mapinfo.h"
#include "base_sbar.h"

#include <time.h>


CVAR(Int,hud_althudscale, 0, CVAR_ARCHIVE)				// Scale the hud to 640x400?
//CVAR(Bool,hud_althud, false, CVAR_ARCHIVE)				// Enable/Disable the alternate HUD

CVAR(Int, am_showtotaltime, 0, CVAR_ARCHIVE);	// Show time on automap
CVAR(Int, am_showtime, 0, CVAR_ARCHIVE);	// Show time on automap

														// These are intentionally not the same as in the automap!
CVAR (Bool,  hud_showsecrets,	true,CVAR_ARCHIVE);		// Show secrets on HUD
CVAR (Bool,  hud_showkills,		true,CVAR_ARCHIVE);		// Show monster stats on HUD
CVAR (Bool,  hud_showstats,		false,	CVAR_ARCHIVE);	// for stamina and accuracy. 
CVAR (Bool,  hud_showscore,		false,	CVAR_ARCHIVE);	// for user maintained score
CVAR (Bool,  hud_showweapons,	true, CVAR_ARCHIVE);	// Show weapons collected
CVAR (Int ,  hud_showammo,		2, CVAR_ARCHIVE);		// Show ammo collected
CVAR (Int ,  hud_showtime,		0,	    CVAR_ARCHIVE);	// Show time on HUD
CVAR (Int ,  hud_showtimestat,	0,	    CVAR_ARCHIVE);	// Show time on HUD as statistics widget
CVAR (Int ,  hud_timecolor,		CR_GOLD,CVAR_ARCHIVE);	// Color of in-game time on HUD

CVAR (Int, hud_ammo_order, 0, CVAR_ARCHIVE);				// ammo image and text order
CVAR (Int, hud_ammo_red, 25, CVAR_ARCHIVE)					// ammo percent less than which status is red    
CVAR (Int, hud_ammo_yellow, 50, CVAR_ARCHIVE)				// ammo percent less is yellow more green        
CVAR (Int, hud_health_red, 25, CVAR_ARCHIVE)				// health amount less than which status is red   
CVAR (Int, hud_health_yellow, 50, CVAR_ARCHIVE)				// health amount less than which status is yellow
CVAR (Int, hud_health_green, 100, CVAR_ARCHIVE)				// health amount above is blue, below is green   
CVAR (Int, hud_armor_red, 25, CVAR_ARCHIVE)					// armor amount less than which status is red    
CVAR (Int, hud_armor_yellow, 50, CVAR_ARCHIVE)				// armor amount less than which status is yellow 
CVAR (Int, hud_armor_green, 100, CVAR_ARCHIVE)				// armor amount above is blue, below is green    

CVAR (Bool, hud_berserk_health, true, CVAR_ARCHIVE);		// when found berserk pack instead of health box
CVAR (Bool, hud_showangles, false, CVAR_ARCHIVE)			// show player's pitch, yaw, roll

CVAR (Int, hudcolor_titl, CR_YELLOW, CVAR_ARCHIVE)			// color of automap title
CVAR (Int, hudcolor_time, CR_RED, CVAR_ARCHIVE)				// color of level/hub time
CVAR (Int, hudcolor_ltim, CR_ORANGE, CVAR_ARCHIVE)			// color of single level time
CVAR (Int, hudcolor_ttim, CR_GOLD, CVAR_ARCHIVE)			// color of total time
CVAR (Int, hudcolor_xyco, CR_GREEN, CVAR_ARCHIVE)			// color of coordinates

CVAR (Int, hudcolor_statnames, CR_RED, CVAR_ARCHIVE)		// For the letters before the stats
CVAR (Int, hudcolor_stats, CR_GREEN, CVAR_ARCHIVE)			// For the stats values themselves


CVAR(Bool, map_point_coordinates, true, CVAR_ARCHIVE|CVAR_GLOBALCONFIG)	// show player or map coordinates?

DObject* AltHud;
extern DStatusBarCore* StatusBar;

//---------------------------------------------------------------------------
//
// Create Alternative HUD
//
//---------------------------------------------------------------------------

/*
CUSTOM_CVAR(Bool, hud_althud_forceinternal, false, CVAR_ARCHIVE | CVAR_NOINITCALL)
{
	CreateAltHUD();
}
*/

static DObject* DoCreateAltHUD(const FName classname)
{
	if (classname == NAME_None)
		return nullptr;

	const auto cls = PClass::FindClass(classname);
	if (!cls)
	{
		Printf(TEXTCOLOR_RED "Unknown alternative HUD class \"%s\"\n", classname.GetChars());
		return nullptr;
	}

	if (!cls->IsDescendantOf(NAME_AltHud))
	{
		Printf(TEXTCOLOR_RED "Alternative HUD class \"%s\" is not derived from AltHud\n", classname.GetChars());
		return nullptr;
	}

	const auto althud = cls->CreateNew();

	IFVIRTUALPTRNAME(althud, NAME_AltHud, Init)
	{
		VMValue params[] = { althud };
		VMCall(func, params, countof(params), nullptr, 0);
	}

	return althud;
}

void CreateAltHUD()
{
	if (AltHud)
	{
		AltHud->Destroy();
		AltHud = nullptr;
	}

	/*
	if (!hud_althud_forceinternal)
		AltHud = DoCreateAltHUD(gameinfo.althudclass);
	*/

	if (!AltHud)
		AltHud = DoCreateAltHUD(NAME_AltHud);

	assert(AltHud);
	GC::AddMarkerFunc([]() { GC::Mark(AltHud); });
}


//---------------------------------------------------------------------------
//
// draw the HUD
//
//---------------------------------------------------------------------------
EXTERN_CVAR(Bool, hud_aspectscale)
//EXTERN_CVAR(Bool, hud_oldscale)
EXTERN_CVAR(Float, hud_scalefactor)

void DrawAltHUD(SummaryInfo *info)
{
	int hudwidth;
	int hudheight;

	if (!AltHud) CreateAltHUD();

	/*
	if (hud_oldscale)
	{
		int scale = GetUIScale(twod, hud_althudscale);
		hudwidth = twod->GetWidth() / scale;
		hudheight = twod->GetHeight() / scale;
	}
	else
	*/
	{
		hudwidth = int(640 / hud_scalefactor);
		hudheight = hudwidth * twod->GetHeight() / twod->GetWidth();
	}
	if (hud_aspectscale) hudheight = hudheight * 5 / 6;


	IFVIRTUALPTRNAME(AltHud, NAME_AltHud, Draw)
	{
		VMValue params[] = { AltHud, StatusBar, info, hudwidth, hudheight };
		VMCall(func, params, countof(params), nullptr, 0);
	}
}

