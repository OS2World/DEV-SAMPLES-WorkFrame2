/****************************************************************************/
/*                                                                          */
/* COPYRIGHT IBM CORP., 1992                                                */
/*                                                                          */
/****************************************************************************/
#define INCL_PM

#include <stdio.h>
#include <string.h>
#include <os2.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys\types.h>
#include <sys\stat.h>

#define ERROR_CODE      1
#define NO_ERROR_CODE   0

int splitLine(char *, char *,  int * );

int EXPENTRY ParseError(HWND    hwndErrorBox,
                        USHORT *usStartline,
                        USHORT *usEndline,
                        HWND    hwndFilenames,
                        USHORT *usCurrentFile,
                        VOID   *pvCompilerOptions,
                        ULONG  *pulAction)

   {

   typedef  union  _TAGPARAM
      {
      unsigned long  tagParam ;
      struct
         {
         USHORT  errorLine  ;
         char    offset     ;
         char    fileNumber ;
         } sub  ;
      } TAGPARAM;

   TAGPARAM  mpParam;

   short sIndex , i , insertIndex ;
   short sCount;
   int   lineNumber;
   char work[260+30];
   char pszSourceFile[260+30];


   /**************************************/
   /* Get the index of the selected line */
   /*     Currently, the selected line   */
   /*     must be a valid line.          */
   /**************************************/

   if ((sIndex= SHORT1FROMMR( WinSendMsg(hwndErrorBox,
                              LM_QUERYSELECTION,
                              (MPARAM)NULL,
                              (MPARAM)NULL)))
                 == LIT_NONE)
      return ERROR_CODE;


    /*********************************/
    /* Get the total number of lines */
    /*********************************/

    sCount= SHORT1FROMMR( WinSendMsg(hwndErrorBox,
                          LM_QUERYITEMCOUNT,
                          (MPARAM)NULL,
                          (MPARAM)NULL));


   /*****************************************/
   /* With sIndex, we can retrieve the line */
   /* the user selected, and pick up the    */
   /* name of the file he is interested in. */
   /*****************************************/

   WinSendMsg(hwndErrorBox,
              LM_QUERYITEMTEXT,
              MPFROM2SHORT(sIndex,sizeof(work)),
              MPFROMP(work));


   /**********************************************/
   /* WindSendMsg has stored in 'work' the LIT   */
   /* line. We will get the filename from 'work'.*/
   /**********************************************/

   if ( splitLine(work,pszSourceFile,&lineNumber) )
      return ERROR_CODE ;


   insertIndex = SHORT1FROMMR(WinSendMsg(hwndFilenames,
                                 LM_SEARCHSTRING,
                                 MPFROM2SHORT(LSS_CASESENSITIVE,LIT_FIRST),
                                 MPFROMP(pszSourceFile)));


   if (insertIndex == LIT_ERROR)
      return ERROR_CODE;

   if (insertIndex == LIT_NONE)
       insertIndex = SHORT1FROMMR(WinSendMsg(hwndFilenames,
                                  LM_INSERTITEM,
                                  MPFROMSHORT(LIT_END),
                                  (MPARAM)pszSourceFile));


   /**************************************/
   /* Set the tag for the selected line. */
   /**************************************/

   mpParam.sub.errorLine   = lineNumber  ;
   mpParam.sub.offset      = 0           ;
   mpParam.sub.fileNumber  = *usCurrentFile = insertIndex ;

   WinSendMsg(hwndErrorBox,
              LM_SETITEMHANDLE,
              MPFROMSHORT(sIndex),
              (MPARAM)(ULONG)(mpParam.tagParam));


   *usStartline = i = sIndex ;

   while ( i-- > 0)
      {
      WinSendMsg(hwndErrorBox,
                      LM_QUERYITEMTEXT,
                      MPFROM2SHORT(i,sizeof(work)),
                      MPFROMP(work));

      if ( splitLine(work,pszSourceFile,&lineNumber) )
         {
         ;
         }
      else
         {
         insertIndex = SHORT1FROMMR(WinSendMsg(hwndFilenames,
                                       LM_SEARCHSTRING,
                                       MPFROM2SHORT(LSS_CASESENSITIVE,LIT_FIRST),
                                       MPFROMP(pszSourceFile)));

         if (insertIndex == LIT_ERROR)
            return ERROR_CODE;

         if (insertIndex == LIT_NONE)
             insertIndex = SHORT1FROMMR(WinSendMsg(hwndFilenames,
                        LM_INSERTITEM,
                        MPFROMSHORT(LIT_END),
                        (MPARAM)pszSourceFile));


         /**************************************/
         /* Set the tag for the selected line. */
         /**************************************/

         mpParam.sub.errorLine   = lineNumber  ;
         mpParam.sub.offset      = 0           ;
         mpParam.sub.fileNumber  = insertIndex ;

         WinSendMsg(hwndErrorBox,
                    LM_SETITEMHANDLE,
                    MPFROMSHORT(i),
                    (MPARAM)(ULONG)(mpParam.tagParam));

         }

      *usStartline = i;
      }


   /*******************************************/
   /* Scan from 'LIT' line + 1 and down for   */
   /* all interested lines.                   */
   /*******************************************/

   *usEndline = i = sIndex ;

   while (i++ < sCount)
      {
      WinSendMsg(hwndErrorBox,
                      LM_QUERYITEMTEXT,
                      MPFROM2SHORT(i,sizeof(work)),
                      MPFROMP(work));

      if ( splitLine(work,pszSourceFile,&lineNumber) )
         {
         ;
         }
      else
         {
         insertIndex = SHORT1FROMMR(WinSendMsg(hwndFilenames,
                                       LM_SEARCHSTRING,
                                       MPFROM2SHORT(LSS_CASESENSITIVE,LIT_FIRST),
                                       MPFROMP(pszSourceFile)));

         if (insertIndex == LIT_ERROR)
            return ERROR_CODE;

         if (insertIndex == LIT_NONE)
             insertIndex = SHORT1FROMMR(WinSendMsg(hwndFilenames,
                        LM_INSERTITEM,
                        MPFROMSHORT(LIT_END),
                        (MPARAM)pszSourceFile));


         /**************************************/
         /* Set the tag for the selected line. */
         /**************************************/

         mpParam.sub.errorLine   = lineNumber  ;
         mpParam.sub.offset      = 0           ;
         mpParam.sub.fileNumber  = insertIndex ;

         WinSendMsg(hwndErrorBox,
                    LM_SETITEMHANDLE,
                    MPFROMSHORT(i),
                    (MPARAM)(ULONG)(mpParam.tagParam));

         }

      *usEndline = i;
      }

   return NO_ERROR_CODE;
   }


           /****************************************/
           /*               splitLine              */
           /*                                      */
           /* Takes an input line and returns the  */
           /* filename for the line as well as its */
           /* line number.                         */
           /****************************************/

