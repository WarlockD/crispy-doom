/**
  ******************************************************************************
  * @file    Display/LTDC_PicturesFromSDCard/Src/main.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    30-December-2016
  * @brief   This file provides main program functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include "main.h"
#include <assert.h>
#include <dirent.h>

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

void dir_test() {
	{
	 DIR *dir;
	        struct dirent *ent;
	        int found = 0;

	        /* Open directory */
	        dir = opendir ("/doom");
	        if (dir == NULL) {
	            fprintf (stderr, "Directory tests/1 not found\n");
	            abort ();
	        }

	        /* Read entries */
	        while ((ent = readdir (dir)) != NULL) {
	        	printf("DIR: %s\n", ent->d_name);
#if 0
	            /* Check each file */
	            if (strcmp (ent->d_name, ".") == 0) {
	                /* Directory itself */
	#ifdef _DIRENT_HAVE_D_TYPE
	                assert (ent->d_type == DT_DIR);
	#endif
	#ifdef _DIRENT_HAVE_D_NAMLEN
	                assert (ent->d_namlen == 1);
	#endif
	#ifdef _D_EXACT_NAMLEN
	                assert (_D_EXACT_NAMLEN(ent) == 1);
	#endif
	#ifdef _D_ALLOC_NAMLEN
	                assert (_D_ALLOC_NAMLEN(ent) > 1);
	#endif
	                found += 1;

	            } else if (strcmp (ent->d_name, "..") == 0) {
	                /* Parent directory */
	#ifdef _DIRENT_HAVE_D_TYPE
	                assert (ent->d_type == DT_DIR);
	#endif
	#ifdef _DIRENT_HAVE_D_NAMLEN
	                assert (ent->d_namlen == 2);
	#endif
	#ifdef _D_EXACT_NAMLEN
	                assert (_D_EXACT_NAMLEN(ent) == 2);
	#endif
	#ifdef _D_ALLOC_NAMLEN
	                assert (_D_ALLOC_NAMLEN(ent) > 2);
	#endif
	                found += 2;

	            } else if (strcmp (ent->d_name, "file") == 0) {
	                /* Regular file */
	#ifdef _DIRENT_HAVE_D_TYPE
	                assert (ent->d_type == DT_REG);
	#endif
	#ifdef _DIRENT_HAVE_D_NAMLEN
	                assert (ent->d_namlen == 4);
	#endif
	#ifdef _D_EXACT_NAMLEN
	                assert (_D_EXACT_NAMLEN(ent) == 4);
	#endif
	#ifdef _D_ALLOC_NAMLEN
	                assert (_D_ALLOC_NAMLEN(ent) > 4);
	#endif
	                found += 4;

	            } else if (strcmp (ent->d_name, "dir") == 0) {
	                /* Just a directory */
	#ifdef _DIRENT_HAVE_D_TYPE
	                assert (ent->d_type == DT_DIR);
	#endif
	#ifdef _DIRENT_HAVE_D_NAMLEN
	                assert (ent->d_namlen == 3);
	#endif
	#ifdef _D_EXACT_NAMLEN
	                assert (_D_EXACT_NAMLEN(ent) == 3);
	#endif
	#ifdef _D_ALLOC_NAMLEN
	                assert (_D_ALLOC_NAMLEN(ent) > 3);
	#endif
	                found += 8;

	            } else {
	                /* Other file */
	                fprintf (stderr, "Unexpected file %s\n", ent->d_name);
	                abort ();
	            }
#endif

	        }

	        /* Make sure that all files were found */
	        assert (found == 0xf);

	        closedir (dir);
	    }

	    /* Rewind of directory stream */
	    {
	        DIR *dir;
	        struct dirent *ent;
	        int found = 0;

	        /* Open directory */
	        dir = opendir ("/");
	        assert (dir != NULL);

	        /* Read entries */
	        while ((ent = readdir (dir)) != NULL) {

	            /* Check each file */
	            if (strcmp (ent->d_name, ".") == 0) {
	                /* Directory itself */
	                found += 1;

	            } else if (strcmp (ent->d_name, "..") == 0) {
	                /* Parent directory */
	                found += 2;

	            } else if (strcmp (ent->d_name, "file") == 0) {
	                /* Regular file */
	                found += 4;

	            } else if (strcmp (ent->d_name, "dir") == 0) {
	                /* Just a directory */
	                found += 8;

	            } else {
	                /* Other file */
	                fprintf (stderr, "Unexpected file %s\n", ent->d_name);
	                abort ();
	            }

	        }

	        /* Make sure that all files were found */
	        assert (found == 0xf);

	        /* Rewind stream and read entries again */
	        rewinddir (dir);
	        found = 0;

	        /* Read entries */
	        while ((ent = readdir (dir)) != NULL) {

	            /* Check each file */
	            if (strcmp (ent->d_name, ".") == 0) {
	                /* Directory itself */
	                found += 1;

	            } else if (strcmp (ent->d_name, "..") == 0) {
	                /* Parent directory */
	                found += 2;

	            } else if (strcmp (ent->d_name, "file") == 0) {
	                /* Regular file */
	                found += 4;

	            } else if (strcmp (ent->d_name, "dir") == 0) {
	                /* Just a directory */
	                found += 8;

	            } else {
	                /* Other file */
	                fprintf (stderr, "Unexpected file %s\n", ent->d_name);
	                abort ();
	            }

	        }

	        /* Make sure that all files were found */
	        assert (found == 0xf);

	        closedir (dir);
	    }

	    /* Rewind with intervening change of working directory */
	    {
	        DIR *dir;
	        struct dirent *ent;
	        int found = 0;
	        int errorcode;

	        /* Open directory */
	        dir = opendir ("/");
	        assert (dir != NULL);

	        /* Read entries */
	        while ((ent = readdir (dir)) != NULL) {

	            /* Check each file */
	            if (strcmp (ent->d_name, ".") == 0) {
	                /* Directory itself */
	                found += 1;

	            } else if (strcmp (ent->d_name, "..") == 0) {
	                /* Parent directory */
	                found += 2;

	            } else if (strcmp (ent->d_name, "file") == 0) {
	                /* Regular file */
	                found += 4;

	            } else if (strcmp (ent->d_name, "dir") == 0) {
	                /* Just a directory */
	                found += 8;

	            } else {
	                /* Other file */
	                fprintf (stderr, "Unexpected file %s\n", ent->d_name);
	                abort ();
	            }

	        }

	        /* Make sure that all files were found */
	        assert (found == 0xf);

	        /* Change working directory */
	        errorcode = chdir ("tests");
	        assert (errorcode == 0);

	        /* Rewind stream and read entries again */
	        rewinddir (dir);
	        found = 0;

	        /* Read entries */
	        while ((ent = readdir (dir)) != NULL) {

	            /* Check each file */
	            if (strcmp (ent->d_name, ".") == 0) {
	                /* Directory itself */
	                found += 1;

	            } else if (strcmp (ent->d_name, "..") == 0) {
	                /* Parent directory */
	                found += 2;

	            } else if (strcmp (ent->d_name, "file") == 0) {
	                /* Regular file */
	                found += 4;

	            } else if (strcmp (ent->d_name, "dir") == 0) {
	                /* Just a directory */
	                found += 8;

	            } else {
	                /* Other file */
	                fprintf (stderr, "Unexpected file %s\n", ent->d_name);
	                abort ();
	            }

	        }

	        /* Make sure that all files were found */
	        assert (found == 0xf);

	        /* Restore working directory */
	        errorcode = chdir ("..");
	        assert (errorcode == 0);

	        closedir (dir);
	    }

	    /* Long file name */
	    {
	        DIR *dir;
	        struct dirent *ent;
	        int found = 0;

	        /* Open directory */
	        dir = opendir ("tests/2");
	        if (dir == NULL) {
	            fprintf (stderr, "Directory tests/2 not found\n");
	            abort ();
	        }

	        /* Read entries */
	        while ((ent = readdir (dir)) != NULL) {

	            /* Check each file */
	            if (strcmp (ent->d_name, ".") == 0) {
	                /* Directory itself */
	                found += 1;

	            } else if (strcmp (ent->d_name, "..") == 0) {
	                /* Parent directory */
	                found += 2;

	            } else if (strcmp (ent->d_name, "file.txt") == 0) {
	                /* Regular 8+3 filename */
	#ifdef _DIRENT_HAVE_D_TYPE
	                assert (ent->d_type == DT_REG);
	#endif
	#ifdef _DIRENT_HAVE_D_NAMLEN
	                assert (ent->d_namlen == 8);
	#endif
	#ifdef _D_EXACT_NAMLEN
	                assert (_D_EXACT_NAMLEN(ent) == 8);
	#endif
	#ifdef _D_ALLOC_NAMLEN
	                assert (_D_ALLOC_NAMLEN(ent) > 8);
	#endif
	                found += 4;

	            } else if (strcmp (ent->d_name, "Testfile-1.2.3.dat") == 0) {
	                /* Long file name with multiple dots */
	#ifdef _DIRENT_HAVE_D_TYPE
	                assert (ent->d_type == DT_REG);
	#endif
	#ifdef _DIRENT_HAVE_D_NAMLEN
	                assert (ent->d_namlen == 18);
	#endif
	#ifdef _D_EXACT_NAMLEN
	                assert (_D_EXACT_NAMLEN(ent) == 18);
	#endif
	#ifdef _D_ALLOC_NAMLEN
	                assert (_D_ALLOC_NAMLEN(ent) > 18);
	#endif
	                found += 8;

	            } else {
	                /* Other file */
	                fprintf (stderr, "Unexpected file %s\n", ent->d_name);
	                abort ();
	            }

	        }

	        /* Make sure that all files were found */
	        assert (found == 0xf);

	        closedir (dir);
	    }

	    printf ("OK\n");


}
int test_main(void)
{
  uint32_t counter = 0, transparency = 0;
  uint8_t str[30];
  dir_test();
  while(1)
  {     
  }
}

/**
  * @}
  */

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
