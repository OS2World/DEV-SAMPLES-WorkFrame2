/****************************************************************************/
/*                                                                          */
/* COPYRIGHT IBM CORP., 1992                                                */
/*                                                                          */
/****************************************************************************/
#define INCL_DOSDEVICES
#define INCL_DOSMISC
#define INCL_ERRORS
#define INCL_WINHELP
#define INCL_DOSFILEMGR
#define INCL_WIN
#define INCL_GPI
#define INCL_NLS
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "IBMCCOMP.H"

BOOL OptActive;
BOOL CGenActive;
BOOL CtlActive;
BOOL PrepActive;
BOOL OutfActive;
BOOL ListActive;
BOOL SctlActive;
HAB   hab;                              /* anchor block handle          */
HWND  handleWise;
HMODULE handleDLL;
RECTL parent;
RECTL child;
RECTL tb;

/* The following variables are used to define the state of the options */

static ULONG * complen;
static ULONG outlen;
static COMPOPTIONS * ivs;
static PSZ outstring;
static PSZ saveFilename;
/* Used by Optimization Dialog */
static HWND hwndOptAll, hwndOptNone,hwndOptions[9];
static BOOL Optall,OptNone,OptOptions[9];

/* Used By Code Gen (and storage customization) Dialogs */
static HWND hwndGenNstack;
static BOOL GenPascal,GenPm,GenNstack,GenConst,Genthresh;
static char Genthreshamount[10];
static short GenModelix,GenFloatix,GenCustCodeix,GenCustDataix,GenCustSegmix;

/* Used by Control Dialog */
static short CtlObjTypeix;
static BOOL CtlDebNone,CtlDebSym,CtlDebLine, CtlDefLib;
static HWND hwndDebNone,hwndDebSym,hwndDebLine;
static char CtlNameLen[4],CtlVersion[256],CtlDataSeg[41],CtlCodeSeg[41];
static BOOL CtlUseB1;

/* Used by Preprocessor Dialog */

static BOOL boolLstFile, boolListSOWL, boolListSONL,boolPrepComment,boolPrepExclude;
static BOOL boolPrepUndAll;
static char PrepIncludeDir[15][260];
static int    PrepInclCount;
static int    PrepDefCount;
static int    PrepUnDefCount;
static char PrepDefines[15][260];
static char PrepUndefines[5][260];

/* used by Outfile Dialog */

static BOOL OutSrcList, OutMap, OutObjList, OutAsmList, OutComList;
static char OutSrcName[260],OutExeName[260],OutObjName[260],OutMapName[260],OutObjFname[260];

/* Used by Listing Dialog */
static short LstWarnix;
static char LstLine[4];
static char LstPage[4];
static char LstTitle[100];
static char LstSubTitle[100];

/* Used by Source Control Dialog */

BOOL SctlDext, SctlSyntax, SctlDecl, SctlConly, SctlJ, SctlCaseI;
short SctlPackix;

ULONG  * SaveAction;

LONG returncode;

