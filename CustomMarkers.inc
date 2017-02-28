/*
   _____          _                    __  __            _                 
  / ____|        | |                  |  \/  |          | |                
 | |    _   _ ___| |_ ___  _ __ ___   | \  / | __ _ _ __| | _____ _ __ ___ 
 | |   | | | / __| __/ _ \| '_ ` _ \  | |\/| |/ _` | '__| |/ / _ \ '__/ __|
 | |___| |_| \__ \ || (_) | | | | | | | |  | | (_| | |  |   <  __/ |  \__ \.
  \_____\__,_|___/\__\___/|_| |_| |_| |_|  |_|\__,_|_|  |_|\_\___|_|  |___/
	
VERSION 1.0

SA-MP : http://forum.sa-mp.com/member.php?u=257910
GitHub: https://github.com/YouzerName

Copyright (c) 2017 YouzerName (a.k.a PeanutButter)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*///---------------------------------------------------------------------------------------------------------------------

#if defined _custommarkers_included
	#endinput
#endif

#if !defined _samp_included
	#tryinclude <a_samp>
	#if !defined _samp_included
		#error CustomMarkers.inc could not locate a_samp.inc file, please check your server includes
	#endif
#endif

#if !defined INVALID_STREAMER_ID
	#tryinclude <streamer>
	#if !defined INVALID_STREAMER_ID
		#error CustomMarkers.inc could not locate streamer.inc file, please check your server includes
	#endif
#endif

#if !defined _inc_y_timers
	#tryinclude <YSI\y_timers>
	#tryinclude <y_timers>
	#if !defined _inc_y_timers
		#error CustomMarkers.inc could not locate YSI\y_timers.inc file, please check your server includes
	#endif
#endif

#define _custommarkers_included

#define TIMER_INT 500

//---------------------------------------------------------------------------------------------------------------------

/*
SetPlayerCustomMarker(playerid, playerarray[], arraycount, type, color = -1, worldid = -1, interiorid = -1, Float:streamdistance = STREAMER_MAP_ICON_SD, STREAMER_TAG_AREA areaid = STREAMER_TAG_AREA -1);
	
	Info:

		Sets a marker for a player that can be seen by everyone in playerarray[]


	Parameters:

		- playerid
			= The playerid that should get a marker on the radar

		- playerarray
			= The array storing the players that should see the playerid's marker on the map

		- arraycount[]
			= Amount of players in the playerarray

		- type
			= The type of marker to display, see http://wiki.sa-mp.com/wiki/MapIcons

		- color (OPTIONAL: Use -1 as default)
			= The color of the icon, as an integer or hex in RGBA color format, only works if type = 0

		- worldid (OPTIONAL: Use -1 as default)
			= The ID of the virtual world in which to marker should be displayed

		- interiorid (OPTIONAL: Use -1 as default)
			= The ID of the interior in which to marker should be displayed

		- streamdistance (OPTIONAL)
			= The max distance, as a float, at which the markers can be seen by other players

		- areaid (OPTIONAL) - Does not work in V1.0 yet
			= The area, from the streamer include, in which the player markers should be shown
*/

//---------------------------------------------------------------------------------------------------------------------

/*
SetPlayerCustomMarkerForAll(playerid, type, color = -1, worldid = -1, interiorid = -1, Float:streamdistance = STREAMER_MAP_ICON_SD, STREAMER_TAG_AREA areaid = STREAMER_TAG_AREA -1);
	
	Info:

		Sets a marker for a player that can be seen by everyone in the server.


	Parameters:

		Same as SetPlayerCustomMarker, but without playerarray[] and arraycount
*/

//---------------------------------------------------------------------------------------------------------------------

/*
DestroyPlayerCustomMarker(playerid)

	Info:

		Deletes a player's marker


	Parameters:

		-playerid
			= The ID of the player to delete the marker for
*/

//---------------------------------------------------------------------------------------------------------------------

enum MarkerInfo
{
	Float:Position[3],
	ShowType, // 1 = SetPlayerCustomMarkerForAll | 0 = SetPlayerCustomMarker
	Showplayerid[MAX_PLAYERS],
	Arraycount,
	Type,
	Color,
	Style,
	Worldid,
	Interiorid,
	Float:Streamdistance,
	STREAMER_TAG_AREA Areaid, // Does not work in V1.0 yet
}
static PlayerMarkerStats[MAX_PLAYERS][MarkerInfo];
static DynamicPlayerMarkers[MAX_PLAYERS][MAX_PLAYERS];
static DynamicPlayerMarkersForAll[MAX_PLAYERS];
static MarkerTimerActive[MAX_PLAYERS];

//---------------------------------------------------------------------------------------------------------------------

