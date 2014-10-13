/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/* COPYRIGHT IBM CORP., 1992                                                */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/*                                                                          */
/* This is the OS/2 component of the DOSRUN package.  It's responsible      */
/* for establishing a unique container between this OS/2 component and      */
/* its sister DOS component.  The DOS component will handle redirecting     */
/* all STDOUT and STDERR messages from the DOS tool into this unique        */
/* container.  Once the DOS tool has completed, it will also communicate    */
/* the return codes either through the container or through some other      */
/* means.  In the current DOSRUN package, the unique container implemented  */
/* is a simple temporary file.  Once the DOS tool has completed, the        */
/* return codes will also be added to the end of the temporary file.        */
/* While the OS/2 component is waiting for the DOS tool to end, it will     */
/* read from the unique container and echo it out to its STDOUT.  Since     */
/* the WorkFrame/2 has already redirected its STDOUT and STDERR into        */
/* the WorkFrame/2's output listbox, this is all that is required to        */
/* redirect the DOS tool output into the WorkFrame/2's output listbox.      */
/*                                                                          */
/* The following illustrates this process for a DOS compiler:               */
/*                                                                          */
/*                                                                          */
/*  ÚÄÄÄÄ¿                                                                  */
/*  ³    ³                                                                  */
/*  ³    ³                                                                  */
/*  ³    ³                                                                  */
/*  ³WF/2³                                                                  */
/*  ³    ³                                                                  */
/*  ³    ³                                                                  */
/*  ÀÄÂÄÄÙ                                                                  */
/*    ³                                                                     */
/*    ³  User invokes                                                       */
/*    ³  action COMPILE                                                     */
/*                                                                         */
/*  ÚÄÁÄÄÄÄÄÄÄÄÄ¿                    ÚÄÄÄÄÄÄÄÄ¿                             */
/*  ³           ³                    ³WF/2    ³                             */
/*  ³WF/2       ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´Output  ³                             */
/*  ³Background ³                    ³Listbox ³                             */
/*  ³Thread     ³                    ÀÄÄÄÂÄÄÄÄÙ                             */
/*  ÀÄÂÄÄÄÄÄÄÄÄÄÙ                        ³                                  */
/*    ³                                  Contents of the unique container  */
/*    ³  Redirect STDOUT/STDERR          ³is echoed by DOSCL1.EXE through   */
/*    ³  back to output listbox          ³its STDOUT which has been         */
/*    ³  and invoke DOSCL1.EXE           ³redirected back to the WF/2.      */
/*                                  ÚÄÄÄÁÄÄÄÄÄ¿                            */
/*  ÚÄÁÄÄÄÄÄÄÄÄ¿                     ³Unique   ³                            */
/*  ³DOSCL1.EXEÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´Container³                            */
/*  ÀÄÂÄÄÄÄÄÄÄÄÙ                     ³         ³                            */
/*    ³                              ÀÄÄÄÂÄÄÄÄÄÙ                            */
/*    ³  Create a unique container       ³                                  */
/*    ³  and invoke DOSCL2.EXE                                             */
/*    ³  the name of the container       ³                                  */
/*                                      ³                                  */
/*  ÚÄÁÄÄÄÄÄÄÄÄ¿                         ³                                  */
/*  ³DOSCL2.EXE³                         ³                                  */
/*  ÀÄÂÄÄÄÄÄÄÄÄÙ                         ³                                  */
/*    ³                                  ³                                  */
/*    ³  Redirect STDOUT/STDERR          ³                                  */
/*    ³  back to the unique container    ³STDOUT/STDERR from DOS            */
/*    ³  and invoke DOS Compiler         ³compiler redirected back to       */
/*                                      ³the unique container              */
/*  ÚÄÁÄÄÄÄÄÄÄÄÄÄ¿                       ³                                  */
/*  ³DOS CompilerÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ                                  */
/*  ÀÄÄÄÄÄÄÄÄÄÄÄÄÙ                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/* A better implementation could use a named pipe as the unique container.  */
/* This way, you can avoid the expense of the physical I/O to disk.         */
/*                                                                          */
/****************************************************************************/

#define INCL_DOS
#define INCL_ERRORS
#define INCL_WIN
#include <os2.h>
#include <string.h>
#include <stdio.h>
#include <process.h>
#include <stdlib.h>
#include <signal.h>
#define EOFFLAG  26

static unsigned char *get_cmdline( void );

#define BUFSIZE 256

ULONG  sess_id,pid;
char  *tempfile;
HFILE  handle;

void handler( int signum );
BOOL ReadData( HFILE handle );
char tmpbuf[BUFSIZE];
int  rc;

