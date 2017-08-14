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
// sys_sun.h -- Sun system driver

#include "quakedef.h"
#include "errno.h"
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <sys/param.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/types.h>
#include <fcntl.h>

#include <stdio.h>
#include "stm32746g_discovery_lcd.h"

qboolean			isDedicated;


/*
================
filelength
================
*/
int filelength (FILE *f)
{
    int             pos;
    int             end;

    pos = ftell (f);
    fseek (f, 0, SEEK_END);
    end = ftell (f);
    fseek (f, pos, SEEK_SET);

    return end;
}

int Sys_FileOpenRead (char *path, int *handle)
{
	int	h;
	struct stat	fileinfo;
    

	h = open (path, O_RDONLY, 0666);
	*handle = h;
	if (h == -1)
		return -1;

	if (fstat (h,&fileinfo) == -1)
		Sys_Error ("Error fstating %s", path);

	return fileinfo.st_size;
}

int Sys_FileOpenWrite (char *path)
{
	int     handle;

	handle = open(path,O_RDWR | O_CREAT | O_TRUNC
	, 0666);

	if (handle == -1)
		Sys_Error ("Error opening %s: %s", path,strerror(errno));

	return handle;
}

void Sys_FileClose (int handle)
{
	close (handle);
}

void Sys_FileSeek (int handle, int position)
{
	lseek (handle, position, SEEK_SET);
}

int Sys_FileRead (int handle, void *dest, int count)
{
    return read (handle, dest, count);
}

int Sys_FileWrite (int handle, void *data, int count)
{
	return write (handle, data, count);
}

int Sys_FileTime (char *path)
{
	struct	stat	buf;

	if (stat (path,&buf) == -1)
		return -1;

	return buf.st_mtime;
}

void Sys_mkdir (char *path)
{
    mkdir( path, 0777 );
}

/*
===============================================================================

SYSTEM IO

===============================================================================
*/

void Sys_MakeCodeWriteable (unsigned long startaddr, unsigned long length)
{

	int r;
	unsigned long addr;
	//int psize = getpagesize();

	//addr = (startaddr & ~(psize-1)) - psize;

//	fprintf(stderr, "writable code %lx(%lx)-%lx, length=%lx\n", startaddr,
//			addr, startaddr+length, length);

	//r = mprotect((char*)addr, length + startaddr - addr + psize, 7);

	if (r < 0)
    		Sys_Error("Protection change failed\n");

}


void Sys_Error (char *error, ...)
{
    va_list         argptr;

    printf ("Sys_Error: ");   
    va_start (argptr,error);
    vprintf (error,argptr);
    va_end (argptr);
    printf ("\n");
    Host_Shutdown();
    exit (1);
}

void Sys_Printf (char *fmt, ...)
{
    va_list         argptr;
    
    va_start (argptr,fmt);
    vprintf (fmt,argptr);
    va_end (argptr);
}

void Sys_Quit (void)
{
    Host_Shutdown();
    exit (0);
}

double Sys_FloatTime (void)
{
    struct timeval tp;
    struct timezone tzp; 
    static int      secbase; 
    
    gettimeofday(&tp, &tzp);  

    if (!secbase)
    {
        secbase = tp.tv_sec;
        return tp.tv_usec/1000000.0;
    }

    return (tp.tv_sec - secbase) + tp.tv_usec/1000000.0;
}

char *Sys_ConsoleInput (void)
{
    static char	text[256];
    int		len;
    fd_set	readfds;
    int		ready;
    struct timeval timeout;
    
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(0, &readfds);
    ready = select(1, &readfds, 0, 0, &timeout);
    
    if (ready>0)
    {
	len = read (0, text, sizeof(text));
	if (len >= 1)
	{
		text[len-1] = 0;	// rip off the /n and terminate
		return text;
	}
    }
    
    return 0;
}

void Sys_Sleep (void)
{
}

#if !id386
void Sys_HighFPPrecision (void)
{
}

void Sys_LowFPPrecision (void)
{
}
#endif

void Sys_Init(void)
{
#if id386
	Sys_SetFPCW();
#endif
}


//=============================================================================

int quake_main (int argc, char **argv)
{
    static quakeparms_t    parms;
    float time, oldtime, newtime;
    
    parms.memsize = 8*1024*1024;
    parms.membase = (void*)(LCD_FB_START_ADDRESS + (480*272*4));
    parms.basedir = ".";
    parms.cachedir = NULL;

    COM_InitArgv (argc, argv);

    parms.argc = com_argc;
    parms.argv = com_argv;

    printf ("Host_Init\n");
    Host_Init (&parms);

	Sys_Init();

    // unroll the simulation loop to give the video side a chance to see _vid_default_mode
    Host_Frame( 0.1 );


    oldtime = Sys_FloatTime();
    while (1)
    {
		newtime = Sys_FloatTime();
		Host_Frame (newtime - oldtime);
		oldtime = newtime;
    }
	return 0;
}




