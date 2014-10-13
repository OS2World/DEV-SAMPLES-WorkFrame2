# IBM Developer's Workframe/2 Make File Creation run at 14:29:25 on 05/03/92

# Make File Creation run in directory:
#   A:\DOSRUN;

.SUFFIXES:

.SUFFIXES: .c

DOSCL2.EXE:  \
  DOSCL2.OBJ \
  DOSCL2.MAK
     link.exe /NOD DOSCL2.OBJ, doscl2.exe,, llibc7;

.c.obj:
   cl.exe /FPc87 /AL /c $*.c

!include DOSCL2.DEP