int main( void )
{
   unsigned char *pCmdline;
   STARTDATA  sd;               /* structure defined in bsedos.h */
   STATUSDATA statusData;
   char      *buffer;
   char      *p1, *p2;
   char      *doscl2;
   ULONG      action;
   BOOL       Done, Done2=FALSE;


  /* Set exception handler for CTRL-C and CTRL-BREAK.                        */
  if ( SIG_ERR==signal(SIGILL, handler )  ||
       SIG_ERR==signal(SIGTERM, handler )  ||
       SIG_ERR==signal(SIGBREAK, handler )  )
     exit(2);

   /* Get a pointer to the command line.                                      */
   pCmdline = get_cmdline( );

   /* Get the executable directory, use it for doscl2.exe */
   p1 = pCmdline + strlen( pCmdline );
   p2 = p1 + 1;
   while ( p1 != pCmdline && *(p1-1) != '\\' && *(p1-1) != '/' ) --p1;

   *p1 = '\0';

   /* Fully qualify doscl2.exe */
   doscl2 = malloc( strlen(pCmdline) + strlen("doscl2.exe") + 1 );
   strcpy( doscl2, pCmdline );
   strcat( doscl2, "doscl2.exe" );

   /* Open a tempfile for doscl2.exe to write and doscl1.exe to read */
   tempfile = tmpnam( NULL );
   if ( (rc=DosOpen( tempfile,
                              &handle,
                              &action,
                              0,
                              FILE_NORMAL,
                              OPEN_ACTION_REPLACE_IF_EXISTS | OPEN_ACTION_CREATE_IF_NEW,
                              OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,
                              0 )) != 0 )
     {
     return rc;
     }

   /* Construct the input parameter string to doscl2.exe */
   buffer = malloc( strlen(tempfile) + strlen(p2) + 2 );
   strcpy( buffer, tempfile );
   strcat( buffer, " " );
   strcat( buffer, p2 );

   sd.Length      = 32;
   sd.Related     = SSF_RELATED_CHILD;
   sd.FgBg        = SSF_FGBG_BACK;
   sd.TraceOpt    = SSF_TRACEOPT_NONE;
   sd.PgmTitle    = "WF/2 DOS Client";
   sd.PgmName     = doscl2;
   sd.PgmInputs   = buffer;
   sd.TermQ       = NULL;
   sd.Environment = NULL;
   sd.InheritOpt  = SSF_INHERTOPT_PARENT;
   sd.SessionType = SSF_TYPE_WINDOWEDVDM;

   statusData.Length = 6;
   statusData.SelectInd = 0;
   statusData.SelectInd = 0;

   if (!DosStartSession( &sd, &sess_id, &pid ))
      Done      = FALSE;

   /* Wait for doscl2.exe to finish, keep reading tempfile at the same time */
   while( !Done )
      {
      if ( DosSetSession( sess_id, &statusData ) )
         Done = TRUE;

      DosSleep(5000);

      Done2 = ReadData( handle );
      }

   while ( !Done2 )
      {
      Done2 = ReadData( handle );
      } /* endwhile */

   DosClose( handle );
   DosDelete( tempfile );
   free( doscl2 );
   free( buffer );

   return(rc);
}



BOOL ReadData( HFILE handle )
   {
   BOOL   ret = FALSE;
   char * p1;
   int    dif;
   char   crc[2];
   ULONG  bytesread;
   ULONG  actual;

   DosRead( handle, tmpbuf, BUFSIZE, &bytesread );

   if ( bytesread )
      {
      if (p1=memchr( tmpbuf, EOFFLAG, bytesread ) )
         {
         dif        = bytesread - (int)(p1-tmpbuf);
         bytesread -= dif;
         if (dif < 3)
            {
            DosRead( handle, crc+dif, 3-dif, &bytesread );
            if (dif==1)
               crc[0] = *(p1+1);
            rc = *(unsigned int *)crc;
            }
         else
            rc = *(unsigned int *)(p1+1);

         ret = TRUE;
         } /* endif */

      DosWrite( 1, tmpbuf, bytesread, &actual );
      }

   return( ret );
   }


/* This function will return a pointer to the command line string.            */

static unsigned char *get_cmdline( void )
   {
   PTIB            ptib;
   PPIB            ppib;

   /* Get a pointer to the first through n arguments that were entered on     */
   /* the command line.                                                       */

   DosGetInfoBlocks( &ptib, &ppib );

   return (ppib->pib_pchcmd);

   }

static void handler( int signum )
   {
   int rc;

   rc = DosStopSession( STOP_SESSION_ALL, sess_id );
   printf( "DosStopSession rc = %d\n", rc );
   rc = DosClose( handle );
   printf( "DosClose rc = %d\n", rc );
   rc = DosDelete( tempfile );
   printf( "DosDelete rc = %d\n", rc );

   exit( EXIT_FAILURE );

   signum = 0;
   }
