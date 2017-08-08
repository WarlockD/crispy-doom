

#include "doomtype.h"
#include "i_cdmus.h"
#include "i_joystick.h"
#include "i_system.h"
#include "i_video.h"
#include "i_sound.h"

// CD interface
WEAK_FUNCTION int I_CDMusInit(void) { return -1; }

// We cannot print status messages inline during startup, they must
// be deferred until after I_CDMusInit has returned.

WEAK_FUNCTION void I_CDMusPrintStartup(void) {}
WEAK_FUNCTION int I_CDMusPlay(int track) { (void)track; return 0;}
WEAK_FUNCTION int I_CDMusStop(void) { return 0;}
WEAK_FUNCTION int I_CDMusResume(void) {return 0;}
WEAK_FUNCTION int I_CDMusSetVolume(int volume) { (void)volume; return 0; }
WEAK_FUNCTION int I_CDMusFirstTrack(void){ return -1; }
WEAK_FUNCTION int I_CDMusLastTrack(void){ return -1; }
WEAK_FUNCTION int I_CDMusTrackLength(int track){ (void)track; return -1; }

//
// Displays the text mode ending screen after the game quits
//

WEAK_FUNCTION void I_Endoom(byte *endoom_data) { (void)endoom_data; }

// joystick
WEAK_FUNCTION void I_ShutdownJoystick(void) {}
WEAK_FUNCTION void I_InitJoystick(void) {}
WEAK_FUNCTION void I_UpdateJoystick(void) {}
WEAK_FUNCTION void I_BindJoystickVariables(void){}

// I Sound
WEAK_FUNCTION void I_InitSound(boolean use_sfx_prefix) {(void)use_sfx_prefix; }
WEAK_FUNCTION void I_ShutdownSound(void) {}
WEAK_FUNCTION int I_GetSfxLumpNum(sfxinfo_t *sfxinfo) { (void)sfxinfo; return 0;}
WEAK_FUNCTION void I_UpdateSound(void) {}
WEAK_FUNCTION void I_UpdateSoundParams(int channel, int vol, int sep) { (void)channel; (void)vol; (void)sep; }
WEAK_FUNCTION int I_StartSound(sfxinfo_t *sfxinfo, int channel, int vol, int sep, int pitch)  {
	(void)sfxinfo; (void)channel; (void)vol; (void)sep; (void)pitch; return 0;
}
WEAK_FUNCTION void I_StopSound(int channel) {  (void)channel; }
WEAK_FUNCTION boolean I_SoundIsPlaying(int channel) {  (void)channel; return false; }
WEAK_FUNCTION void I_PrecacheSounds(sfxinfo_t *sounds, int num_sounds)  {  (void)sounds;  (void)num_sounds; }
// music
WEAK_FUNCTION void I_InitMusic(void) {}
WEAK_FUNCTION void I_ShutdownMusic(void) {}
WEAK_FUNCTION void I_SetMusicVolume(int volume) { (void)volume; }
WEAK_FUNCTION void I_PauseSong(void) {}
WEAK_FUNCTION void I_ResumeSong(void) {}
WEAK_FUNCTION void * I_RegisterSong(void* data, int len) {(void)data; (void)len; return NULL; }
WEAK_FUNCTION void I_UnRegisterSong(void *handle) { (void)handle; }
WEAK_FUNCTION void I_PlaySong(void *handle, boolean looping) { (void)handle; (void)looping; }
WEAK_FUNCTION void I_StopSong(){}
WEAK_FUNCTION boolean I_MusicIsPlaying(){ return false; }
WEAK_FUNCTION void I_BindSoundVariables(void) {}