stock SetPlayerCustomMarker(playerid, playerarray[], arraycount, type, color = -1, worldid = -1, interiorid = -1, Float:streamdistance = STREAMER_MAP_ICON_SD, areaid = -1)
{
	if(IsPlayerConnected(playerid))
	{
		PlayerMarkerStats[playerid][ShowType] = 0;
		PlayerMarkerStats[playerid][Type] = type;
		PlayerMarkerStats[playerid][Color] = color;
		PlayerMarkerStats[playerid][Worldid] = worldid;
		PlayerMarkerStats[playerid][Interiorid] = interiorid;
		PlayerMarkerStats[playerid][Streamdistance] = streamdistance;
		PlayerMarkerStats[playerid][Style] = MAPICON_GLOBAL;
		PlayerMarkerStats[playerid][Areaid] = areaid; // Does not work in V1.0 yet
		PlayerMarkerStats[playerid][Arraycount] = arraycount;

		GetPlayerPos(playerid, PlayerMarkerStats[playerid][Position][0], PlayerMarkerStats[playerid][Position][1], PlayerMarkerStats[playerid][Position][2]);

		for(new i; i < arraycount; i++)
		{
			PlayerMarkerStats[playerid][Showplayerid][i] = playerarray[i];
		}

		if(MarkerTimerActive[playerid] == 0)
		{
			repeat UpdateMarkers(playerid);
			MarkerTimerActive[playerid] = 1;
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------

stock SetPlayerCustomMarkerForAll(playerid, type, color = -1, worldid = -1, interiorid = -1, Float:streamdistance = STREAMER_MAP_ICON_SD, areaid = -1)
{
	if(IsPlayerConnected(playerid))
	{
		PlayerMarkerStats[playerid][ShowType] = 1;
		PlayerMarkerStats[playerid][Type] = type;
		PlayerMarkerStats[playerid][Color] = color;
		PlayerMarkerStats[playerid][Worldid] = worldid;
		PlayerMarkerStats[playerid][Interiorid] = interiorid;
		PlayerMarkerStats[playerid][Streamdistance] = streamdistance;
		PlayerMarkerStats[playerid][Style] = MAPICON_GLOBAL;
		PlayerMarkerStats[playerid][Areaid] = areaid; // Does not work in V1.0 yet

		GetPlayerPos(playerid, PlayerMarkerStats[playerid][Position][0], PlayerMarkerStats[playerid][Position][1], PlayerMarkerStats[playerid][Position][2]);

		if(MarkerTimerActive[playerid] == 0)
		{
			repeat UpdateMarkers(playerid);
			MarkerTimerActive[playerid] = 1;
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------

stock DestroyPlayerCustomMarker(playerid)
{
	if(PlayerMarkerStats[playerid][ShowType] == 1)
	{
		DestroyDynamicMapIcon(DynamicPlayerMarkersForAll[playerid]);
	}
	else
	{
		for(new i; i < PlayerMarkerStats[playerid][Arraycount]; i++)
		{
			DestroyDynamicMapIcon(DynamicPlayerMarkers[playerid][i]);
		}
	}
	MarkerTimerActive[playerid] = 0;
}

//---------------------------------------------------------------------------------------------------------------------

timer UpdateMarkers[TIMER_INT](playerid)
{
	if(MarkerTimerActive[playerid] == 1)
	{
		GetPlayerPos(playerid, PlayerMarkerStats[playerid][Position][0], PlayerMarkerStats[playerid][Position][1], PlayerMarkerStats[playerid][Position][2]);

		if(PlayerMarkerStats[playerid][ShowType] == 1)
		{
			DestroyDynamicMapIcon(DynamicPlayerMarkersForAll[playerid]);
			DynamicPlayerMarkersForAll[playerid] = CreateDynamicMapIcon(PlayerMarkerStats[playerid][Position][0], PlayerMarkerStats[playerid][Position][1], PlayerMarkerStats[playerid][Position][2], PlayerMarkerStats[playerid][Type], PlayerMarkerStats[playerid][Color], PlayerMarkerStats[playerid][Worldid], PlayerMarkerStats[playerid][Interiorid], -1, PlayerMarkerStats[playerid][Streamdistance], PlayerMarkerStats[playerid][Style]);
			for(new i; i < MAX_PLAYERS; i++)
			{
				if(IsPlayerConnected(i) && i != playerid /*&& IsPlayerInRangeOfPoint(i, PlayerMarkerStats[playerid][Streamdistance], PlayerMarkerStats[playerid][Position][0], PlayerMarkerStats[playerid][Position][1], PlayerMarkerStats[playerid][Position][2])*/) Streamer_Update(i);
			}
		}
		else
		{
			for(new i; i < PlayerMarkerStats[playerid][Arraycount]; i++)
			{
				DestroyDynamicMapIcon(DynamicPlayerMarkers[playerid][i]);
				DynamicPlayerMarkers[playerid][i] = CreateDynamicMapIcon(PlayerMarkerStats[playerid][Position][0], PlayerMarkerStats[playerid][Position][1], PlayerMarkerStats[playerid][Position][2], PlayerMarkerStats[playerid][Type], PlayerMarkerStats[playerid][Color], PlayerMarkerStats[playerid][Worldid], PlayerMarkerStats[playerid][Interiorid], PlayerMarkerStats[playerid][Showplayerid][i], PlayerMarkerStats[playerid][Streamdistance], PlayerMarkerStats[playerid][Style]);
				if(i != playerid /*&& IsPlayerInRangeOfPoint(i, PlayerMarkerStats[playerid][Streamdistance], PlayerMarkerStats[playerid][Position][0], PlayerMarkerStats[playerid][Position][1], PlayerMarkerStats[playerid][Position][2])*/) Streamer_Update(i);
			}
		}

		Streamer_Update(playerid);
	}
}

//---------------------------------------------------------------------------------------------------------------------
