#ifndef _H_FEATURES_H_
#define _H_FEATURES_H_

//#define _DEFAULT_SOURCE 1
#if 0
#define _POSIX_CLOCK_SELECTION  1
#define _POSIX_MONOTONIC_CLOCK  1
#define _POSIX_TIMERS  1


#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 200809L
#endif

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#endif
#ifndef _POSIX_TIMERS
#define _POSIX_TIMERS 200809L
#endif

#ifndef _POSIX_MONOTONIC_CLOCK
#define _POSIX_MONOTONIC_CLOCK 200809L
#endif

#ifndef _POSIX_CLOCK_SELECTION
#define _POSIX_CLOCK_SELECTION 200809L
#endif




#include_next <sys\features.h>









#endif
