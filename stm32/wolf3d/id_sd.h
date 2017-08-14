//
//      ID Engine
//      ID_SD.h - Sound Manager Header
//      Version for Wolfenstein
//      By Jason Blochowiak
//

#ifndef __ID_SD__
#define __ID_SD__

#define alOut(n,b) YM3812Write(0, n, b)

#define TickBase        70      // 70Hz per tick - used as a base for timer 0

typedef enum
{
    sdm_Off,
    sdm_PC,sdm_AdLib,
} SDMode;

typedef enum
{
    smm_Off,smm_AdLib
} SMMode;

typedef enum
{
    sds_Off,sds_PC,sds_SoundBlaster
} SDSMode;

typedef struct
{
    longword        length;
    word            priority;
} SoundCommon;

#define ORIG_SOUNDCOMMON_SIZE 6

//      PC Sound stuff
#define pcTimer         0x42
#define pcTAccess       0x43
#define pcSpeaker       0x61

#define pcSpkBits       3

typedef struct
{
    SoundCommon     common;
    byte            data[1];
} PCSound;

//      Register addresses
// Operator stuff
#define alChar          0x20
#define alScale         0x40
#define alAttack        0x60
#define alSus           0x80
#define alWave          0xe0
// Channel stuff
#define alFreqL         0xa0
#define alFreqH         0xb0
#define alFeedCon       0xc0
// Global stuff
#define alEffects       0xbd

typedef struct
{
    byte    mChar,cChar,
            mScale,cScale,
            mAttack,cAttack,
            mSus,cSus,
            mWave,cWave,
            nConn,

            // These are only for Muse - these bytes are really unused
            voice,
            mode;
    byte    unused[3];
} Instrument;

#define ORIG_INSTRUMENT_SIZE 16

typedef struct
{
    SoundCommon     common;
    Instrument      inst;
    byte            block;
    byte            data[1];
} AdLibSound;

#define ORIG_ADLIBSOUND_SIZE (ORIG_SOUNDCOMMON_SIZE + ORIG_INSTRUMENT_SIZE + 2)

//
//      Sequencing stuff
//
#define sqMaxTracks     10

typedef struct
{
    word    length;
    word    values[1];
} MusicGroup;

typedef struct
{
    int valid;
    fixed globalsoundx, globalsoundy;
} globalsoundpos;

extern globalsoundpos channelSoundPos[];

// Global variables
extern  boolean         AdLibPresent,
                        SoundBlasterPresent,
                        SoundPositioned;
extern  SDMode          SoundMode;
extern  SDSMode         DigiMode;
extern  SMMode          MusicMode;
extern  int             DigiMap[];
extern  int             DigiChannel[];

#define GetTimeCount()  ((GetTicksMS()*7)/100)

#if defined(USE_SDL)
inline void DelayMS(int ms) { DelayMS(ms); }
inline uint32_t GetTicksMS(int ms) { return GetTicksMS(); }

#endif

#if defined(USE_STM32746G_DISCO)
inline void DelayMS(int ms) { HAL_Delay(ms); }
inline uint32_t GetTicksMS() { return HAL_GetTick(); }

#endif

inline void Delay(int wolfticks)
{
    if(wolfticks>0) DelayMS(wolfticks * 100 / 7);
}
#define SOUND_ENABLED

#ifdef SOUND_ENABLED

// Function prototypes
extern  void    SD_Startup(void),
                SD_Shutdown(void);

extern  int     SD_GetChannelForDigi(int which);
extern  void    SD_PositionSound(int leftvol,int rightvol);
extern  boolean SD_PlaySound(soundnames sound);
extern  void    SD_SetPosition(int channel, int leftvol,int rightvol);
extern  void    SD_StopSound(void),
                SD_WaitSoundDone(void);

extern  void    SD_StartMusic(int chunk);
extern  void    SD_ContinueMusic(int chunk, int startoffs);
extern  void    SD_MusicOn(void),
                SD_FadeOutMusic(void);
extern  int     SD_MusicOff(void);

extern  boolean SD_MusicPlaying(void);
extern  boolean SD_SetSoundMode(SDMode mode);
extern  boolean SD_SetMusicMode(SMMode mode);
extern  word    SD_SoundPlaying(void);

extern  void    SD_SetDigiDevice(SDSMode);
extern  void	SD_PrepareSound(int which);
extern  int     SD_PlayDigitized(word which,int leftpos,int rightpos);
extern  void    SD_StopDigitized(void);
#else
#define SD_Startup()
#define SD_Shutdown(void);

#define SD_GetChannelForDigi( which)
#define SD_PositionSound( leftvol, rightvol)
#define SD_PlaySound( sound)
#define SD_SetPosition( channel,  leftvol, rightvol)
#define SD_StopSound()
#define SD_WaitSoundDone()

#define SD_StartMusic( chunk)
#define SD_ContinueMusic( chunk,  startoffs)
#define SD_MusicOn()
#define SD_FadeOutMusic()
#define SD_MusicOff()

#define   SD_MusicPlaying() (false)
#define   SD_SetSoundMode( mode) (false)
#define   SD_SetMusicMode( mode) (false)
#define   SD_SoundPlaying() (0)

#define SD_SetDigiDevice(SDSMode)
#define SD_PrepareSound( which)
#define SD_PlayDigitized(which, leftpos, rightpos) (-1)
#define SD_StopDigitized()
#endif

#endif
