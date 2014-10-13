/****************************************************************************/
/*                                                                          */
/* COPYRIGHT IBM CORP., 1992                                                */
/*                                                                          */
/****************************************************************************/
#define SUCCESS               0
#define CANCEL                1
#define ERRORCONDITION        2

#define  ID_APP     256


#define  MAINDLG     257
#define  FILEDLG     258
#define  LSTDLG     259

#define  TESTDLG     260
#define  SORCDLG     261
#define  GNRTDLG     262
#define  PRPRDLG     263
#define  SAVETIT     265
#define  SAVELSE     266


#define  DLG_LIST    270
#define  OPTMDLG     356
#define  OPT_NOPT    357
#define  OPT_MAX     358
#define  OPT_SPEED   359
#define  OPT_LOOP    360
#define  OPT_PREC    361
#define  OPT_ALIAS   362
#define  OPT_SPACE   363
#define  OPT_IALIAS  364
#define  OPT_INTRIN  365
#define  OPT_SAFE    366
#define  OPT_DINLNE  367

#define  CGENDLG     400
#define  GENMODELST  401
#define  GENMODELCUST 406
#define  GENFLOATST  408
#define  GENPASCAL   414
#define  GENPM       415
#define  GENSTACK    416
#define  GENCONST    417
#define  GENTHRESH   418
#define  GENTHRESH_ENTRY   419

#define  GENCUSTDLG  500
#define  GENCUSTSHRT 502
#define  GENCUSTNEAR 505
#define  GENCUSTSSDS 509

#define CTLDLG       600
#define CTLBOX1      601
#define CTLTYPEEXE   602
#define CTLTYPEDLL   603
#define CTLBOX2      604
#define CTLDEBNONE   605
#define CTLDEBLINE   606
#define CTLDEBSYM    607
#define CTLNMLGTH    608
#define CTLVERSION   609
#define CTLDATASEG   610
#define CTLCODESEG   611
#define CTLDEFLIB    612
#define CTLUSEB1     613

#define PREPDLG      700
#define PREPLSTGRP   701
#define PREPLSTFILE  702
#define PREPLSTSOWL  703
#define PREPLSTSONL  704
#define PREPCOMMENT  705
#define PREPEXCLUDE  706
#define PREPUNDALL   707
#define PREPINCLUDE  708
#define PREPDEFINE   709
#define PREPUNDEFINE 710

#define OUTFDLG      800
#define OUTSRCOPT    801
#define OUTSRCNAME   802
#define OUTMAPOPT    803
#define OUTMAPNAME   804
#define OUTOBJFNAME   805
#define OUTOBJGRP    806
#define OUTOBJOBJ    807
#define OUTOBJASM    808
#define OUTOBJCOM    809
#define OUTOBJNAME   810
#define OUTEXENAME   811

#define LISTDLG      900
#define LSTWARNGRP   901
#define LSTWARN0     902
#define LSTWARN1     903
#define LSTWARN2     904
#define LSTWARN3     905
#define LSTLINE      906
#define LSTPAGE      907
#define LSTTITLE     908
#define LSTSUBTITLE     909


#define SCTLDLG      1000
#define SCTLDEXT     1001
#define SCTLSYNTAX   1002
#define SCTLDECL     1003
#define SCTLPACKGRP  1004
#define SCTLPACK1    1005
#define SCTLPACK2    1006
#define SCTLPACK4    1007
#define SCTLCONLY    1008
#define SCTLJ        1009
#define SCTLCASEI    1010

#define OPTION_HELP      280
#define OPTION_RESET     281
#define OPTION_DEFAULT   282


#define      ID_HELP_APP                   255
#define      ID_HELP_DLG                   270
#define      ID_HELP_ERROR                 271

#define ID_MAIN_HELPTABLE       400
#define ID_MAIN_HELPSUBTABLE    401

#define      MSG_CMPOPTSMALL    2001
#define      MSG_CMPSTRINGSMALL 2002