LONG APIENTRY GETCOMPILEOPTS(HWND hWise,PVOID Parms, ULONG *MaxParml, PSZ PgmName, PSZ Outstring, ULONG Outlen,HMODULE hModHandle, PSZ Profile, PSZ File, ULONG * Action);
MRESULT EXPENTRY MAINPROC( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY OPTPROC( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY GENPROC( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY GENCUSTPROC( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY CTLPROC( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY PREPPROC( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY OUTFPROC( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY LISTPROC( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY SCTLPROC( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2);

/* internal functions */

static void Disable_ALL_Checks(void);
static void Enable_ALL_Checks(void) ;
static void ReturnOutputString(void);

LONG APIENTRY GETCOMPILEOPTS(HWND hWise,PVOID Parms, ULONG *MaxParml, PSZ PgmName, PSZ Outstring, ULONG Outlen,HMODULE hModHandle, PSZ Profile, PSZ Filename, ULONG * Action)
{
   char * work;
   char * work2;
   int i;
  handleWise = hWise;
  handleDLL  = hModHandle;
  saveFilename = Filename;
  outstring= Outstring;
  complen=MaxParml;
  outlen = Outlen;

  returncode = SUCCESS;

     if ( *MaxParml < sizeof(COMPOPTIONS) )
     {
        char msg[100];

        WinLoadString ( HWND_DESKTOP, hModHandle, MSG_CMPOPTSMALL, 100, msg);

        WinMessageBox(  HWND_DESKTOP,
                        hWise,
                        msg,
                        NULL,
                        MSG_CMPOPTSMALL,
                        MB_MOVEABLE | MB_OK | MB_ERROR |  MB_APPLMODAL);

         return ERRORCONDITION;
     }

/*  WinQueryWindowRect(handleWise,&parent); */
  WinQueryWindowRect(HWND_DESKTOP,&parent);
     OptActive = FALSE;
     CGenActive = FALSE;
     CtlActive = FALSE;
     PrepActive = FALSE;
     OutfActive = FALSE;
     ListActive = FALSE;
     SctlActive = FALSE;

   ivs=(COMPOPTIONS *)Parms;

   if (*Action & USEDEFAULTS)
   {
     Optall=0;
     OptNone=0;
     OptOptions[0]=1;
     for (i=1;i<9;i++)
        OptOptions[i]=0;
     GenPascal=GenPm=GenNstack=GenConst=Genthresh=0;
     GenModelix=0;
     GenFloatix=3;
     GenCustCodeix=GenCustDataix=GenCustSegmix=0;
     Genthreshamount[0]='\0';

     CtlObjTypeix=0;
     CtlDefLib=0;
     CtlUseB1=0;
     CtlDebNone=1,
     CtlDebSym=CtlDebLine=0;
     strcpy(CtlNameLen,"31");
     CtlVersion[0]=CtlDataSeg[0]=CtlCodeSeg[0]='\0';

     boolLstFile = 0;
     boolListSOWL= 0;
     boolListSONL= 0;
     boolPrepComment= 0;
     boolPrepExclude= 0;
     boolPrepUndAll = 0;
     PrepInclCount=0;
     PrepDefCount =0;
     PrepUnDefCount =0;

     OutSrcList=0;
     OutMap  =0;
     OutObjList=0;
     OutAsmList=0;
     OutComList=0;

     OutSrcName[0]=OutExeName[0]=OutObjName[0]=OutMapName[0]=OutObjFname[0]= '\0';


     LstWarnix = 2;
     LstLine[0]= LstPage[0]= LstTitle[0] = LstSubTitle[0] = '\0';


     SctlDext=0;
     SctlSyntax=0;
     SctlDecl=0;
     SctlConly=0;
     SctlJ=0;
     SctlCaseI=0;
     SctlPackix=2;
    }
    else
    {
     Optall=ivs->optmax;
     OptNone=ivs->optnone;
     OptOptions[0]=ivs->optspeed    ;
     OptOptions[1]=ivs->optloop     ;
     OptOptions[2]=ivs->optprecise  ;
     OptOptions[3]=ivs->optaliasfn  ;
     OptOptions[4]=ivs->optspace    ;
     OptOptions[5]=ivs->optialias   ;
     OptOptions[6]=ivs->optintrins  ;
     OptOptions[7]=ivs->optunsafe   ;
     OptOptions[8]=ivs->optinliner  ;
     GenPascal= ivs->PascalLink     ;
     GenPm=     ivs->PMLink         ;
     GenNstack= ivs->NoStackPrb     ;
     GenConst=  ivs->StringinCs     ;
     Genthresh= ivs->DataThresh     ;
     GenModelix=ivs->MemoryModel    ;
     GenFloatix=ivs->FloatOption    ;
     GenCustCodeix=ivs->CustModelCode;
     GenCustDataix=ivs->CustModelData;
     GenCustSegmix=ivs->CustModelSetS;
     if (ivs->DataThreshAmt==0)
        Genthreshamount[0]='\0';
     else
        sprintf(Genthreshamount,"%d",ivs->DataThreshAmt);

     CtlObjTypeix=ivs->ObjectisExe;
     CtlDefLib=ivs->RemoveDefLib;
     CtlUseB1=ivs->UseB1;
     CtlDebLine=ivs->DebLineNum;
     CtlDebSym= ivs->DebSymDeb ;
     if (ivs->DebLineNum==0 && ivs->DebSymDeb==0)
       CtlDebNone=1;
     else
       CtlDebNone=0;
     if (ivs->ExternNameLgth==0)
       strcpy(CtlNameLen,"31");
     else
        sprintf(CtlNameLen,"%d",ivs->ExternNameLgth);
     strcpy(CtlVersion,ivs->stringtablestart+ivs->VersionStringoff);
     strcpy(CtlDataSeg,ivs->stringtablestart+ivs->DsegNameoff);
     strcpy(CtlCodeSeg,ivs->stringtablestart+ivs->CsegNameoff);

     if (ivs->Prepoutput==1)
        boolLstFile = 1;
     else
        boolLstFile = 0;
     if (ivs->Prepoutput==2)
        boolListSOWL= 1;
     else
        boolListSOWL= 0;
     if (ivs->Prepoutput==3)
        boolListSONL= 1;
     else
        boolListSONL= 0;
     boolPrepComment= ivs->SaveComments;

     boolPrepExclude= ivs->ExcludeStd ;
     boolPrepUndAll = ivs->UndefineAll;

     PrepInclCount= ivs->NumIncludePaths;

     work = ivs->stringtablestart+ivs->IncludePathoff;
     for (i=0;i<PrepInclCount;i++)
     {
       work2=PrepIncludeDir[i];
       while (*work != '\0')
        {
           *work2=*work;
           ++work;
           ++work2;
        }

        *work2='\0';
        strcat(work2,CRLF);
        ++work ;
     }
     PrepDefCount = ivs->NumDefinedMacros;
     work = ivs->stringtablestart+ivs->DefinedMacrooff;
     for (i=0;i<PrepDefCount;i++)
     {
       work2=PrepDefines[i];
       while (*work != '\0')
        {
           *work2=*work;
           ++work;
           ++work2;
        }
        *work2='\0';
        strcat(work2,CRLF);
        ++work ;
     }

     PrepUnDefCount=ivs->NumUnDefinedMacros;
     work = ivs->stringtablestart+ivs->UnDefinedMacrooff;
     for (i=0;i<PrepUnDefCount;i++)
     {
       work2=PrepUndefines[i];
       while (*work != '\0')
        {
           *work2=*work;
           ++work;
           ++work2;
        }
        *work2='\0';
        strcat(work2,CRLF);
        ++work ;
     }
     OutSrcList=ivs->SrcListRequired ;
     OutMap    =ivs->MapFileRequired ;
     if (ivs->ObjectListReq == 1)
       OutObjList=1;
     else
       OutObjList=0;
     if (ivs->ObjectListReq ==2)
       OutAsmList=1;
     else
       OutAsmList=0;
     if (ivs->ObjectListReq ==3)
       OutComList=1;
     else
       OutComList=0;

     strcpy(OutSrcName, ivs->stringtablestart+ivs->SrcListFileoff );
     strcpy(OutMapName, ivs->stringtablestart+ivs->MapListFileoff );
     strcpy(OutObjFname,ivs->stringtablestart+ivs->ObjListFileoff );
     strcpy(OutObjName, ivs->stringtablestart+ivs->ObjFileNameoff );
     strcpy(OutExeName,ivs->stringtablestart+ivs->ExeFileNameoff );


     LstWarnix = ivs->WarningLevel;
     if (ivs->LineWidth==0)
       LstLine[0]= '\0';
     else
       sprintf(LstLine,"%d",ivs->LineWidth);
     if (ivs->PageLength==0)
       LstPage[0]= '\0';
     else
       sprintf(LstPage,"%d",ivs->PageLength);
     strcpy(LstTitle,ivs->stringtablestart+ivs->Titlestringoff );
     strcpy(LstSubTitle,ivs->stringtablestart+ivs->SubTitlestringoff );


     SctlConly=  ivs->CompileOnly    ;
     SctlSyntax= ivs->SyntaxCheck    ;
     SctlDecl=   ivs->GenerateDecl   ;
     SctlDext=   ivs->DisableExten   ;
     SctlJ=      ivs->CharisUnsigned ;
     SctlCaseI=  ivs->CaseInsensitive;
     SctlPackix= ivs->StructPack     ;
    }
   if (*Action & NODLGS)
     {
         ReturnOutputString();
         return returncode;
      }
   hab = WinInitialize((USHORT)NULL);            /* initialize                      */

  SaveAction=Action;
  if (WinDlgBox( HWND_DESKTOP,
             handleWise,
             MAINPROC,
             handleDLL,
             MAINDLG,
             NULL ) == TRUE)
      {
         WinTerminate(hab);
         return returncode;
      }
   else
    {
         WinTerminate(hab);
         return CANCEL;
    }
}




MRESULT EXPENTRY MAINPROC( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg) {
   case WM_INITDLG:
      WinQueryWindowRect(WinWindowFromID(hwndDlg,FID_TITLEBAR),&tb);
      WinSetWindowPos(hwndDlg,HWND_TOP,(short)(parent.xLeft+5L),(short)(parent.yBottom+5L),
               0,0,SWP_MOVE | SWP_SHOW);
      break;
   case WM_COMMAND:

      switch (SHORT1FROMMP(mp1)) {
      case PRPRDLG:
        WinEnableWindow(WinWindowFromID(hwndDlg,PRPRDLG),
                   FALSE);
        WinDlgBox( HWND_DESKTOP,
                   handleWise,
                   PREPPROC,
                   handleDLL,
                   PREPDLG,
                   NULL );
        WinEnableWindow(WinWindowFromID(hwndDlg,PRPRDLG),
                   TRUE);
         break;
      case LSTDLG:
        WinEnableWindow(WinWindowFromID(hwndDlg,LSTDLG),
                   FALSE);
        WinDlgBox( HWND_DESKTOP,
                   handleWise,
                   LISTPROC,
                   handleDLL,
                   LISTDLG,
                   NULL );
        WinEnableWindow(WinWindowFromID(hwndDlg,LSTDLG),
                   TRUE);
         break;
      case SORCDLG:
        WinEnableWindow(WinWindowFromID(hwndDlg,SORCDLG),
                   FALSE);
        WinDlgBox( HWND_DESKTOP,
                   handleWise,
                   SCTLPROC,
                   handleDLL,
                   SCTLDLG,
                   NULL );
        WinEnableWindow(WinWindowFromID(hwndDlg,SORCDLG),
                   TRUE);
         break;
      case FILEDLG:
        WinEnableWindow(WinWindowFromID(hwndDlg,FILEDLG),
                   FALSE);
        WinDlgBox( HWND_DESKTOP,
                   handleWise,
                   OUTFPROC,
                   handleDLL,
                   OUTFDLG,
                   NULL );
        WinEnableWindow(WinWindowFromID(hwndDlg,FILEDLG),
                   TRUE);
         break;
      case CTLDLG:
        WinEnableWindow(WinWindowFromID(hwndDlg,CTLDLG),
                   FALSE);
        WinDlgBox( HWND_DESKTOP,
                   handleWise,
                   CTLPROC,
                   handleDLL,
                   CTLDLG,
                   NULL );
        WinEnableWindow(WinWindowFromID(hwndDlg,CTLDLG),
                   TRUE);
         break;
      case CGENDLG:
        WinEnableWindow(WinWindowFromID(hwndDlg,CGENDLG),
                   FALSE);
        WinDlgBox( HWND_DESKTOP,
                   handleWise,
                   GENPROC,
                   handleDLL,
                   CGENDLG,
                   NULL );
        WinEnableWindow(WinWindowFromID(hwndDlg,CGENDLG),
                   TRUE);
         break;
      case OPTMDLG:
        WinEnableWindow(WinWindowFromID(hwndDlg,OPTMDLG),
                   FALSE);
        WinDlgBox( HWND_DESKTOP,
                   handleWise,
                   OPTPROC,
                   handleDLL,
                   OPTMDLG,
                   NULL );
        WinEnableWindow(WinWindowFromID(hwndDlg,OPTMDLG),
                   TRUE);
         break;
      case DID_OK:
         ReturnOutputString();
         *SaveAction |=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,SAVELSE ),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
         WinDismissDlg(hwndDlg,TRUE);   /* Returns TRUE to WinDlgBox in the Window */
           break;
      case DID_CANCEL:
         WinDismissDlg(hwndDlg,FALSE);  /* Returns FALSE to WinDlgBox in the Window */
         break;
      default:
         break;
      } /* endswitch */
      break;
   default:
      return WinDefDlgProc( hwndDlg, msg, mp1, mp2 );
   } /* endswitch */
   return FALSE;
}


MRESULT EXPENTRY OPTPROC( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2)
{
  USHORT checkbox;
  int i;
  short Adjust;
   switch (msg) {
   case WM_INITDLG:
      WinQueryWindowRect(hwndDlg,&child);
      Adjust=(OptActive+CGenActive+CtlActive+PrepActive
                      +OutfActive+ListActive+SctlActive)*(USHORT)tb.yTop;

      WinSetWindowPos(hwndDlg,HWND_TOP,(short)(parent.xRight-child.xRight-1)
                      ,(short)(parent.yTop-tb.yTop-Adjust-child.yTop)
                      ,0,0,SWP_MOVE | SWP_SHOW);

      hwndOptNone = WinWindowFromID(hwndDlg,OPT_NOPT);
      hwndOptAll = WinWindowFromID(hwndDlg,OPT_MAX);
      hwndOptions[0] = WinWindowFromID(hwndDlg,OPT_SPEED  );
      hwndOptions[1] = WinWindowFromID(hwndDlg,OPT_LOOP   );
      hwndOptions[2] = WinWindowFromID(hwndDlg,OPT_PREC   );
      hwndOptions[3] = WinWindowFromID(hwndDlg,OPT_ALIAS  );
      hwndOptions[4] = WinWindowFromID(hwndDlg,OPT_SPACE  );
      hwndOptions[5] = WinWindowFromID(hwndDlg,OPT_IALIAS );
      hwndOptions[6] = WinWindowFromID(hwndDlg,OPT_INTRIN );
      hwndOptions[7] = WinWindowFromID(hwndDlg,OPT_SAFE   );
      hwndOptions[8] = WinWindowFromID(hwndDlg,OPT_DINLNE );
      WinSendMsg(hwndOptNone,
                 BM_SETCHECK,
                 MPFROMSHORT(OptNone),
                 NULL);
      WinSendMsg(hwndOptAll,
                 BM_SETCHECK,
                 MPFROMSHORT(Optall),
                 NULL);
      if (Optall ==1 || OptNone ==1)
               Disable_ALL_Checks();
      else
         {
           for (i=0;i<9;i++)
            {
             WinSendMsg(hwndOptions[i],
                       BM_SETCHECK,
                      MPFROMSHORT(OptOptions[i]),
                      NULL);
            }
         }
     OptActive = TRUE;

      break;
   case WM_CONTROL:
       if  (HIUSHORT(mp1)==BN_CLICKED)
          {
            switch (LOUSHORT((ULONG)mp1)) {
              case OPT_NOPT:
                    OptNone = (OptNone ^1) &1;
                    WinSendMsg(hwndOptNone,
                               BM_SETCHECK,
                               MPFROMSHORT(OptNone),
                               NULL);
                    /* Clear All option */
                    if (OptNone ==1)
                     {
                      Optall = 0;
                      WinSendMsg(hwndOptAll,
                                 BM_SETCHECK,
                                 MPFROMSHORT(0),
                                 NULL);
                      Disable_ALL_Checks();
                     }
                   else
                      Enable_ALL_Checks();
                break;
              case OPT_MAX:
                    Optall  = (Optall ^1) &1;
                    WinSendMsg(hwndOptAll,
                               BM_SETCHECK,
                               MPFROMSHORT(Optall),
                               NULL);
                    if (Optall ==1)
                     {
                      OptNone = 0;
                      WinSendMsg(hwndOptNone,
                                 BM_SETCHECK,
                                 MPFROMSHORT(0),
                                 NULL);
                      Disable_ALL_Checks();
                      CtlDebNone = TRUE;
                      CtlDebSym  = FALSE;
                      CtlDebLine = FALSE;
                      if (CtlActive==TRUE)
                      {
                       WinSendMsg(hwndDebNone,
                                  BM_SETCHECK,
                                  MPFROMSHORT(CtlDebNone),
                                  NULL);
                       WinSendMsg(hwndDebSym ,
                                  BM_SETCHECK,
                                  MPFROMSHORT(0),
                                  NULL);
                       WinSendMsg(hwndDebLine,
                                  BM_SETCHECK,
                                  MPFROMSHORT(0),
                                  NULL);
                      }
                      GenNstack = 1;
                      if (CGenActive==TRUE)
                      {
                       WinSendMsg(hwndGenNstack,
                                  BM_SETCHECK,
                                  MPFROMSHORT(1),
                                  NULL);
                      }
                     }
                   else
                      Enable_ALL_Checks();
                break;
             default:
                    checkbox = LOUSHORT((ULONG)mp1)-OPT_SPEED;
                    OptOptions[checkbox] = (OptOptions[checkbox] ^1) &1;
                    WinSendMsg(hwndOptions[checkbox],
                               BM_SETCHECK,
                               MPFROMSHORT(OptOptions[checkbox]),
                               NULL);
                      if (CtlActive == TRUE && CtlDebNone == FALSE)
                      {
                       WinSendMsg(hwndDebNone,
                                  BM_SETCHECK,
                                  MPFROMSHORT(1),
                                  NULL);
                       WinSendMsg(hwndDebSym ,
                                  BM_SETCHECK,
                                  MPFROMSHORT(0),
                                  NULL);
                       WinSendMsg(hwndDebLine,
                                  BM_SETCHECK,
                                  MPFROMSHORT(0),
                                  NULL);
                      }
                      CtlDebNone = TRUE;
                      CtlDebSym  = FALSE;
                      CtlDebLine = FALSE;
                break;
             }
          }
        else
            return(WinDefDlgProc(hwndDlg,
                                  msg,
                                  mp1,
                                  mp2));
      break;
   case WM_COMMAND:

      switch (SHORT1FROMMP(mp1)) {
      case DID_OK:
         OptActive = FALSE;
         WinDismissDlg(hwndDlg,TRUE);   /* Returns TRUE to WinDlgBox in the Window */
           break;
      case DID_CANCEL:
         OptActive = FALSE;
         WinDismissDlg(hwndDlg,FALSE);   /* Returns TRUE to WinDlgBox in the Window */
         break;
      case OPTION_DEFAULT:
          if (Optall ==1 || OptNone ==1)
                   Enable_ALL_Checks();
          Optall=0;
          OptNone=0;
          OptOptions[0]=1;
          for (i=1;i<9;i++)
             OptOptions[i]=0;
          WinSendMsg(hwndOptNone,
                     BM_SETCHECK,
                     MPFROMSHORT(0),
                     NULL);
          WinSendMsg(hwndOptAll,
                     BM_SETCHECK,
                     MPFROMSHORT(0),
                     NULL);
           for (i=0;i<9;i++)
             {
              WinSendMsg(hwndOptions[i],
                        BM_SETCHECK,
                       MPFROMSHORT(OptOptions[i]),
                       NULL);
             }
             break;
      case OPTION_RESET:
         break;
      default:
         break;
      } /* endswitch */
      break;
   default:
      return WinDefDlgProc( hwndDlg, msg, mp1, mp2 );
   } /* endswitch */
   return FALSE;
}


static void Disable_ALL_Checks(void)
{
   int i;
   for (i=0;i<9 ;i++ ) {
      if (OptOptions[i]==1)
       {
          OptOptions[i]=0;
          WinSendMsg(hwndOptions[i],
                    BM_SETCHECK,
                    MPFROMSHORT(0),
                    NULL);
       }
      /* set the checkbox gray */
      WinEnableWindow(hwndOptions[i],
                 FALSE);

   } /* endfor */
}
static void Enable_ALL_Checks(void)
{
   int i;
   for (i=0;i<9 ;i++ ) {
      /* set the checkbox gray */
      WinEnableWindow(hwndOptions[i],
                 TRUE);

   } /* endfor */
}
MRESULT EXPENTRY GENPROC( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   short Adjust;
   switch (msg) {
   case WM_INITDLG:
      WinQueryWindowRect(hwndDlg,&child);
      Adjust=(OptActive+CGenActive+CtlActive+PrepActive
                      +OutfActive+ListActive+SctlActive)*(USHORT)tb.yTop;

      WinSetWindowPos(hwndDlg,HWND_TOP,(short)(parent.xRight-child.xRight-1)
                      ,(short)(parent.yTop-tb.yTop-Adjust-child.yTop)
                      ,0,0,SWP_MOVE | SWP_SHOW);

      WinSendMsg(WinWindowFromID(hwndDlg,GENFLOATST+GenFloatix),
                 BM_SETCHECK,
                 MPFROMSHORT(1),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,GENMODELST + GenModelix),
                 BM_SETCHECK,
                 MPFROMSHORT(1),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,GENPASCAL ),
                 BM_SETCHECK,
                 MPFROMSHORT(GenPascal),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,GENPM     ),
                 BM_SETCHECK,
                 MPFROMSHORT(GenPm),
                 NULL);
      hwndGenNstack =  WinWindowFromID(hwndDlg,GENSTACK  );
      WinSendMsg(WinWindowFromID(hwndDlg,GENSTACK  ),
                 BM_SETCHECK,
                 MPFROMSHORT(GenNstack),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,GENCONST  ),
                 BM_SETCHECK,
                 MPFROMSHORT(GenConst ),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,GENTHRESH ),
                 BM_SETCHECK,
                 MPFROMSHORT(Genthresh),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,GENTHRESH_ENTRY ),
                 EM_SETTEXTLIMIT,
                 MPFROMSHORT(8),
                 NULL);
      WinSetWindowText(WinWindowFromID(hwndDlg,GENTHRESH_ENTRY), Genthreshamount);
     CGenActive = TRUE;

      break;
     case WM_CONTROL:
       if  (HIUSHORT(mp1)==BN_CLICKED)
          {
            if (LOUSHORT((ULONG)mp1)==GENMODELCUST)
              {
               WinDlgBox( HWND_DESKTOP,
                          hwndDlg,
                          GENCUSTPROC,
                          handleDLL,
                          GENCUSTDLG,
                          NULL );
              }
          }
      break;
     case WM_COMMAND:

      switch (SHORT1FROMMP(mp1)) {
         case DID_OK:
            GenFloatix=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,GENFLOATST),
                       BM_QUERYCHECKINDEX,
                       NULL,
                       NULL));
             --GenFloatix;
            GenModelix=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,GENMODELST),
                       BM_QUERYCHECKINDEX,
                       NULL,
                       NULL));
             --GenModelix;
            GenPascal=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,GENPASCAL ),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            GenPm=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,GENPM     ),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            GenNstack=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,GENSTACK  ),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            GenConst=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,GENCONST  ),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            Genthresh=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,GENTHRESH ),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
             if (Genthresh)
                {
                 WinQueryWindowText(WinWindowFromID(hwndDlg,GENTHRESH_ENTRY ),
                      10,
                      Genthreshamount);
                }
             else
                Genthreshamount[0]='\0';
             CGenActive = FALSE;
             WinDismissDlg(hwndDlg,TRUE);   /* Returns TRUE to WinDlgBox in the Window */
              break;
         case DID_CANCEL:
            CGenActive = FALSE;
            WinDismissDlg(hwndDlg,FALSE);   /* Returns TRUE to WinDlgBox in the Window */
            break;
         case OPTION_DEFAULT:
             GenPascal=GenPm=GenNstack=GenConst=Genthresh=0;
             GenModelix=0;
             GenFloatix=3;
             Genthreshamount[0]='\0';
             WinSendMsg(WinWindowFromID(hwndDlg,GENMODELST+GenModelix),
                        BM_SETCHECK,
                        MPFROMSHORT(1),
                        NULL);
             WinSendMsg(WinWindowFromID(hwndDlg,GENFLOATST+GenFloatix),
                        BM_SETCHECK,
                        MPFROMSHORT(1),
                        NULL);
             WinSendMsg(WinWindowFromID(hwndDlg,GENPASCAL ),
                        BM_SETCHECK,
                        MPFROMSHORT(GenPascal),
                        NULL);
             WinSendMsg(WinWindowFromID(hwndDlg,GENPM     ),
                        BM_SETCHECK,
                        MPFROMSHORT(GenPm),
                        NULL);
             WinSendMsg(WinWindowFromID(hwndDlg,GENSTACK  ),
                        BM_SETCHECK,
                        MPFROMSHORT(GenNstack),
                        NULL);
             WinSendMsg(WinWindowFromID(hwndDlg,GENCONST  ),
                        BM_SETCHECK,
                        MPFROMSHORT(GenConst ),
                        NULL);
             WinSendMsg(WinWindowFromID(hwndDlg,GENTHRESH ),
                        BM_SETCHECK,
                        MPFROMSHORT(Genthresh),
                        NULL);
             WinSendMsg(WinWindowFromID(hwndDlg,GENTHRESH_ENTRY ),
                        EM_SETTEXTLIMIT,
                        MPFROMSHORT(8),
                        NULL);
             WinSetWindowText(WinWindowFromID(hwndDlg,GENTHRESH_ENTRY), Genthreshamount);
            break;
         case OPTION_RESET:
           break;
         default:
         break;
         } /* endswitch */
      break;
     default:
      return WinDefDlgProc( hwndDlg, msg, mp1, mp2 );
   } /* endswitch */
   return FALSE;
}
MRESULT EXPENTRY GENCUSTPROC( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   short Adjust;
   switch (msg) {
     case WM_INITDLG:
      WinQueryWindowRect(hwndDlg,&child);
      Adjust=(OptActive+CGenActive+CtlActive+PrepActive
                      +OutfActive+ListActive+SctlActive)*(USHORT)tb.yTop;


      WinSetWindowPos(hwndDlg,HWND_TOP,(short)(parent.xRight-child.xRight-1)
                      ,(short)(parent.yTop-tb.yTop-Adjust-child.yTop)
                      ,0,0,SWP_MOVE | SWP_SHOW);
      WinSendMsg(WinWindowFromID(hwndDlg,GENCUSTSHRT+GenCustCodeix),
                 BM_SETCHECK,
                 MPFROMSHORT(1),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,GENCUSTNEAR+GenCustDataix),
                 BM_SETCHECK,
                 MPFROMSHORT(1),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,GENCUSTSSDS+GenCustSegmix),
                 BM_SETCHECK,
                 MPFROMSHORT(1),
                 NULL);
      break;
     case WM_COMMAND:

      switch (SHORT1FROMMP(mp1)) {
         case DID_OK:
            GenCustCodeix=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,GENCUSTSHRT),
                       BM_QUERYCHECKINDEX,
                       NULL,
                       NULL));
             --GenCustCodeix;
            GenCustDataix=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,GENCUSTNEAR),
                       BM_QUERYCHECKINDEX,
                       NULL,
                       NULL));
             --GenCustDataix;
            GenCustSegmix=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,GENCUSTSSDS),
                       BM_QUERYCHECKINDEX,
                       NULL,
                       NULL));
             --GenCustSegmix;
             break;
         default:
             break;
         } /* endswitch */
     default:
      return WinDefDlgProc( hwndDlg, msg, mp1, mp2 );
   } /* endswitch */
   return FALSE;
}
MRESULT EXPENTRY CTLPROC( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2)
{
  short Adjust;
  int i;
   switch (msg) {
   case WM_INITDLG:
      WinQueryWindowRect(hwndDlg,&child);
      Adjust=(OptActive+CGenActive+CtlActive+PrepActive
                      +OutfActive+ListActive+SctlActive)*(USHORT)tb.yTop;

      WinSetWindowPos(hwndDlg,HWND_TOP,(short)(parent.xRight-child.xRight-1)
                      ,(short)(parent.yTop-tb.yTop-Adjust-child.yTop)
                      ,0,0,SWP_MOVE | SWP_SHOW);
      hwndDebNone = WinWindowFromID(hwndDlg,CTLDEBNONE);
      hwndDebSym  = WinWindowFromID(hwndDlg,CTLDEBSYM );
      hwndDebLine = WinWindowFromID(hwndDlg,CTLDEBLINE);
      WinSendMsg(WinWindowFromID(hwndDlg,CTLTYPEEXE+CtlObjTypeix),
                 BM_SETCHECK,
                 MPFROMSHORT(1),
                 NULL);
      WinSendMsg(hwndDebNone,
                 BM_SETCHECK,
                 MPFROMSHORT(CtlDebNone),
                 NULL);
      WinSendMsg(hwndDebLine,
                 BM_SETCHECK,
                 MPFROMSHORT(CtlDebLine),
                 NULL);
      WinSendMsg(hwndDebSym,
                 BM_SETCHECK,
                 MPFROMSHORT(CtlDebSym),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,CTLNMLGTH ),
                 EM_SETTEXTLIMIT,
                 MPFROMSHORT(3),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,CTLDEFLIB),
                 BM_SETCHECK,
                 MPFROMSHORT(CtlDefLib),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,CTLUSEB1 ),
                 BM_SETCHECK,
                 MPFROMSHORT(CtlUseB1),
                 NULL);

      WinSetWindowText(WinWindowFromID(hwndDlg,CTLNMLGTH  ), CtlNameLen  );
      WinSendMsg(WinWindowFromID(hwndDlg,CTLVERSION),
                 EM_SETTEXTLIMIT,
                 MPFROMSHORT(255),
                 NULL);
      WinSetWindowText(WinWindowFromID(hwndDlg,CTLVERSION ), CtlVersion  );
      WinSendMsg(WinWindowFromID(hwndDlg,CTLDATASEG),
                 EM_SETTEXTLIMIT,
                 MPFROMSHORT(40),
                 NULL);
      WinSetWindowText(WinWindowFromID(hwndDlg,CTLDATASEG ), CtlDataSeg  );
      WinSendMsg(WinWindowFromID(hwndDlg,CTLCODESEG),
                 EM_SETTEXTLIMIT,
                 MPFROMSHORT(40),
                 NULL);
      WinSetWindowText(WinWindowFromID(hwndDlg,CTLCODESEG ), CtlCodeSeg  );

     CtlActive = TRUE ;
      break;
     case WM_CONTROL:
       if  (HIUSHORT(mp1)==BN_CLICKED)
          {
            switch (LOUSHORT((ULONG)mp1))
              {
               case CTLDEBNONE:
                  CtlDebNone= (CtlDebNone ^1) &1;
                  WinSendMsg(hwndDebNone,
                             BM_SETCHECK,
                             MPFROMSHORT(CtlDebNone),
                             NULL);
                  if (CtlDebNone)
                  {
                     if (CtlDebSym==1)
                      {
                        CtlDebSym=0;
                        WinSendMsg(hwndDebSym,
                                   BM_SETCHECK,
                                   MPFROMSHORT(0),
                                   NULL);
                      }
                     if (CtlDebLine==1)
                      {
                        CtlDebLine=0;
                        WinSendMsg(hwndDebLine,
                                   BM_SETCHECK,
                                   MPFROMSHORT(0),
                                   NULL);
                      }
                   }
                  break;
               case CTLDEBLINE:
                  CtlDebLine= (CtlDebLine ^1) &1;
                  WinSendMsg(hwndDebLine,
                             BM_SETCHECK,
                             MPFROMSHORT(CtlDebLine),
                             NULL);
                  if (CtlDebNone==1)
                   {
                     CtlDebNone=0;
                     WinSendMsg(hwndDebNone,
                                BM_SETCHECK,
                                MPFROMSHORT(0),
                                NULL);
                     if (OptNone == FALSE)
                     {
                      OptNone = TRUE;
                      Optall = FALSE;
                      if (OptActive)
                      {
                              WinSendMsg(hwndOptNone,
                                       BM_SETCHECK,
                                       MPFROMSHORT(OptNone),
                                       NULL);
                              WinSendMsg(hwndOptAll,
                                         BM_SETCHECK,
                                         MPFROMSHORT(0),
                                         NULL);
                              Disable_ALL_Checks();
                      } /* endif */
                      else
                        for (i=0;i<9 ;i++ )
                           OptOptions[i]=0;
                     }
                   }
                  break;
               case CTLDEBSYM:
                  CtlDebSym= (CtlDebSym ^1) &1;
                  WinSendMsg(hwndDebSym ,
                             BM_SETCHECK,
                             MPFROMSHORT(CtlDebSym),
                             NULL);
                  if (CtlDebNone==1)
                   {
                     CtlDebNone=0;
                     WinSendMsg(hwndDebNone,
                                BM_SETCHECK,
                                MPFROMSHORT(0),
                                NULL);
                     if (OptNone == FALSE)
                     {
                      OptNone = TRUE;
                      Optall = FALSE;
                      if (OptActive)
                      {
                              WinSendMsg(hwndOptNone,
                                       BM_SETCHECK,
                                       MPFROMSHORT(OptNone),
                                       NULL);
                              WinSendMsg(hwndOptAll,
                                         BM_SETCHECK,
                                         MPFROMSHORT(0),
                                         NULL);
                              Disable_ALL_Checks();
                      } /* endif */
                      else
                        for (i=0;i<9 ;i++ )
                           OptOptions[i]=0;
                     }
                   }
                  break;
               default :
                  break;
              }
          }
      break;
     case WM_COMMAND:

      switch (SHORT1FROMMP(mp1)) {
         case DID_OK:
            CtlObjTypeix=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,CTLTYPEEXE),
                       BM_QUERYCHECKINDEX,
                       NULL,
                       NULL));
             --CtlObjTypeix;

             CtlDefLib=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,CTLDEFLIB),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
             CtlUseB1 =SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,CTLUSEB1 ),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
             WinQueryWindowText(WinWindowFromID(hwndDlg,CTLNMLGTH ),
                     4,
                     CtlNameLen );
             WinQueryWindowText(WinWindowFromID(hwndDlg,CTLVERSION),
                     256,
                     CtlVersion );
             WinQueryWindowText(WinWindowFromID(hwndDlg,CTLDATASEG),
                     41,
                     CtlDataSeg );
             WinQueryWindowText(WinWindowFromID(hwndDlg,CTLCODESEG),
                     41,
                     CtlCodeSeg );
             CtlActive = FALSE;
             WinDismissDlg(hwndDlg,TRUE);   /* Returns TRUE to WinDlgBox in the Window */
              break;
         case DID_CANCEL:
            CtlActive = FALSE;
            WinDismissDlg(hwndDlg,FALSE);   /* Returns TRUE to WinDlgBox in the Window */
            break;
         case OPTION_DEFAULT:
             CtlObjTypeix=0;
             CtlDebNone=1,
             CtlDefLib=0;
             CtlUseB1 =0;
             CtlDebSym=CtlDebLine=0;
             strcpy(CtlNameLen,"31");
             CtlVersion[0]=CtlDataSeg[0]=CtlCodeSeg[0]='\0';
            WinSendMsg(WinWindowFromID(hwndDlg,CTLDEFLIB),
                 BM_SETCHECK,
                 MPFROMSHORT(CtlDefLib),
                 NULL);
            WinSendMsg(WinWindowFromID(hwndDlg,CTLUSEB1 ),
                 BM_SETCHECK,
                 MPFROMSHORT(CtlUseB1 ),
                 NULL);
             WinSendMsg(WinWindowFromID(hwndDlg,CTLTYPEEXE+CtlObjTypeix),
                        BM_SETCHECK,
                        MPFROMSHORT(1),
                        NULL);
             WinSendMsg(WinWindowFromID(hwndDlg,CTLDEBNONE),
                        BM_SETCHECK,
                        MPFROMSHORT(CtlDebNone),
                        NULL);
             WinSendMsg(WinWindowFromID(hwndDlg,CTLDEBLINE),
                        BM_SETCHECK,
                        MPFROMSHORT(CtlDebLine),
                        NULL);
             WinSendMsg(WinWindowFromID(hwndDlg,CTLDEBSYM ),
                        BM_SETCHECK,
                        MPFROMSHORT(CtlDebSym),
                        NULL);
             WinSetWindowText(WinWindowFromID(hwndDlg,CTLNMLGTH  ), CtlNameLen  );
             WinSetWindowText(WinWindowFromID(hwndDlg,CTLVERSION ), CtlVersion  );
             WinSetWindowText(WinWindowFromID(hwndDlg,CTLDATASEG ), CtlDataSeg  );
             WinSetWindowText(WinWindowFromID(hwndDlg,CTLCODESEG ), CtlCodeSeg  );
         case OPTION_RESET:
           break;
         default:
         break;
         } /* endswitch */
      break;
     default:
      return WinDefDlgProc( hwndDlg, msg, mp1, mp2 );
   } /* endswitch */
   return FALSE;
}
MRESULT EXPENTRY PREPPROC( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   IPT x,y;
   int i;
   HWND hwndDependMLE;
   short Adjust;
   switch (msg) {
     case WM_INITDLG:
      WinQueryWindowRect(hwndDlg,&child);
      Adjust=(OptActive+CGenActive+CtlActive+PrepActive
                      +OutfActive+ListActive+SctlActive)*(USHORT)tb.yTop;

      WinSetWindowPos(hwndDlg,HWND_TOP,(short)(parent.xRight-child.xRight-1)
                      ,(short)(parent.yTop-tb.yTop-Adjust-child.yTop)
                      ,0,0,SWP_MOVE | SWP_SHOW);
      WinSendMsg(WinWindowFromID(hwndDlg,PREPLSTFILE),
                 BM_SETCHECK,
                 MPFROMSHORT(boolLstFile),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,PREPLSTSOWL),
                 BM_SETCHECK,
                 MPFROMSHORT(boolListSOWL),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,PREPLSTSONL),
                 BM_SETCHECK,
                 MPFROMSHORT(boolListSONL),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,PREPCOMMENT),
                 BM_SETCHECK,
                 MPFROMSHORT(boolPrepComment),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,PREPEXCLUDE),
                 BM_SETCHECK,
                 MPFROMSHORT(boolPrepExclude),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,PREPUNDALL),
                 BM_SETCHECK,
                 MPFROMSHORT(boolPrepUndAll),
                 NULL);
      hwndDependMLE=WinWindowFromID(hwndDlg,PREPINCLUDE);
     for (i=0;i<PrepInclCount;i++)
      {
        x= (IPT)WinSendMsg(hwndDependMLE,
                   MLM_CHARFROMLINE,
                   MPFROMSHORT(i),
                   0L);
           WinSendMsg(hwndDependMLE,
                   MLM_SETSEL,
                   MPFROMLONG(x),
                   MPFROMLONG(x));
           WinSendMsg(hwndDependMLE,
                   MLM_INSERT,
                   PrepIncludeDir[i],
                   NULL);
      }
      hwndDependMLE=WinWindowFromID(hwndDlg,PREPDEFINE );
     for (i=0;i<PrepDefCount;i++)
      {
        x= (IPT)WinSendMsg(hwndDependMLE,
                   MLM_CHARFROMLINE,
                   MPFROMSHORT(i),
                   0L);
           WinSendMsg(hwndDependMLE,
                   MLM_SETSEL,
                   MPFROMLONG(x),
                   MPFROMLONG(x));
           WinSendMsg(hwndDependMLE,
                   MLM_INSERT,
                   PrepDefines[i],
                   NULL);
      }
      hwndDependMLE=WinWindowFromID(hwndDlg,PREPUNDEFINE );
     for (i=0;i<PrepUnDefCount;i++)
      {
        x= (IPT)WinSendMsg(hwndDependMLE,
                   MLM_CHARFROMLINE,
                   MPFROMSHORT(i),
                   0L);
           WinSendMsg(hwndDependMLE,
                   MLM_SETSEL,
                   MPFROMLONG(x),
                   MPFROMLONG(x));
           WinSendMsg(hwndDependMLE,
                   MLM_INSERT,
                   PrepUndefines[i],
                   NULL);
      }
     PrepActive = TRUE ;
      break;

     case WM_CONTROL:
       if  (HIUSHORT(mp1)==BN_CLICKED)
          {
            switch (LOUSHORT((ULONG)mp1))
              {
               case PREPLSTFILE :
                  if (WinSendMsg(WinWindowFromID(hwndDlg,PREPLSTFILE),
                            BM_QUERYCHECK,
                            NULL,
                            NULL))
                  {
                   WinSendMsg(WinWindowFromID(hwndDlg,PREPLSTSOWL),
                              BM_SETCHECK,
                              MPFROMSHORT(0),
                              NULL);
                   WinSendMsg(WinWindowFromID(hwndDlg,PREPLSTSONL),
                              BM_SETCHECK,
                              MPFROMSHORT(0),
                              NULL);
                  }
                  break;
               case PREPLSTSOWL :
                  if (WinSendMsg(WinWindowFromID(hwndDlg,PREPLSTSOWL),
                            BM_QUERYCHECK,
                            NULL,
                            NULL))
                  {
                   WinSendMsg(WinWindowFromID(hwndDlg,PREPLSTFILE),
                              BM_SETCHECK,
                              MPFROMSHORT(0),
                              NULL);
                   WinSendMsg(WinWindowFromID(hwndDlg,PREPLSTSONL),
                              BM_SETCHECK,
                              MPFROMSHORT(0),
                              NULL);
                  }
                  break;
               case PREPLSTSONL :
                  if (WinSendMsg(WinWindowFromID(hwndDlg,PREPLSTSONL),
                            BM_QUERYCHECK,
                            NULL,
                            NULL))
                  {
                   WinSendMsg(WinWindowFromID(hwndDlg,PREPLSTFILE),
                              BM_SETCHECK,
                              MPFROMSHORT(0),
                              NULL);
                   WinSendMsg(WinWindowFromID(hwndDlg,PREPLSTSOWL),
                              BM_SETCHECK,
                              MPFROMSHORT(0),
                              NULL);
                  }
                  break;
               default :
                  break;
              }
          }
      break;
   case WM_COMMAND:
      switch (SHORT1FROMMP(mp1)) {
         case DID_OK:
            boolLstFile=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,PREPLSTFILE ),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            boolListSOWL=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,PREPLSTSOWL),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            boolListSONL=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,PREPLSTSONL),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            boolPrepComment=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,PREPCOMMENT),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            boolPrepExclude=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,PREPEXCLUDE),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            boolPrepUndAll=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,PREPUNDALL),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            hwndDependMLE=WinWindowFromID(hwndDlg,PREPINCLUDE);
            PrepInclCount= SHORT1FROMMP(WinSendMsg(hwndDependMLE,
                           MLM_QUERYLINECOUNT,
                           NULL,
                           0L));
            if (PrepInclCount > 15)
               PrepInclCount = 15;
            for (i=0;i<PrepInclCount;i++)
            {
                  x= (IPT)WinSendMsg(hwndDependMLE,
                             MLM_CHARFROMLINE,
                             MPFROMSHORT(i),
                             0L);
                  y= (IPT)WinSendMsg(hwndDependMLE,
                             MLM_CHARFROMLINE,
                             MPFROMSHORT(i+1),
                             0L);
                  WinSendMsg(hwndDependMLE,
                          MLM_SETSEL,
                          MPFROMLONG(x),
                          MPFROMLONG(y));
                  WinSendMsg(hwndDependMLE,
                          MLM_QUERYSELTEXT,
                          MPFROMLONG(PrepIncludeDir[i]),
                          NULL);
             }
            hwndDependMLE=WinWindowFromID(hwndDlg,PREPDEFINE );
            PrepDefCount= SHORT1FROMMP(WinSendMsg(hwndDependMLE,
                           MLM_QUERYLINECOUNT,
                           NULL,
                           0L));
            if (PrepDefCount > 15)
               PrepDefCount = 15;
            for (i=0;i<PrepDefCount;i++)
            {
                  x= (IPT)WinSendMsg(hwndDependMLE,
                             MLM_CHARFROMLINE,
                             MPFROMSHORT(i),
                             0L);
                  y= (IPT)WinSendMsg(hwndDependMLE,
                             MLM_CHARFROMLINE,
                             MPFROMSHORT(i+1),
                             0L);
                  WinSendMsg(hwndDependMLE,
                          MLM_SETSEL,
                          MPFROMLONG(x),
                          MPFROMLONG(y));
                  WinSendMsg(hwndDependMLE,
                          MLM_QUERYSELTEXT,
                          MPFROMLONG(PrepDefines[i]),
                          NULL);
             }
             hwndDependMLE=WinWindowFromID(hwndDlg,PREPUNDEFINE );
            PrepUnDefCount= SHORT1FROMMP(WinSendMsg(hwndDependMLE,
                           MLM_QUERYLINECOUNT,
                           NULL,
                           0L));
            if (PrepUnDefCount > 15)
               PrepUnDefCount = 15;
            for (i=0;i<PrepUnDefCount;i++)
            {
                  x= (IPT)WinSendMsg(hwndDependMLE,
                             MLM_CHARFROMLINE,
                             MPFROMSHORT(i),
                             0L);
                  y= (IPT)WinSendMsg(hwndDependMLE,
                             MLM_CHARFROMLINE,
                             MPFROMSHORT(i+1),
                             0L);
                  WinSendMsg(hwndDependMLE,
                          MLM_SETSEL,
                          MPFROMLONG(x),
                          MPFROMLONG(y));
                  WinSendMsg(hwndDependMLE,
                          MLM_QUERYSELTEXT,
                          MPFROMLONG(PrepUndefines[i]),
                          NULL);
             }
             PrepActive = FALSE ;

             WinDismissDlg(hwndDlg,TRUE);   /* Returns TRUE to WinDlgBox in the Window */
              break;
         case DID_CANCEL:
            PrepActive = FALSE ;
            WinDismissDlg(hwndDlg,FALSE);   /* Returns TRUE to WinDlgBox in the Window */
            break;
         case OPTION_DEFAULT:
            boolLstFile = 0;
            boolListSOWL= 0;
            boolListSONL= 0;
            boolPrepComment= 0;
            boolPrepExclude= 0;
            boolPrepUndAll = 0;
            WinSendMsg(WinWindowFromID(hwndDlg,PREPLSTFILE),
                       BM_SETCHECK,
                       MPFROMSHORT(boolLstFile),
                       NULL);
            WinSendMsg(WinWindowFromID(hwndDlg,PREPLSTSOWL),
                       BM_SETCHECK,
                       MPFROMSHORT(boolListSOWL),
                       NULL);
            WinSendMsg(WinWindowFromID(hwndDlg,PREPLSTSONL),
                       BM_SETCHECK,
                       MPFROMSHORT(boolListSONL),
                       NULL);
            WinSendMsg(WinWindowFromID(hwndDlg,PREPCOMMENT),
                       BM_SETCHECK,
                       MPFROMSHORT(boolPrepComment),
                       NULL);
            WinSendMsg(WinWindowFromID(hwndDlg,PREPEXCLUDE),
                       BM_SETCHECK,
                       MPFROMSHORT(boolPrepExclude),
                       NULL);
            WinSendMsg(WinWindowFromID(hwndDlg,PREPUNDALL),
                       BM_SETCHECK,
                       MPFROMSHORT(boolPrepUndAll),
                       NULL);

              hwndDependMLE=WinWindowFromID(hwndDlg,PREPINCLUDE);
              PrepInclCount= SHORT1FROMMP(WinSendMsg(hwndDependMLE,
                           MLM_QUERYLINECOUNT,
                           NULL,
                           0L));
                  x= (IPT)WinSendMsg(hwndDependMLE,
                             MLM_CHARFROMLINE,
                             MPFROMSHORT(0),
                             0L);
                  y= (IPT)WinSendMsg(hwndDependMLE,
                             MLM_CHARFROMLINE,
                             MPFROMSHORT(PrepInclCount),
                             0L);
                  WinSendMsg(hwndDependMLE,
                          MLM_SETSEL,
                          MPFROMLONG(x),
                          MPFROMLONG(y));
                  WinSendMsg(hwndDependMLE,
                          MLM_CLEAR,
                          NULL,
                          NULL);
            hwndDependMLE=WinWindowFromID(hwndDlg,PREPDEFINE );
            PrepDefCount= SHORT1FROMMP(WinSendMsg(hwndDependMLE,
                           MLM_QUERYLINECOUNT,
                           NULL,
                           0L));
                  x= (IPT)WinSendMsg(hwndDependMLE,
                             MLM_CHARFROMLINE,
                             MPFROMSHORT(0),
                             0L);
                  y= (IPT)WinSendMsg(hwndDependMLE,
                             MLM_CHARFROMLINE,
                             MPFROMSHORT(PrepDefCount),
                             0L);
                  WinSendMsg(hwndDependMLE,
                          MLM_SETSEL,
                          MPFROMLONG(x),
                          MPFROMLONG(y));
                  WinSendMsg(hwndDependMLE,
                          MLM_CLEAR,
                          NULL,
                          NULL);
             hwndDependMLE=WinWindowFromID(hwndDlg,PREPUNDEFINE );
            PrepUnDefCount= SHORT1FROMMP(WinSendMsg(hwndDependMLE,
                           MLM_QUERYLINECOUNT,
                           NULL,
                           0L));
                  x= (IPT)WinSendMsg(hwndDependMLE,
                             MLM_CHARFROMLINE,
                             MPFROMSHORT(0),
                             0L);
                  y= (IPT)WinSendMsg(hwndDependMLE,
                             MLM_CHARFROMLINE,
                             MPFROMSHORT(PrepUnDefCount),
                             0L);
                  WinSendMsg(hwndDependMLE,
                          MLM_SETSEL,
                          MPFROMLONG(x),
                          MPFROMLONG(y));
                  WinSendMsg(hwndDependMLE,
                          MLM_CLEAR,
                          NULL,
                          NULL);

            PrepInclCount=0;
            PrepDefCount =0;
            PrepUnDefCount =0;
                  break;
               case OPTION_RESET:
                 break;
               default:
               break;
               } /* endswitch */
      break;
     default:
      return WinDefDlgProc( hwndDlg, msg, mp1, mp2 );
   } /* endswitch */
   return FALSE;
}
MRESULT EXPENTRY OUTFPROC( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   short Adjust;
   switch (msg) {
   case WM_INITDLG:
      WinQueryWindowRect(hwndDlg,&child);
      Adjust=(OptActive+CGenActive+CtlActive+PrepActive
                      +OutfActive+ListActive+SctlActive)*(USHORT)tb.yTop;

      WinSetWindowPos(hwndDlg,HWND_TOP,(short)(parent.xRight-child.xRight-1)
                      ,(short)(parent.yTop-tb.yTop-Adjust-child.yTop)
                      ,0,0,SWP_MOVE | SWP_SHOW);
      WinSendMsg(WinWindowFromID(hwndDlg,OUTSRCOPT),
                 BM_SETCHECK,
                 MPFROMSHORT(OutSrcList),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,OUTMAPOPT),
                 BM_SETCHECK,
                 MPFROMSHORT(OutMap    ),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,OUTOBJOBJ),
                 BM_SETCHECK,
                 MPFROMSHORT(OutObjList),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,OUTOBJASM),
                 BM_SETCHECK,
                 MPFROMSHORT(OutAsmList),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,OUTOBJCOM),
                 BM_SETCHECK,
                 MPFROMSHORT(OutComList),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,OUTSRCNAME  ),
                 EM_SETTEXTLIMIT,
                 MPFROMSHORT(260),
                 NULL);
      WinSetWindowText(WinWindowFromID(hwndDlg,OUTSRCNAME ), OutSrcName  );
      WinSendMsg(WinWindowFromID(hwndDlg,OUTMAPNAME  ),
                 EM_SETTEXTLIMIT,
                 MPFROMSHORT(260),
                 NULL);
      WinSetWindowText(WinWindowFromID(hwndDlg,OUTMAPNAME ), OutMapName  );
      WinSendMsg(WinWindowFromID(hwndDlg,OUTOBJFNAME  ),
                 EM_SETTEXTLIMIT,
                 MPFROMSHORT(260),
                 NULL);
      WinSetWindowText(WinWindowFromID(hwndDlg,OUTOBJFNAME ), OutObjFname );
      WinSendMsg(WinWindowFromID(hwndDlg,OUTOBJNAME  ),
                 EM_SETTEXTLIMIT,
                 MPFROMSHORT(260),
                 NULL);
      WinSetWindowText(WinWindowFromID(hwndDlg,OUTOBJNAME ), OutObjName );
      WinSendMsg(WinWindowFromID(hwndDlg,OUTEXENAME  ),
                 EM_SETTEXTLIMIT,
                 MPFROMSHORT(260),
                 NULL);
      WinSetWindowText(WinWindowFromID(hwndDlg,OUTEXENAME ), OutExeName );
      OutfActive = TRUE ;
      break;
     case WM_CONTROL:
       if  (HIUSHORT(mp1)==BN_CLICKED)
          {
            switch (LOUSHORT((ULONG)mp1))
              {
               case OUTOBJOBJ:
                  if (WinSendMsg(WinWindowFromID(hwndDlg,OUTOBJOBJ  ),
                            BM_QUERYCHECK,
                            NULL,
                            NULL))
                  {
                    WinSendMsg(WinWindowFromID(hwndDlg,OUTOBJASM  ),
                              BM_SETCHECK,
                              MPFROMSHORT(0),
                              NULL);
                    WinSendMsg(WinWindowFromID(hwndDlg,OUTOBJCOM  ),
                              BM_SETCHECK,
                              MPFROMSHORT(0),
                              NULL);
                   }
               break;
               case OUTOBJASM:
                  if (WinSendMsg(WinWindowFromID(hwndDlg,OUTOBJASM  ),
                            BM_QUERYCHECK,
                            NULL,
                            NULL))
                  {
                    WinSendMsg(WinWindowFromID(hwndDlg,OUTOBJOBJ  ),
                              BM_SETCHECK,
                              MPFROMSHORT(0),
                              NULL);
                    WinSendMsg(WinWindowFromID(hwndDlg,OUTOBJCOM  ),
                              BM_SETCHECK,
                              MPFROMSHORT(0),
                              NULL);
                   }
               break;
               case OUTOBJCOM:
                  if (WinSendMsg(WinWindowFromID(hwndDlg,OUTOBJCOM  ),
                            BM_QUERYCHECK,
                            NULL,
                            NULL))
                  {
                    WinSendMsg(WinWindowFromID(hwndDlg,OUTOBJOBJ  ),
                              BM_SETCHECK,
                              MPFROMSHORT(0),
                              NULL);
                    WinSendMsg(WinWindowFromID(hwndDlg,OUTOBJASM  ),
                              BM_SETCHECK,
                              MPFROMSHORT(0),
                              NULL);
                   }
               break;
              }
          }
      break;
   case WM_COMMAND:
      switch (SHORT1FROMMP(mp1)) {
         case DID_OK:
            OutSrcList =SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,OUTSRCOPT   ),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            OutMap     =SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,OUTMAPOPT   ),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            OutObjList =SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,OUTOBJOBJ   ),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            OutAsmList =SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,OUTOBJASM   ),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            OutComList =SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,OUTOBJCOM   ),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            WinQueryWindowText(WinWindowFromID(hwndDlg,OUTSRCNAME      ),
                      260,
                      OutSrcName     );
            WinQueryWindowText(WinWindowFromID(hwndDlg,OUTMAPNAME      ),
                      260,
                      OutMapName     );
            WinQueryWindowText(WinWindowFromID(hwndDlg,OUTOBJFNAME      ),
                      260,
                      OutObjFname     );
            WinQueryWindowText(WinWindowFromID(hwndDlg,OUTOBJNAME      ),
                      260,
                      OutObjName     );
            WinQueryWindowText(WinWindowFromID(hwndDlg,OUTEXENAME      ),
                      260,
                      OutExeName     );
             OutfActive = FALSE;
             WinDismissDlg(hwndDlg,TRUE);   /* Returns TRUE to WinDlgBox in the Window */
              break;
         case DID_CANCEL:
             OutfActive = FALSE;
            WinDismissDlg(hwndDlg,FALSE);   /* Returns TRUE to WinDlgBox in the Window */
            break;
         case OPTION_DEFAULT:
            OutSrcList=0;
            OutMap  =0;
            OutObjList=0;
            OutAsmList=0;
            OutComList=0;
            OutSrcName[0]=OutExeName[0]=OutObjName[0]=OutMapName[0]=OutObjFname[0]= '\0';
            WinSendMsg(WinWindowFromID(hwndDlg,OUTSRCOPT),
                       BM_SETCHECK,
                       MPFROMSHORT(OutSrcList),
                       NULL);
            WinSendMsg(WinWindowFromID(hwndDlg,OUTMAPOPT),
                       BM_SETCHECK,
                       MPFROMSHORT(OutMap    ),
                       NULL);
            WinSendMsg(WinWindowFromID(hwndDlg,OUTOBJOBJ),
                       BM_SETCHECK,
                       MPFROMSHORT(OutObjList),
                       NULL);
            WinSendMsg(WinWindowFromID(hwndDlg,OUTOBJASM),
                       BM_SETCHECK,
                       MPFROMSHORT(OutAsmList),
                       NULL);
            WinSendMsg(WinWindowFromID(hwndDlg,OUTOBJCOM),
                       BM_SETCHECK,
                       MPFROMSHORT(OutComList),
                       NULL);
            WinSetWindowText(WinWindowFromID(hwndDlg,OUTSRCNAME ), OutSrcName  );
            WinSetWindowText(WinWindowFromID(hwndDlg,OUTMAPNAME ), OutMapName  );
            WinSetWindowText(WinWindowFromID(hwndDlg,OUTOBJFNAME ), OutObjFname );
            WinSetWindowText(WinWindowFromID(hwndDlg,OUTOBJNAME ), OutObjName );
            WinSetWindowText(WinWindowFromID(hwndDlg,OUTEXENAME ), OutExeName );
         case OPTION_RESET:
           break;
         default:
         break;
         } /* endswitch */
      break;
     default:
      return WinDefDlgProc( hwndDlg, msg, mp1, mp2 );
   } /* endswitch */
   return FALSE;
}

