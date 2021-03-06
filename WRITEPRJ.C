/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/* COPYRIGHT IBM CORP., 1992                                                */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/*                                                                          */
/*   WriteProject                                                           */
/*                                                                          */
/*      Function to create a new PRJ file                                   */
/*                                                                          */
/*   Parameters:                                                            */
/*                                                                          */
/*      PSZ    projname     -  fully qualified file name                    */
/*      PSZ    projdesc     -  description                                  */
/*      PSZ    directory    -  fully qualified directory name               */
/*      PSZ    exename      -  exe (unqualified)                            */
/*      PSZ    makename     -  make file name (unqualified)                 */
/*      void * compopts     -  NULL                                         */
/*      USHORT compsize     -  0                                            */
/*      void * linkopts     -  NULL                                         */
/*      USHORT linksize     -  0                                            */
/*      void * projlang     -  language eyecatcher                          */
/*      USHORT projlangsize -  language eyecatcher size                     */
/*      void * runopts      -  NULL                                         */
/*      USHORT runsize      -  0                                            */
/*      void * makeopts     -  NULL                                         */
/*      USHORT makesize     -  0                                            */
/*      void * debugopts    -  NULL                                         */
/*      USHORT debugsize    -  0                                            */
/*      PSZ    mask         -  "*.*"                                        */
/*      BOOL   old          -  Reserved                                     */
/*                                                                          */
/*   Returns:                                                               */
/*                                                                          */
/*    MSG_THREAD_OUT_OF_MEMORY -  MEMORY ERROR                              */
/*    MSG_FILE_ERROR           -  FILE ERROR                                */
/*    MSG_NO_ERROR             -  NO ERROR                                  */
/*                                                                          */
/****************************************************************************/

#define MSG_NO_ERROR 0
#define MSG_FILE_ERROR 8
#define MSG_THREAD_OUT_OF_MEMORY 256

USHORT WriteProject( PSZ    projname,
                     PSZ    projdesc,
                     PSZ    directory,
                     PSZ    exename, 
                     PSZ    makename,
                     void * compopts,
                     USHORT compsize,
                     void * linkopts,
                     USHORT linksize,
                     void * projlang,
                     USHORT projlangsize,
                     void * runopts,
                     USHORT runsize,
                     void * makeopts,
                     USHORT makesize,
                     void * debugopts,
                     USHORT debugsize,
                     PSZ    mask,
                     BOOL   old)
   {
   char * work;
   char * workstart;
   HFILE  outfile;
   USHORT ActionTaken;
   USHORT descsize,
          dirsize,
          exesize,
          maksize,
          masksize;
   USHORT SizeRequired,  
          rc;
   int    byteswritten;

   descsize     = strlen(projdesc)+1;
   dirsize      = strlen(directory)+1;
   exesize      = strlen(exename)+1;
   maksize      = strlen(makename)+1;
   masksize     = strlen(mask)+1;
   SizeRequired = descsize+dirsize+exesize+maksize+
                  masksize+compsize+linksize+projlangsize+
                  runsize+makesize+debugsize+14;

   work = malloc(SizeRequired);
   if (work==NULL)
      {
      return MSG_THREAD_OUT_OF_MEMORY;
      }

   workstart=work;
   rc = DosOpen(projname,
                (PHFILE)&outfile,
                &ActionTaken,
                0L,                           /* FileSize            */
                0,                            /* FileAttribute       */
                OPEN_ACTION_CREATE_IF_NEW |
                OPEN_ACTION_REPLACE_IF_EXISTS,/* OpenFlag            */
                OPEN_SHARE_DENYREADWRITE  |
                OPEN_ACCESS_READWRITE,        /* OpenMode            */
                0L);                          /* ReservedDWord       */

   if (rc !=0)
      {
      free(workstart);
      return MSG_FILE_ERROR ;
      }

   /***************************************************/
   /*   Store the Project type                        */
   /***************************************************/
   byteswritten = 0xFFFE;
   memcpy(work,&byteswritten,2); 
   work += 2;

   /***************************************************/
   /*   Store the Project description                 */
   /***************************************************/
   memcpy(work,projdesc,descsize); 
   work += descsize;

   /***************************************************/
   /*   Store the Project directory                   */
   /***************************************************/
   strupr(directory);
   memcpy(work,directory,dirsize);
   work += dirsize;

   /***************************************************/
   /*   Store the Project executable                  */
   /***************************************************/
   strupr(exename);
   memcpy(work,exename  ,exesize);
   work += exesize;

   /***************************************************/
   /*   Store the Project make  file                  */
   /***************************************************/
   strupr(makename);
   memcpy(work,makename  ,maksize);
   work += maksize;

   /***************************************************/
   /*   Store the Project selection mask              */
   /***************************************************/
   memcpy(work,mask      ,masksize);
   work += masksize;

   /***************************************************/
   /*   Store the Project compile options             */
   /***************************************************/
   memcpy(work,&compsize,2);
   work +=2;
   memcpy(work,compopts,compsize);
   work += compsize;

   /***************************************************/
   /*   Store the Project link    options             */
   /***************************************************/
   memcpy(work,&linksize,2);
   work +=2;
   memcpy(work,linkopts,linksize);
   work += linksize;

   /***************************************************/
   /*   Store the Project  run    options             */
   /***************************************************/
   memcpy(work,&runsize,2);
   work +=2;
   memcpy(work,runopts,runsize);
   work += runsize;

   /***************************************************/
   /*   Store the Project  make   options             */
   /***************************************************/
   memcpy(work,&makesize,2);
   work +=2;
   memcpy(work,makeopts,makesize);
   work += makesize;

   /***************************************************/
   /*   Store the Project debug   options             */
   /***************************************************/
   memcpy(work,&debugsize,2);
   work +=2;
   memcpy(work,debugopts,debugsize);
   work += debugsize;

   /***************************************************/
   /*   Store the Project options language            */
   /***************************************************/
   memcpy(work,&projlangsize,2);
   work +=2;
   memcpy(work,projlang,projlangsize);
   work += projlangsize;

   /***************************************************/
   /*   Write the file                                */
   /***************************************************/
   rc = DosWrite(outfile,workstart,SizeRequired,&byteswritten);
   if (rc !=0 || SizeRequired != (USHORT)byteswritten)
      {
      free(workstart);
      DosClose(outfile);
      return MSG_FILE_ERROR;
      }

   free(workstart);

   if ((rc=DosClose(outfile)) != 0)
      {
      return MSG_FILE_ERROR;
      }

   return MSG_NO_ERROR;
   }

