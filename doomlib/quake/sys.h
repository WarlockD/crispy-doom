/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// sys.h -- non-portable functions

//
// file IO
//

// returns the file size
// return -1 if file is not present
// the file should be in BINARY mode for stupid OSs that care
EXTERN_CPP int Sys_FileOpenRead (char *path, int *hndl);

EXTERN_CPP int Sys_FileOpenWrite (char *path);
EXTERN_CPP void Sys_FileClose (int handle);
EXTERN_CPP void Sys_FileSeek (int handle, int position);
EXTERN_CPP int Sys_FileRead (int handle, void *dest, int count);
EXTERN_CPP int Sys_FileWrite (int handle, void *data, int count);
EXTERN_CPP int	Sys_FileTime (char *path);
EXTERN_CPP void Sys_mkdir (char *path);

//
// memory protection
//
EXTERN_CPP void Sys_MakeCodeWriteable (unsigned long startaddr, unsigned long length);

//
// system IO
//
EXTERN_CPP void Sys_DebugLog(char *file, char *fmt, ...);

EXTERN_CPP void Sys_Error (char *error, ...);
// an error will cause the entire program to exit

EXTERN_CPP void Sys_Printf (char *fmt, ...);
// send text to the console

EXTERN_CPP void Sys_Quit (void);

EXTERN_CPP double Sys_FloatTime (void);

EXTERN_CPP char *Sys_ConsoleInput (void);

EXTERN_CPP void Sys_Sleep (void);
// called to yield for a little bit so as
// not to hog cpu when paused or debugging

EXTERN_CPP void Sys_SendKeyEvents (void);
// Perform Key_Event () callbacks until the input que is empty

EXTERN_CPP void Sys_LowFPPrecision (void);
EXTERN_CPP void Sys_HighFPPrecision (void);
EXTERN_CPP void Sys_SetFPCW (void);