MRESULT EXPENTRY LISTPROC( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   short Adjust;
   switch (msg) {
   case WM_INITDLG:
      WinQueryWindowRect(hwndDlg,&child);
      Adjust=(OptActive+CGenActive+CtlActive+PrepActive
                      +OutfActive+ListActive+SctlActive)*(USHORT)tb.yTop;

      WinSetWindowPos(hwndDlg,HWND_TOP,(short)(parent.xRight-child.xRight-1)
                      ,(short)(parent.yTop-tb.yTop-Adjust-child.yTop)
                      ,0,0,SWP_MOVE | SWP_SHOW);
      WinSendMsg(WinWindowFromID(hwndDlg,LSTWARN0+LstWarnix),
                 BM_SETCHECK,
                 MPFROMSHORT(1),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,LSTLINE ),
                 EM_SETTEXTLIMIT,
                 MPFROMSHORT(3),
                 NULL);
      WinSetWindowText(WinWindowFromID(hwndDlg,LSTLINE), LstLine);
      WinSendMsg(WinWindowFromID(hwndDlg,LSTPAGE ),
                 EM_SETTEXTLIMIT,
                 MPFROMSHORT(3),
                 NULL);
      WinSetWindowText(WinWindowFromID(hwndDlg,LSTPAGE), LstPage);
      WinSendMsg(WinWindowFromID(hwndDlg,LSTTITLE),
                 EM_SETTEXTLIMIT,
                 MPFROMSHORT(99),
                 NULL);
      WinSetWindowText(WinWindowFromID(hwndDlg,LSTTITLE), LstTitle);
      WinSendMsg(WinWindowFromID(hwndDlg,LSTSUBTITLE),
                 EM_SETTEXTLIMIT,
                 MPFROMSHORT(99),
                 NULL);
      WinSetWindowText(WinWindowFromID(hwndDlg,LSTSUBTITLE), LstSubTitle);
      ListActive = TRUE;

      break;
     case WM_COMMAND:

      switch (SHORT1FROMMP(mp1)) {
         case DID_OK:
            LstWarnix=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,LSTWARN0),
                       BM_QUERYCHECKINDEX,
                       NULL,
                       NULL));
             --LstWarnix;
            WinQueryWindowText(WinWindowFromID(hwndDlg,LSTLINE ),
                     4,
                     LstLine );
            WinQueryWindowText(WinWindowFromID(hwndDlg,LSTPAGE ),
                     4,
                     LstPage );
            WinQueryWindowText(WinWindowFromID(hwndDlg,LSTTITLE),
                     100,
                     LstTitle);
            WinQueryWindowText(WinWindowFromID(hwndDlg,LSTSUBTITLE),
                     100,
                     LstSubTitle);
            ListActive = FALSE;
            WinDismissDlg(hwndDlg,TRUE);   /* Returns TRUE to WinDlgBox in the Window */
           break;
      case DID_CANCEL:
            ListActive = FALSE;
            WinDismissDlg(hwndDlg,FALSE);   /* Returns TRUE to WinDlgBox in the Window */
         break;
      case OPTION_DEFAULT:
            LstWarnix = 2;
            LstLine[0]= LstPage[0]= LstTitle[0] = LstSubTitle[0] = '\0';
            WinSendMsg(WinWindowFromID(hwndDlg,LSTWARN0+LstWarnix),
                       BM_SETCHECK,
                       MPFROMSHORT(1),
                       NULL);
            WinSetWindowText(WinWindowFromID(hwndDlg,LSTLINE), LstLine);
            WinSetWindowText(WinWindowFromID(hwndDlg,LSTPAGE), LstPage);
            WinSetWindowText(WinWindowFromID(hwndDlg,LSTTITLE), LstTitle);
            WinSetWindowText(WinWindowFromID(hwndDlg,LSTSUBTITLE), LstSubTitle);
             break;
      case OPTION_RESET:
         break;
      default:
         break;
      } /* endswitch */
      break;
   default:
      return WinDefDlgProc( hwndDlg, msg, mp1, mp2 );
   } /* endswitch */
   return FALSE;
}