int splitLine(char * inputLine , char * fileName ,  int * lineNumber)
   {
   char *s , *s2 , *temp;
   struct stat buf;

   /* Scanning for the Left Paranthesis: */

   if ((s = strchr(inputLine,'(')) == NULL)
      return ERROR_CODE ;

   if (s == inputLine)       /* Check if line starts with '(' */
      return ERROR_CODE ;

   *s = '\0' ;

   strcpy(fileName,inputLine) ;

   *s++ = '(' ;               /* Restore char to the original value */


   /****************************/
   /* Check filename is valid. */
   /****************************/


   if ( stat(fileName, &buf) )         /* Must be able to get file info */
      return ERROR_CODE ;

   if ( ! (buf.st_mode &  S_IFREG) )   /* Must be a regukar file */
      return ERROR_CODE ;

   /* Scanning for the Right Paranthesis: */

   if ((s2 = strchr(s,')')) == NULL)
      return ERROR_CODE ;

   /* Check that there is a ':' right of the ')' */

   if ((temp = strchr(s2,':')) == NULL)
      return ERROR_CODE ;

   *s2 = '\0' ;

   /***********************************/
   /* Check if the string is a string */
   /* of all numbers, and it is less  */
   /* than 6 digits in length.        */
   /***********************************/

   temp = s ;

   /***********************************/
   /* A simple test to see that all   */
   /* characters between '(' and ')'  */
   /* are digits or spaces.           */
   /***********************************/

   while ( *temp )
      {
      if ( ! isdigit(*temp)  && !isspace(*temp)  )
         return ERROR_CODE ;
      ++temp;
      }

   *lineNumber = atoi(s) ;

   *s2 = ')' ;           /* Restore char to the original value */

   return NO_ERROR_CODE ;
   }


