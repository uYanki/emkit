#ifndef __EEFS_CONF_H__
#define __EEFS_CONF_H__

#include "gsdk.h"

/* Maximum number of files in the file system */
#define EEFS_MAX_FILES 64

/* Maximum number of file descriptors */
#define EEFS_MAX_OPEN_FILES 20

/* Default number of spare bytes added to the end of a slot when a
   new file is created by calling the EEFS_LibCreat function */
#define EEFS_DEFAULT_CREAT_SPARE_BYTES 512

/*
 * System Dependent Lower Level Functions
 */

/* These macros define the lower level EEPROM interface functions.  Defaults to memcpy(Dest, Src, Length) */
#define EEFS_LIB_EEPROM_WRITE(Dest, Src, Length) eefs_write(Dest, Src, Length)
#define EEFS_LIB_EEPROM_READ(Dest, Src, Length)  eefs_read(Dest, Src, Length)
#define EEFS_LIB_EEPROM_FLUSH

/* These macros define the lock and unlock interface functions used to guarentee
 * exclusive access to shared resources.  Defaults to undefined since it is implementation dependent */
#define EEFS_LIB_LOCK
#define EEFS_LIB_UNLOCK

/* This macro defines the time interface function.  Defaults to time(NULL) */
#define EEFS_LIB_TIME (GetTickUs() / 1000)  // time(NULL)

/* This macro defines the file system write protection interface function.  If the file system
   is read-only then set this macro to TRUE.  If the file system is always write enabled then
   set this macro to FALSE.  If the eeprom has an external write protection interface then a custom
   function can be called to determine the write protect status. */
#define EEFS_LIB_IS_WRITE_PROTECTED FALSE

#endif