#define CRLF "\x0D\x0A"
#define STOREOPTS 0x0001
#define NODLGS    0x0002
#define USEDEFAULTS 0x0004
#define BATCHOPTS 0x0008
typedef struct  {
              unsigned optnone    : 1;     /* /Od */
              unsigned optmax     : 1;     /* /Ox */
              unsigned optspeed   : 1;     /* /Ot */
              unsigned optloop    : 1;     /* /Ol */
              unsigned optprecise : 1;     /* /Op */
              unsigned optaliasfn : 1;     /* /Ow */
              unsigned optspace   : 1;     /* /Os */
              unsigned optialias  : 1;     /* /Oa */
              unsigned optintrins : 1;     /* /Oi */
              unsigned optunsafe  : 1;     /* /On */
              unsigned optinliner : 1;     /* /Or */
              unsigned PascalLink : 1;     /* /Gc */
              unsigned PMLink     : 1;     /* /Gw */
              unsigned NoStackPrb : 1;     /* /Gs */
              unsigned StringinCs : 1;     /* /Gm */
              unsigned DataThresh : 1;     /* /Gt */
              unsigned FloatOption : 3;    /* /FPa = 0 */
                                           /* /FPc = 1 */
                                           /* /FPc87 = 2 */
                                           /* /FPi   = 3 */
                                           /* /FPi87 = 4 */
              unsigned MemoryModel : 3;    /* /AS  = 0 */
                                           /* /AM  = 1 */
                                           /* /AC  = 2 */
                                           /* /AL  = 3 */
                                           /* /AH  = 4 */
                                           /* custom=5 */
              unsigned CustModelCode :1;   /* /As??=0 */
                                           /* /Al??=1 */
              unsigned CustModelData :2;   /* /A?n?=0 */
                                           /* /A?f?=1 */
                                           /* /A?h?=2 */
              unsigned CustModelSetS :2;   /* /A??d=0 */
                                           /* /A??u=1 */
                                           /* /A??w=2 */
              unsigned ObjectisExe   :1;
              unsigned DebLineNum    :1;   /* /Zd     */
              unsigned DebSymDeb     :1;   /* /Zi     */
              unsigned SaveComments  :1;   /* /C      */
              unsigned ExcludeStd    :1;   /* /X      */
              unsigned UndefineAll   :1;   /* /u      */
              unsigned RemoveDefLib  :1;   /* /Zl     */
              unsigned Prepoutput    :2;   /* none=0  */
                                           /* /P  =1  */
                                           /* /E  =2  */
                                           /* /EP =3  */
              unsigned SrcListRequired :1; /* /Fs     */
              unsigned MapFileRequired :1; /* /Fm     */
              unsigned ObjectListReq :2;   /* none=0  */
                                           /* /Fl =1  */
                                           /* /Fa =2  */
                                           /* /Fc =3  */
              unsigned WarningLevel : 2;   /* /W0 = 0 */
                                           /* /W1 = 1 */
                                           /* /W2 = 2 */
                                           /* /W3 = 3 */
              unsigned CompileOnly  : 1;   /* /c  */
              unsigned SyntaxCheck  : 1;   /* /Zs */
              unsigned GenerateDecl : 1;   /* /Zg */
              unsigned DisableExten : 1;   /* /Za */
              unsigned CharisUnsigned : 1; /* /J  */
              unsigned CaseInsensitive : 1; /* /Zc */
              unsigned StructPack      : 2; /* /Zp1 = 0 */
                                            /* /Zp2 = 1 */
                                            /* /Zp4 = 2 */

              unsigned UseB1          : 1;  /* /B1      */
              unsigned Reserved       : 13; /* bites left over */
              USHORT   SrcListFileoff;     /* /Fsxxxx */
              USHORT   MapListFileoff;     /* /Fmxxxx */
              USHORT   ObjListFileoff;     /* /F?xxxx */
              USHORT   ObjFileNameoff;     /* /Foxxxx */
              USHORT   ExeFileNameoff;     /* /Fexxxx */
              USHORT   NumIncludePaths;
              USHORT   IncludePathoff;
              USHORT   NumDefinedMacros;
              USHORT   DefinedMacrooff;
              USHORT   NumUnDefinedMacros;
              USHORT   UnDefinedMacrooff;
              USHORT   ExternNameLgth;     /* /Hxxxx   */
              USHORT   DataThreshAmt ;     /* /Gtxxxxx */
              USHORT   DsegNameoff;        /* /NDxxxxx */
              USHORT   CsegNameoff;        /* /NTxxxxx */
              USHORT   VersionStringoff;   /* /Vxxxxx  */
              USHORT   LineWidth    ;      /* /Slxxxx  */
              USHORT   PageLength   ;      /* /Spxxxx  */
              USHORT   Titlestringoff;     /* /Stxxxx  */
              USHORT   SubTitlestringoff;  /* /Ssxxxx  */


              char     stringtablestart[1];/* 1stbyte=x'FF'*/

             } COMPOPTIONS;
