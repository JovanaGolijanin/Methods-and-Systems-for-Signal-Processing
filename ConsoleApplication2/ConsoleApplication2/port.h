#pragma once

#ifndef DONE_PORT       /* Only do this if not previously done.                   */

#ifdef THINK_C
#define UBYTE unsigned char      /* Unsigned byte                       */
#define UWORD unsigned int       /* Unsigned word (2 bytes)             */
#define ULONG unsigned long      /* Unsigned word (4 bytes)             */
#define BOOL  unsigned char      /* Boolean                             */
#define FOPEN_BINARY_READ  "rb"  /* Mode string for binary reading.     */
#define FOPEN_BINARY_WRITE "wb"  /* Mode string for binary writing.     */
#define FOPEN_TEXT_APPEND  "a"   /* Mode string for text appending.     */
#define REAL double              /* USed for floating point stuff.      */
#endif

#define DONE_PORT                   /* Don't do all this again.            */
#define MALLOC_FAIL NULL            /* Failure status from malloc()        */
#define LOCAL static                /* For non-exported routines.          */
#define EXPORT                      /* Signals exported function.          */
#define then                        /* Useful for aligning ifs.            */

#endif