MRESULT EXPENTRY SCTLPROC( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   short Adjust;
   switch (msg) {
   case WM_INITDLG:
      WinQueryWindowRect(hwndDlg,&child);
      Adjust=(OptActive+CGenActive+CtlActive+PrepActive
                      +OutfActive+ListActive+SctlActive)*(USHORT)tb.yTop;

      WinSetWindowPos(hwndDlg,HWND_TOP,(short)(parent.xRight-child.xRight-1)
                      ,(short)(parent.yTop-tb.yTop-Adjust-child.yTop)
                      ,0,0,SWP_MOVE | SWP_SHOW);
      WinSendMsg(WinWindowFromID(hwndDlg,SCTLPACK1+SctlPackix),
                 BM_SETCHECK,
                 MPFROMSHORT(1),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,SCTLDEXT  ),
                 BM_SETCHECK,
                 MPFROMSHORT(SctlDext ),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,SCTLSYNTAX),
                 BM_SETCHECK,
                 MPFROMSHORT(SctlSyntax),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,SCTLDECL  ),
                 BM_SETCHECK,
                 MPFROMSHORT(SctlDecl  ),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,SCTLCONLY ),
                 BM_SETCHECK,
                 MPFROMSHORT(SctlConly ),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,SCTLJ     ),
                 BM_SETCHECK,
                 MPFROMSHORT(SctlJ     ),
                 NULL);
      WinSendMsg(WinWindowFromID(hwndDlg,SCTLCASEI ),
                 BM_SETCHECK,
                 MPFROMSHORT(SctlCaseI  ),
                 NULL);
      SctlActive = TRUE;
      break;
     case WM_COMMAND:

      switch (SHORT1FROMMP(mp1)) {
         case DID_OK:
            SctlPackix=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,SCTLPACK1),
                       BM_QUERYCHECKINDEX,
                       NULL,
                       NULL));
             --SctlPackix;
            SctlDext =SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,SCTLDEXT  ),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            SctlSyntax=SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,SCTLSYNTAX),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            SctlDecl  =SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,SCTLDECL  ),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            SctlConly =SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,SCTLCONLY ),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            SctlJ     =SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,SCTLJ     ),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            SctlCaseI =SHORT1FROMMP(WinSendMsg(WinWindowFromID(hwndDlg,SCTLCASEI ),
                       BM_QUERYCHECK,
                       NULL,
                       NULL));
            SctlActive = FALSE;
            WinDismissDlg(hwndDlg,TRUE );   /* Returns TRUE to WinDlgBox in the Window */
            break;
         case DID_CANCEL:
            SctlActive = FALSE;
            WinDismissDlg(hwndDlg,FALSE);   /* Returns TRUE to WinDlgBox in the Window */
            break;
         case OPTION_DEFAULT:
            SctlDext=0;
            SctlSyntax=0;
            SctlDecl=0;
            SctlConly=0;
            SctlJ=0;
            SctlCaseI=0;
            SctlPackix=2;
            WinSendMsg(WinWindowFromID(hwndDlg,SCTLPACK1+SctlPackix),
                       BM_SETCHECK,
                       MPFROMSHORT(1),
                       NULL);
            WinSendMsg(WinWindowFromID(hwndDlg,SCTLDEXT  ),
                       BM_SETCHECK,
                       MPFROMSHORT(SctlDext ),
                       NULL);
            WinSendMsg(WinWindowFromID(hwndDlg,SCTLSYNTAX),
                       BM_SETCHECK,
                       MPFROMSHORT(SctlSyntax),
                       NULL);
            WinSendMsg(WinWindowFromID(hwndDlg,SCTLDECL  ),
                       BM_SETCHECK,
                       MPFROMSHORT(SctlDecl  ),
                       NULL);
            WinSendMsg(WinWindowFromID(hwndDlg,SCTLCONLY ),
                       BM_SETCHECK,
                       MPFROMSHORT(SctlConly ),
                       NULL);
            WinSendMsg(WinWindowFromID(hwndDlg,SCTLJ     ),
                       BM_SETCHECK,
                       MPFROMSHORT(SctlJ     ),
                       NULL);
            WinSendMsg(WinWindowFromID(hwndDlg,SCTLCASEI ),
                       BM_SETCHECK,
                       MPFROMSHORT(SctlCaseI  ),
                       NULL);

            break;
         case OPTION_RESET:
           break;
         default:
         break;
         } /* endswitch */
      break;
     default:
      return WinDefDlgProc( hwndDlg, msg, mp1, mp2 );
   } /* endswitch */
   return FALSE;
}

