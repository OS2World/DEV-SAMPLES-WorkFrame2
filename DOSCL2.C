/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/* COPYRIGHT IBM CORP., 1992                                                */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/*                                                                          */
/* See the DOSCL1.C prolog for a description of the DOSRUN package.         */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <process.h>
#include <stdlib.h>
#include <errno.h>
#include <io.h>
#include <dos.h>
#include <fcntl.h>
#include <share.h>
#include <string.h>

#define EOFFLAG 26

void main( int argc, char *argv[] )
{
   char *buffer;
   int i;
   int handle;
   unsigned int rc;
   int buflen;
   char *temp;
   unsigned int actual;

   /* Open tempfile, deny none, read/write access */
   _dos_open( argv[1], O_RDWR | SH_DENYNO, &handle );

   /* Find the length of invocation string */
   buflen = 0;
   for (i=2; i<argc; i++) {
      buflen += strlen ( argv[i] ) + 1;
   }
   buflen++;

   /* Allocate at least 3 bytes for EOF flag and rc */
   if ( buflen < 3 )
      buflen = 3;

   buffer = (char *)malloc( buflen );

   /* Build invocation string */
   *buffer = '\0';
   for (i=2; i<argc; i++) {
      strcat ( buffer, argv[i] );
      strcat ( buffer, " " );
   } /* endfor */

   /* Redirect stdout and stderr to tempfile */
   dup2( handle, fileno(stdout) );
   dup2( handle, fileno(stderr) );

   /* Invoke the executable */
   rc = system( buffer );

   /* Write EOF flag followed by rc as the last 3 bytes in tempfile */
   *buffer = EOFFLAG;
   temp = (char *)&rc;
   *(buffer+1) = *temp;
   *(buffer+2) = *(temp+1);
   _dos_write( handle, buffer, 3, &actual );

   free( buffer );
   return;

}

