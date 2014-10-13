# IBM Developer's Workframe/2 Make File Creation run at 14:33:11 on 05/03/92

# Make File Creation run in directory:
#   A:\DOSRUN;

.SUFFIXES:

.SUFFIXES: .c

DOSCL1.EXE:  \
  DOSCL1.OBJ \
  DOSCL1.MAK
   @REM @<<DOSCL1.@0
     DOSCL1.OBJ
     DOSCL1.EXE
     
     
     ;
<<
   LINK386.EXE @DOSCL1.@0

{.}.c.obj:
   ICC.EXE .\$*.c

!include DOSCL1.DEP