#define CheckOutlen(increment)                                                 \
{                                                                              \
   if ( (ULONG)(outstring+increment-outstringstart) > outlen )                 \
   {                                                                           \
      char msg[100];                                                           \
                                                                               \
      WinLoadString ( HWND_DESKTOP, handleDLL, MSG_CMPSTRINGSMALL, 100, msg);  \
                                                                               \
      WinMessageBox(  HWND_DESKTOP,                                            \
                      handleWise,                                              \
                      msg,                                                     \
                      NULL,                                                    \
                      MSG_CMPSTRINGSMALL,                                      \
                      MB_MOVEABLE | MB_OK | MB_ERROR |  MB_APPLMODAL);         \
                                                                               \
       returncode = ERRORCONDITION;                                            \
       return;                                                                 \
   }                                                                           \
}

#define Checkcomplen(increment)                                                \
{                                                                              \
   if ( (ULONG)((ULONG)stringtable+increment-(ULONG)ivs) > *complen )          \
   {                                                                           \
      char msg[100];                                                           \
                                                                               \
      WinLoadString ( HWND_DESKTOP, handleDLL, MSG_CMPOPTSMALL, 100, msg);     \
                                                                               \
      WinMessageBox(  HWND_DESKTOP,                                            \
                      handleWise,                                              \
                      msg,                                                     \
                      NULL,                                                    \
                      MSG_CMPOPTSMALL,                                         \
                      MB_MOVEABLE | MB_OK | MB_ERROR |  MB_APPLMODAL);         \
                                                                               \
       returncode = ERRORCONDITION;                                            \
       return;                                                                 \
   }                                                                           \
}

