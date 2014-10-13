/****************************************************************************/
/*                                                                          */
/* COPYRIGHT IBM CORP., 1992                                                */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/*   Is_WorkFrame_installed()                                               */
/*                                                                          */
/*      Check to see whether the WorkFrame/2 has been installed or not      */
/*      in the OS2.INI.  If the WorkFrame/2 was installed, save the         */
/*      WorkFrame's directory in the buffer provided.  This buffer          */
/*      bust be large enough to hold the longest possible path name.        */
/*                                                                          */
/*                                                                          */
/*   Returns:                                                               */
/*                                                                          */
/*      TRUE   -  the information in the OS2.INI indicates that the         */
/*                WorkFrame/2 is installed                                  */
/*                                                                          */
/*      FALSE  -  the WorkFrame is not installed yet.                       */
/*                                                                          */
/****************************************************************************/

#define DEFAULT_SHELL_APP   "IBMWF"
#define DEFAULT_SHELL_DIR   "DIR"

BOOL Is_WorkFrame_installed( int    Buffer_size,
                             char * Buffer )
   {
   /********************************************************/
   /*   Retrieve the previous directory from the OS2.INI   */
   /********************************************************/
   if( PrfQueryProfileString(  HINI_USERPROFILE,    /* search OS2.INI only */
                               DEFAULT_SHELL_APP,
                               DEFAULT_SHELL_DIR,
                               NULL,
                               Buffer,
                               Buffer_size) == 0L)
      {
      return FALSE;
      }


   return TRUE;
   }

