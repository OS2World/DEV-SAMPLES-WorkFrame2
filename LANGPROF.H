/****************************************************************************/
/*                                                                          */
/* COPYRIGHT IBM CORP., 1992                                                */
/*                                                                          */
/****************************************************************************/

typedef struct _ACTION
        {
        long   Offset_EXE_NameString;
        long   Offset_DLL_NameString;
        } ACTION, * pACTION;


/* All long offsets are zero-based from the start of the StringTable */
typedef struct
        {
        char   lprofid[4];             /* must be "LPRF" */

        char   eyecatcher[8];

        long   Size_Of_The_profile_In_Bytes;

        ACTION compiler;
        ACTION linker;
        ACTION maker;
        ACTION debugger;

        long   Offset_LanguageName;
        long   Offset_LlanguageMask;
        long   Offset_DefaultMake_String;
        long   Offset_DefaultDebug_String;

        long   maxcompsize;
        long   maxcompstr;
        long   maxlinksize;
        long   maxlinkstr;

        long   StringTableLengthInBytes;

        short  debugger_invocation;

        /* equals 1 if DLL is 32 bits, 0 if 16 bits - Not Required       */
        unsigned int compiler_dll_32 : 1;
        unsigned int linker_dll_32   : 1;
        unsigned int maker_dll_32    : 1;
        unsigned int debugger_dll_32 : 1;
        unsigned int Reserved1       : 12;

        long   include_variable_string_offset;
        long   lib_variable_string_offset;

        long   Reserved[21];

        char   StringTable[0];
        } LANG_PROF;