static void ReturnOutputString(void)
{
     USHORT offset;
     int i, invcount;
     char * work;
     char * work2;
     char * stringtable;
     char * stringstart;
     char * outstringstart;
     ivs->stringtablestart[0]='\0';
     outstringstart = outstring;
     stringtable = ivs->stringtablestart+1;
     stringstart = ivs->stringtablestart;
     ivs->optmax    = Optall;
     ivs->optnone   = OptNone;
     ivs->optspeed  = OptOptions[0] ;
     ivs->optloop   = OptOptions[1] ;
     ivs->optprecise= OptOptions[2] ;
     ivs->optaliasfn= OptOptions[3] ;
     ivs->optspace  = OptOptions[4] ;
     ivs->optialias = OptOptions[5] ;
     ivs->optintrins= OptOptions[6] ;
     ivs->optunsafe = OptOptions[7] ;
     ivs->optinliner= OptOptions[8] ;
     if (ivs->optmax    )
       {
        CheckOutlen(4);
        strcpy(outstring,"/Ox ");
        outstring +=4;
       }
     else if (ivs->optnone   )
       {
        CheckOutlen(4);
        strcpy(outstring,"/Od ");
        outstring +=4;
       }
     else
      {
       CheckOutlen(2);
       strcpy(outstring,"/O");
       outstring +=2;
       if (ivs->optspeed  )
        {
          CheckOutlen(1);
          *outstring='t';
          ++outstring;
        }
       if (ivs->optloop   )
        {
          CheckOutlen(1);
          *outstring='l';
          ++outstring;
        }
       if (ivs->optprecise)
        {
          CheckOutlen(1);
          *outstring='p';
          ++outstring;
        }
       if (ivs->optaliasfn)
        {
          CheckOutlen(1);
          *outstring='w';
          ++outstring;
        }
       if (ivs->optspace  )
        {
          CheckOutlen(1);
          *outstring='s';
          ++outstring;
        }
       if (ivs->optialias )
        {
          CheckOutlen(1);
          *outstring='a';
          ++outstring;
        }
       if (ivs->optintrins)
        {
          CheckOutlen(1);
          *outstring='i';
          ++outstring;
        }
       if (ivs->optunsafe )
        {
          CheckOutlen(1);
          *outstring='n';
          ++outstring;
        }
       if (ivs->optinliner)
        {
          CheckOutlen(1);
          *outstring='r';
          ++outstring;
        }
       CheckOutlen(1);
       *outstring =' ';
       ++outstring;
      }
     ivs->PascalLink = GenPascal    ;
     ivs->PMLink     = GenPm        ;
     ivs->NoStackPrb = GenNstack    ;
     ivs->StringinCs = GenConst     ;
     ivs->DataThresh = Genthresh    ;
     ivs->MemoryModel= GenModelix   ;
     ivs->FloatOption= GenFloatix   ;
     ivs->CustModelCode=GenCustCodeix;
     ivs->CustModelData=GenCustDataix;
     ivs->CustModelSetS=GenCustSegmix;
     if (Genthresh==0 || Genthreshamount[0]=='\0')
         ivs->DataThreshAmt=0;
     else
         ivs->DataThreshAmt=(USHORT)atoi(Genthreshamount);

     if (GenPascal  )
      {
        CheckOutlen(4);
        strcpy(outstring,"/Gc ");
        outstring+=4;
      }
     if (GenPm      )
      {
        CheckOutlen(4);
        strcpy(outstring,"/Gw ");
        outstring+=4;
      }
     if (GenNstack  )
      {
        CheckOutlen(5);
        strcpy(outstring,"/G2s ");
        outstring+=5;
      }
     else
      {
        CheckOutlen(4);
        strcpy(outstring,"/G2 ");
        outstring+=4;
      }
     if (GenConst   )
      {
        CheckOutlen(4);
        strcpy(outstring,"/Gm ");
        outstring+=4;
      }
     if (Genthresh  )
      {
        CheckOutlen(3);
        strcpy(outstring,"/Gt");
        outstring+=3;
        if (ivs->DataThreshAmt!=0)
         {
          CheckOutlen(strlen(Genthreshamount));
          strcat(outstring,Genthreshamount);
          outstring += strlen(Genthreshamount);
         }
        CheckOutlen(1);
        *outstring = ' ';
        outstring++;
      }

      switch (ivs->MemoryModel)
      {
          case 0:
             break;
          case 1:
            CheckOutlen(4);
            strcpy(outstring,"/AM ");
            outstring+=4;
             break;
          case 2:
            CheckOutlen(4);
            strcpy(outstring,"/AC ");
            outstring+=4;
             break;
          case 3:
            CheckOutlen(4);
            strcpy(outstring,"/AL ");
            outstring+=4;
             break;
          case 4:
            CheckOutlen(4);
            strcpy(outstring,"/AH ");
            outstring+=4;
             break;
          case 5:
            CheckOutlen(2);
             strcpy(outstring,"/A");
             outstring+=2;
             if (ivs->CustModelCode==0)
               {
                 CheckOutlen(1);
                 *outstring='s';
                 ++outstring;
               }
             else
               {
                 CheckOutlen(1);
                 *outstring='l';
                 ++outstring;
               }
             if (ivs->CustModelData==0)
               {
                 CheckOutlen(1);
                 *outstring='n';
                 ++outstring;
               }
             else if (ivs->CustModelData==1)
                  {
                    CheckOutlen(1);
                    *outstring='f';
                    ++outstring;
                  }
                  else
                  {
                    CheckOutlen(1);
                    *outstring='h';
                    ++outstring;
                  }
             if (ivs->CustModelSetS==0)
               {
                 CheckOutlen(1);
                 *outstring='d';
                 ++outstring;
               }
             else if (ivs->CustModelSetS==1)
                  {
                    CheckOutlen(1);
                    *outstring='u';
                    ++outstring;
                  }
                  else
                  {
                    CheckOutlen(1);
                    *outstring='w';
                    ++outstring;
                  }
             CheckOutlen(1);
             *outstring =' ';
             outstring++;
             break;
      }
      switch (ivs->FloatOption )
      {
         case 0:
           CheckOutlen(5);
           strcpy(outstring,"/FPa ");
           outstring+=5;
           break;
         case 1:
           CheckOutlen(5);
           strcpy(outstring,"/FPc ");
           outstring+=5;
           break;
         case 2:
           CheckOutlen(7);
           strcpy(outstring,"/FPc87 ");
           outstring+=7;
           break;
         case 3:
           break;
         case 4:
           CheckOutlen(7);
           strcpy(outstring,"/FPi87 ");
           outstring+=7;
           break;
      }




     ivs->ObjectisExe =CtlObjTypeix ;
     ivs->RemoveDefLib=CtlDefLib    ;
     ivs->UseB1       =CtlUseB1     ;
     ivs->DebLineNum  =CtlDebLine   ;
     ivs->DebSymDeb   =CtlDebSym    ;
     if (ivs->ObjectisExe==1)
      {
        CheckOutlen(6);
        strcpy(outstring,"/DDLL ");
        outstring+=6;
      }
     if (ivs->RemoveDefLib)
      {
        CheckOutlen(4);
        strcpy(outstring,"/Zl ");
        outstring+=4;
      }
     if (ivs->UseB1       )
      {
        CheckOutlen(12);
        strcpy(outstring,"/B1 C1L.EXE ");
        outstring+=12;
      }
     if (ivs->DebLineNum  )
      {
        CheckOutlen(4);
        strcpy(outstring,"/Zd ");
        outstring+=4;
      }
     if (ivs->DebSymDeb   )
      {
        CheckOutlen(4);
        strcpy(outstring,"/Zi ");
        outstring+=4;
      }

      ivs->ExternNameLgth=atoi(CtlNameLen);
      if (ivs->ExternNameLgth !=0 && ivs->ExternNameLgth != 31)
       {
        CheckOutlen(2);
        strcpy(outstring,"/H");
        outstring+=2;
        CheckOutlen(strlen(CtlNameLen));
        strcpy(outstring,CtlNameLen);
        outstring+=strlen(CtlNameLen);
       }


     if (CtlVersion[0]=='\0')
        ivs->VersionStringoff= 0;
     else
       {
        CheckOutlen(2);
        strcpy(outstring,"/V");
        outstring +=2;
        CheckOutlen(strlen(CtlVersion));
        strcpy(outstring,CtlVersion);
        outstring +=strlen(CtlVersion);
        CheckOutlen(1);
        *outstring = ' ';
        ++outstring;
        offset = (USHORT)(stringtable-stringstart);
        Checkcomplen(strlen(CtlVersion)+1);
        work = CtlVersion;
        while (*work != '\0')
         {
           *stringtable=*work;
           ++stringtable;
           ++work;
         }
           *stringtable='\0';
           ++stringtable;
        ivs->VersionStringoff= offset;

       }
     if (CtlDataSeg[0]=='\0')
        ivs->DsegNameoff= 0;
     else
       {
        CheckOutlen(3);
        strcpy(outstring,"/ND");
        outstring +=3;
        CheckOutlen(strlen(CtlDataSeg));
        strcpy(outstring,CtlDataSeg);
        outstring +=strlen(CtlDataSeg);
        CheckOutlen(1);
        *outstring = ' ';
        ++outstring;
        offset = (USHORT)(stringtable-stringstart);
        Checkcomplen(strlen(CtlDataSeg)+1);
        work = CtlDataSeg;
        while (*work != '\0')
         {
           *stringtable=*work;
           ++stringtable;
           ++work;
         }
           *stringtable='\0';
           ++stringtable;
        ivs->DsegNameoff= offset;

       }
     if (CtlCodeSeg[0]=='\0')
        ivs->CsegNameoff= 0;
     else
       {
        CheckOutlen(3);
        strcpy(outstring,"/NT");
        outstring +=3;
        CheckOutlen(strlen(CtlCodeSeg));
        strcpy(outstring,CtlCodeSeg);
        outstring +=strlen(CtlCodeSeg);
        CheckOutlen(1);
        *outstring = ' ';
        ++outstring;
        offset = (USHORT)(stringtable-stringstart);
        Checkcomplen(strlen(CtlCodeSeg)+1);
        work = CtlCodeSeg;
        while (*work != '\0')
         {
           *stringtable=*work;
           ++stringtable;
           ++work;
         }
           *stringtable='\0';
           ++stringtable;
        ivs->CsegNameoff= offset;

       }

     if (boolLstFile==1)
        {
        ivs->Prepoutput=1;
        CheckOutlen(3);
        strcpy(outstring,"/P ");
        outstring+=3;
        }
     else
       if (boolListSOWL==1)
        {
        ivs->Prepoutput=2;
        CheckOutlen(3);
        strcpy(outstring,"/E ");
        outstring+=3;
        }
     else
       if (boolListSONL==1)
        {
        ivs->Prepoutput=3;
        CheckOutlen(4);
        strcpy(outstring,"/EP ");
        outstring+=4;
        }
        else
          ivs->Prepoutput=0;



     ivs->SaveComments= boolPrepComment;
     ivs->ExcludeStd  = boolPrepExclude;
     ivs->UndefineAll = boolPrepUndAll ;

     if (ivs->SaveComments)
      {
        CheckOutlen(3);
        strcpy(outstring,"/C ");
        outstring+=3;
      }
     if (ivs->ExcludeStd  )
      {
        CheckOutlen(3);
        strcpy(outstring,"/X ");
        outstring+=3;
      }
     if (ivs->UndefineAll )
      {
        CheckOutlen(3);
        strcpy(outstring,"/x ");
        outstring+=3;
      }


     ivs->NumIncludePaths=PrepInclCount ;
     if (ivs->NumIncludePaths==0)
       ivs->IncludePathoff = 0;
     else
      {
       offset=(USHORT)(stringtable-stringstart);
       i=0;
       invcount=0;
       while (i<PrepInclCount)
       {
         work2=PrepIncludeDir[i];
         if (*work2 == '\0' || *work2 == 0x0D || *work2 == 0x0D)
         {
            ++invcount;
         } /* endif */
         else
         {
           CheckOutlen(2);
           strcpy(outstring,"/I");
           outstring+=2;
           while (*work2 != '\0' && *work2 != 0x0D && *work2 != 0x0D)
            {
               CheckOutlen(1);
               Checkcomplen(1);
               *outstring=*work2;
               *stringtable=*work2;
               ++outstring;
               ++stringtable;
               ++work2;
            }
            Checkcomplen(1);
            *stringtable='\0';
            ++stringtable;
            CheckOutlen(1);
            *outstring  =' ';
            ++outstring  ;
         }
            ++i;
       ivs->IncludePathoff=offset;
       }
       ivs->NumIncludePaths -=invcount;
      }


     ivs->NumDefinedMacros = PrepDefCount  ;

     if (ivs->NumDefinedMacros ==0)
       ivs->DefinedMacrooff= 0;
     else
      {
       offset =(USHORT)(stringtable-stringstart);
       i=0;
       invcount=0;
       while ((USHORT)i<ivs->NumDefinedMacros)
       {
         work2=PrepDefines[i];
         if (*work2 == '\0' || *work2 == 0x0D || *work2 == 0x0D)
         {
            ++invcount;
         } /* endif */
         else
         {
           CheckOutlen(2);
           strcpy(outstring,"/D");
           outstring+=2;
           while (*work2 != '\0' && *work2 != 0x0D && *work2 != 0x0D)
            {
               CheckOutlen(1);
               Checkcomplen(1);
               *outstring=*work2;
               *stringtable=*work2;
               ++outstring;
               ++stringtable;
               ++work2;
            }
            Checkcomplen(1);
            *stringtable='\0';
            ++stringtable;
            CheckOutlen(1);
            *outstring  =' ';
            ++outstring  ;
          }
            ++i;
       ivs->DefinedMacrooff =offset;
       }
       ivs->NumDefinedMacros -=invcount;
      }


     ivs->NumUnDefinedMacros = PrepUnDefCount ;
     if (ivs->NumUnDefinedMacros ==0)
       ivs->UnDefinedMacrooff= 0;
     else
      {
       offset =(USHORT)(stringtable-stringstart);
       i=0;
       invcount=0;
       while ((USHORT)i<ivs->NumUnDefinedMacros)
       {
         work2=PrepUndefines[i];
         if (*work2 == '\0' || *work2 == 0x0D || *work2 == 0x0D)
         {
            ++invcount;
         } /* endif */
         else
         {
           CheckOutlen(2);
           strcpy(outstring,"/U");
           outstring+=2;
           while (*work2 != '\0' && *work2 != 0x0D && *work2 != 0x0D)
            {
               CheckOutlen(1);
               Checkcomplen(1);
               *outstring=*work2;
               *stringtable=*work2;
               ++outstring;
               ++stringtable;
               ++work2;
            }
            Checkcomplen(1);
            *stringtable='\0';
            ++stringtable;
            CheckOutlen(1);
            *outstring  =' ';
            ++outstring  ;
          }
            ++i;
       ivs->UnDefinedMacrooff =offset;
       }
       ivs->NumUnDefinedMacros -=invcount;
      }



     ivs->SrcListRequired=OutSrcList ;
     ivs->MapFileRequired=OutMap     ;
     if (ivs->SrcListRequired)
      {
        CheckOutlen(3);
        strcpy(outstring,"/Fs");
        outstring+=3;
        if (OutSrcName[0]=='\0')
        {
         ivs->SrcListFileoff = 0;
        }
        else
        {
         offset = (USHORT)(stringtable-stringstart);
         work2=OutSrcName;
         while (*work2 != '\0')
          {
             CheckOutlen(1);
             Checkcomplen(1);
             *outstring=*work2;
             *stringtable=*work2;
             ++outstring;
             ++stringtable;
             ++work2;
          }
          Checkcomplen(1);
          *stringtable='\0';
          ++stringtable;
         ivs->SrcListFileoff = offset;
        }
        CheckOutlen(1);
        *outstring=' ';
        ++outstring;
      }
     else
      {
      ivs->SrcListFileoff = 0;
      }

     if (ivs->MapFileRequired)
      {
        CheckOutlen(3);
        strcpy(outstring,"/Fm");
        outstring+=3;
        if (OutMapName[0]=='\0')
        {
         ivs->MapListFileoff = 0;
        }
        else
        {
         offset = (USHORT)(stringtable-stringstart);
         work2=OutMapName;
         while (*work2 != '\0')
          {
             CheckOutlen(1);
             Checkcomplen(1);
             *outstring=*work2;
             *stringtable=*work2;
             ++outstring;
             ++stringtable;
             ++work2;
          }
          Checkcomplen(1);
          *stringtable='\0';
          ++stringtable;
          ivs->MapListFileoff = offset;
        }
        CheckOutlen(1);
        *outstring=' ';
        ++outstring;
      }
     else
      {
      ivs->MapListFileoff = 0;
      }




     if (OutObjList==1)
       {

        ivs->ObjectListReq = 1;
        CheckOutlen(3);
        strcpy(outstring,"/Fl");
        outstring+=3;
        if (OutObjFname[0]=='\0')
        {
         ivs->ObjListFileoff = 0;
        }
        else
        {
         offset = (USHORT)(stringtable-stringstart);
         work2=OutObjFname;
         while (*work2 != '\0')
          {
             CheckOutlen(1);
             Checkcomplen(1);
             *outstring=*work2;
             *stringtable=*work2;
             ++outstring;
             ++stringtable;
             ++work2;
          }
          Checkcomplen(1);
          *stringtable='\0';
          ++stringtable;
          ivs->ObjListFileoff = offset;
        }
        CheckOutlen(1);
        *outstring=' ';
        ++outstring;
       }
     else
     if (OutAsmList==1)
       {

        ivs->ObjectListReq = 2;
        CheckOutlen(3);
        strcpy(outstring,"/Fa");
        outstring+=3;
        if (OutObjFname[0]=='\0')
        {
         ivs->ObjListFileoff = 0;
        }
        else
        {
         offset = (USHORT)(stringtable-stringstart);
         work2=OutObjFname;
         while (*work2 != '\0')
          {
             CheckOutlen(1);
             Checkcomplen(1);
             *outstring=*work2;
             *stringtable=*work2;
             ++outstring;
             ++stringtable;
             ++work2;
          }
          Checkcomplen(1);
          *stringtable='\0';
          ++stringtable;
          ivs->ObjListFileoff = offset;
        }
        CheckOutlen(1);
        *outstring=' ';
        ++outstring;
       }
     else
     if (OutComList==1)
       {

        ivs->ObjectListReq = 3;
        CheckOutlen(3);
        strcpy(outstring,"/Fc");
        outstring+=3;
        if (OutObjFname[0]=='\0')
        {
         ivs->ObjListFileoff = 0;
        }
        else
        {
         offset = (USHORT)(stringtable-stringstart);
         work2=OutObjFname;
         while (*work2 != '\0')
          {
             CheckOutlen(1);
             Checkcomplen(1);
             *outstring=*work2;
             *stringtable=*work2;
             ++outstring;
             ++stringtable;
             ++work2;
          }
          Checkcomplen(1);
          *stringtable='\0';
          ++stringtable;
          ivs->ObjListFileoff = offset;
        }
        CheckOutlen(1);
        *outstring=' ';
        ++outstring;
       }
     else
       {
         ivs->ObjectListReq = 0;
         ivs->ObjListFileoff = 0;
       }


     if (*OutObjName=='\0')
        ivs->ObjFileNameoff=0;
     else
      {
         CheckOutlen(3);
         strcpy(outstring,"/Fo");
         outstring+=3;
         offset = (USHORT)(stringtable-stringstart);
         work2=OutObjName;
         while (*work2 != '\0')
          {
             CheckOutlen(1);
             Checkcomplen(1);
             *outstring=*work2;
             *stringtable=*work2;
             ++outstring;
             ++stringtable;
             ++work2;
          }
          Checkcomplen(1);
          *stringtable='\0';
          ++stringtable;
          CheckOutlen(1);
          *outstring=' ';
          ++outstring;
          ivs->ObjFileNameoff = offset;
      }


     if (*OutExeName=='\0')
        ivs->ExeFileNameoff=0;
     else
      {
         CheckOutlen(3);
         strcpy(outstring,"/Fe");
         outstring+=3;
         offset = (USHORT)(stringtable-stringstart);
         work2=OutExeName;
         while (*work2 != '\0')
          {
             CheckOutlen(1);
             Checkcomplen(1);
             *outstring=*work2;
             *stringtable=*work2;
             ++outstring;
             ++stringtable;
             ++work2;
          }
          Checkcomplen(1);
          *stringtable='\0';
          ++stringtable;
          CheckOutlen(1);
          *outstring=' ';
          ++outstring;
          ivs->ExeFileNameoff = offset;
      }


     ivs->WarningLevel = LstWarnix  ;
     if (LstWarnix != 2)
       {
         CheckOutlen(4);
         sprintf(outstring,"/W%d ",LstWarnix);
         outstring+=4;
       }

     if (LstLine[0]== '\0')
        ivs->LineWidth=0 ;
     else
      {
        ivs->LineWidth=atoi(LstLine);
        CheckOutlen(3);
        strcpy(outstring,"/Sl");
        outstring+=3;
        CheckOutlen(strlen(LstLine));
        strcpy(outstring,LstLine);
        outstring+=strlen(LstLine);
        CheckOutlen(1);
        *outstring=' ';
        ++outstring;
      }
     if (LstPage[0]== '\0')
        ivs->PageLength=0 ;
     else
      {
        ivs->PageLength=atoi(LstPage);
        CheckOutlen(3);
        strcpy(outstring,"/Sp");
        outstring+=3;
        CheckOutlen(strlen(LstPage));
        strcpy(outstring,LstPage);
        outstring+=strlen(LstPage);
        CheckOutlen(1);
        *outstring=' ';
        ++outstring;

      }
     if (*LstTitle  =='\0')
        ivs->Titlestringoff=0;
     else
      {
         CheckOutlen(3);
         strcpy(outstring,"/St");
         outstring+=3;
         offset = (USHORT)(stringtable-stringstart);
         work2=LstTitle;
         while (*work2 != '\0')
          {
             CheckOutlen(1);
             Checkcomplen(1);
             *outstring=*work2;
             *stringtable=*work2;
             ++outstring;
             ++stringtable;
             ++work2;
          }
          Checkcomplen(1);
          *stringtable='\0';
          ++stringtable;
          CheckOutlen(1);
          *outstring=' ';
          ++outstring;
          ivs->Titlestringoff = offset;
      }
     if (*LstSubTitle  =='\0')
        ivs->SubTitlestringoff=0;
     else
      {
         CheckOutlen(3);
         strcpy(outstring,"/Ss");
         outstring+=3;
         offset = (USHORT)(stringtable-stringstart);
         work2=LstSubTitle;
         while (*work2 != '\0')
          {
             CheckOutlen(1);
             Checkcomplen(1);
             *outstring=*work2;
             *stringtable=*work2;
             ++outstring;
             ++stringtable;
             ++work2;
          }
          Checkcomplen(1);
          *stringtable='\0';
          ++stringtable;
          CheckOutlen(1);
          *outstring=' ';
          ++outstring;
          ivs->SubTitlestringoff = offset;

      }
      ivs->CompileOnly    =SctlConly  ;
      ivs->SyntaxCheck    =SctlSyntax ;
      ivs->GenerateDecl   =SctlDecl   ;
      ivs->DisableExten   =SctlDext   ;
      ivs->CharisUnsigned =SctlJ      ;
      ivs->CaseInsensitive=SctlCaseI  ;
      ivs->StructPack     =SctlPackix ;
      if (ivs->CompileOnly    )
       {
         CheckOutlen(3);
         strcpy(outstring,"/c ");
         outstring+=3;
       }

      if (ivs->SyntaxCheck    )
       {
         CheckOutlen(4);
         strcpy(outstring,"/Zs ");
         outstring+=4;
       }
      if (ivs->GenerateDecl   )
       {
         CheckOutlen(4);
         strcpy(outstring,"/Zg ");
         outstring+=4;
       }
      if (ivs->DisableExten   )
       {
         CheckOutlen(4);
         strcpy(outstring,"/Za ");
         outstring+=4;
       }
      if (ivs->CharisUnsigned )
       {
         CheckOutlen(3);
         strcpy(outstring,"/J ");
         outstring+=3;
       }
      if (ivs->CaseInsensitive)
       {
         CheckOutlen(4);
         strcpy(outstring,"/Zc ");
         outstring+=4;
       }
      if (ivs->StructPack==0  )
       {
         CheckOutlen(5);
         strcpy(outstring,"/Zp1 ");
         outstring+=5;
       }
      else
      if (ivs->StructPack==1  )
       {
         CheckOutlen(5);
         strcpy(outstring,"/Zp2 ");
         outstring+=5;
       }
      CheckOutlen(1);
      *outstring='\0'  ;
      if (saveFilename != NULL)
      {
        CheckOutlen(strlen(saveFilename)+1);
        strcat(outstringstart,saveFilename);
      }
      *complen=(ULONG)stringtable-(ULONG)ivs+1L;

}
