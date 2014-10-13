This diskette contains documents and sample code to assist developers
integrate their tools into the WorkFrame/2 environment.  Updates
will be shipped to registered users as they become available.   The
sample code is shipped as-is with no guarantees to its correctness.
Any support of the code will be provided as time permits.


The following documents are available on this diskette:

   1. INTRFACE.DOC  - "WorkFrame/2 Programming Interfaces"
   2. NDXFORM.DOC   - "NDX File Format"
   3. WFDRAG.DOC    - "WorkFrame/2 Drag & Drop Conventions"

These documents are also available in hard copy form.  The diskette
versions are straight ASCII files and as a result, are missing the
figures showing several of the WorkFrame/2 screens.


In the SAMPLES subdirectory you will find the following C sample code:

   1. INSTALL.C   - This file contains a subroutine which will help you
                    identify whether the WorkFrame/2 has already been 
                    installed.  If installed, it will return its installation
                    directory.  This directory should be used for installing
                    any PRJ and PRF (project control and language profiles)
                    files during your product installation.
 
   2. WRITEPRJ.C  - This file contains a complete subroutine which will
                    allow you to dynamically create a PRJ file.  This will
                    be necessary if you intend on intalling sample projects
                    and wish to automatically make its PRJ file available.
                    This subroutine can also be used to determine the layout
                    format of the PRJ file.

   3. LANGPROF.H  - This provides the structure layout of the language profile
                    (PRF) file.  This information will be required if you
                    need to dynamically create a language profile as part
                    of your installation.


In addition to the sample code, you will also find the following utilities:

   1. CRTPRF.EXE  - This is our own version of the MAKEINI.EXE used
                    for creating the necessary INI input file to the ADDTOOL
                    utility. The CRTPRF has larger string length limits to
                    match that of the Tool descriptions within the 
                    WorkFrame/2. 

      SAMPLE.RC   - This is a sample RC input file to CRTPRF.EXE which
                    installs 2 distinct tools into the WorkFrame/2 Tools 
                    pulldown menu.  It is also self-documented, describing
                    the RC file format as well as the various options.

   2. WF2DDE.EXE  - This PM program simulates the editor DDE server
                    component.  It can be used to ensure that the
                    proper compiler error information is being passed
                    to the editor session when dragged(!) to this process.
                    It is a good test for the following compiler support
                    APIs while in dragging mode:  PARSEERROR, QUERYHELPFILE,
                    QUERYRESOURCEID.  Note that the program should be
                    started as a tool and not as an editor.


Also in the SAMPLES subdirectory, there are 2 language profiles:

   1. DOSRUN.PRF  - This profile uses the DEFAULT.DLL and the DOSRUN 
                    components to provide support for developing DOS 
                    applications.

   2. MSC600.PRF  - This profile uses the IBMCCOMP.DLL (sample user built
                    DLL for non-supporting compilers), to provide greater
                    WorkFrame/2 integration then that provided by the
                    DEFAULT.DLL.


The following samples are also included in this diskette:

   1. DOSRUN contains a composite project "DOSRUN" and 2 base projects
      "DOSCL1" and "DOSCL2".  This project works in conjuntion with
      the DOSRUN.PRF language profile to provide native DOS compiler
      support.

   2. IBMC2 contains all the source required to build the IBMCCOMP.DLL.
      This DLL provides the compiler support for IBM C/2 and Microsoft
      C 5.0 to 6.0 compilers.  It does not contain suppport for creating
      make files.




If you have any technical questions, forward them to

John Kehm
IBM Canada Ltd.,
844 Don Mills Rd.,
North York, Ontario,
Canada M3C 1V7

IBM VNET : KEHM at TOROLAB6
email    : workframe@vnet.ibm.com
