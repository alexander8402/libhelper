/**
 * 
 *     libhelper
 *     Copyright (C) 2019, @h3adsh0tzz
 *
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include "macho.h"


/**
 *  libhelper-macho - Load Commands Documentation.
 * 
 *  == Introduction ==
 * 
 *  The way Mach-O Load Commands are handled in libhelper are as follows:
 *  
 *  The base Load Command, meaning the mach_load_command_t struct that just
 *  contains the command type and size, is read from the file. This is then
 *  added to a mach_command_info_t structure that also contains the offset of
 *  the LC in the file, this is so it can be re-read later when we want to
 *  grab the entire load command, not just the type and size.
 * 
 *  The macho_t structure has a GSList of Load Commands, they are stored in
 *  that list in the mach_command_info_t structure. 
 * 
 *  So, there are two ways one can read a Load Command. 1) Use the Mach-O
 *  loading, or 2) Specify an offset of the Load Command and a file.
 * 
 *  1)  Use implemented Mach-O Loading
 *  
 *      Load commands are stored in the macho_t struct as a glib GSList. So
 *      they can be retrieved using GSList functions.
 * 
 *      The first example is fetching a Load Command at a given index that you
 *      already know:
 *      
 *          mach_command_info_t *cmdinfo = malloc (sizeof(mach_command_info_t));
 *          cmdinfo = (mach_command_info_t *) g_slist_nth_data (mach->lcmds, 2);
 * 
 *      Another method is using a for loop to go through each Load Command and,
 *      for example, print it out:
 *  
 *          GSList *cmds = mach->lcmds;
 *          for (int i = 0; i < g_slist_length (cmds); i++) {
 * 
 *              // Fetch command at index `i`
 *              mach_command_info_t *tmp = malloc (sizeof(mach_command_info_t));
 *              tmp = (mach_command_info_t *) g_slist_nth_data (mach->lcmds, i);
 * 
 *              // Print the entire load command info struct (includes libhelper data)
 *              mach_load_command_info_print (tmp);
 *          }
 * 
 *      More on printing load commands later.
 *
 * 
 *  2)  Specify an offset of a Load Command.
 * 
 *      Two function calls, one to create a command info structure in memory,
 *      the other to load the data from an offset in the file into that struct.
 * 
 *          // Create a new mach_command_info_t
 *          mach_command_info_t *lc = mach_command_info_create ();
 * 
 *          // Load a LC with a given file and offset.
 *          lc = mach_command_info_load (file_t, off_t);
 * 
 *  A list of all Load Commands (excluding Segment Commands) can be retrieved
 *  either by calling mach_load_command_info_get_list(), or simply grabbing the
 *  `lcmds` property of a macho_t, providing the macho_load() function has been
 *  invoked.
 * 
 * 
 *  == Command Dump / Printing ==
 * 
 *  A Load Command can be printed with a single function call. The Command type
 *  is detected and everything relevant is printed.
 * 
 *      // Load an LC (see above)
 *      lc = mach_command_info_load (macho, offset);
 * 
 *      // Print the command
 *      mach_command_info_print (lc);
 * 
 *      // Print the entire load command info struct (includes libhelper data)
 *      mach_load_command_info_print (tmp);
 * 
 *      // Print just the load command data
 *      mach_load_command_print (tmp, LC_INFO);
 *      mach_load_command_print (tmp->lc, LC_RAW)
 * 
 *  In the above example, there are two options for printing load command data,
 *  either the entire mach_command_info_t struct, or just the actual load
 *  command data stored within the file. Notice the seccond arg, this is a flag
 *  to tell the print function the type of struct it has passed, so you can either
 *  give the function a mach_command_info_t or mach_load_command_t struct.
 * 
 * 
 *  == Command Types ==
 * 
 *  There are a wide range of command types. As Segment Commands are so common,
 *  and contain so much more data, they are implemented seperately and stored
 *  seperately in the macho_t structure. 
 * 
 *  The only load commands that have more than one present in a Mach-O file are
 *  segment commands, so instead of returning a list for commands that will only
 *  ever have one element, fetching different command data is impleted as follows,
 *  with fetching the LC_SOURCE_VERSION Load Command as an example.
 * 
 *      mach_source_version_t *srcv = mach_command_get_source_version_lc (macho);
 * 
 *  Operations on different Load Commands could be implemented at a later stage.
 * 
 */


//////////////////////////////////////////////////////////////////////////
//                  Base Mach-O Load commands                           //
//////////////////////////////////////////////////////////////////////////


/**
 *  Function:   mach_load_command_create
 *  ------------------------------------
 * 
 *  Creates a new Mach-O Load Command structure and assigns sufficient memory. Should
 *  be called to safely create a new raw Load Command structure.
 * 
 *  returns:    A mach_load_command_t structure with sufficient allocated memory.
 * 
 */
mach_load_command_t *mach_load_command_create ()
{
    mach_load_command_t *lc = malloc(MACH_LOAD_COMMAND_SIZE);
    memset (lc, '\0', sizeof(mach_load_command_t));
    return lc;
}


/**
 *  mach_command_info_create ()
 * 
 *  Create a new Mach-O Load Command Info structure and assign enough memory 
 *  for it.
 */

/**
 *  Function:   mach_command_info_create
 *  ------------------------------------
 * 
 *  Creates a new Mach-O Load Command Info structure and assigns sufficient memory. 
 *  Should be called to safely create a new Load Command Info structure.
 * 
 *  returns:    A mach_command_info_t structure with sufficient allocated memory.
 * 
 */
mach_command_info_t *mach_command_info_create ()
{
    mach_command_info_t *cmd = malloc(MACH_COMMAND_INFO_SIZE);
    memset (cmd, '\0', sizeof(mach_command_info_t));
    return cmd;
}


/**
 *  Function:   mach_command_info_load
 *  ----------------------------------
 * 
 *  Loads a raw Mach-O Load Command from a given offset in a verified Mach-O file, and
 *  returns the resulting structure.
 *  
 *  file:       The verified Mach-O file.
 *  offset:     The offset of the Load Command within the file.
 * 
 *  returns:    A verified Mach Command Info structure.
 * 
 */
mach_command_info_t *mach_command_info_load (file_t *file, off_t offset)
{
    mach_command_info_t *ret = mach_command_info_create ();
    mach_load_command_t *lc = mach_load_command_create ();

    lc = (mach_load_command_t *) file_load_bytes (file, MACH_LOAD_COMMAND_SIZE, offset);
    if (!lc) {
        g_print ("[*] Error: Unable to find Load Command at offset 0x%llx\n", offset);
        return NULL;
    }

    ret->lc = lc;
    ret->off = offset;
    ret->type = lc->cmd;

    return ret;
}


/**
 *  Function:   mach_load_command_info_get_list
 *  -------------------------------------------
 * 
 *  Returns a list of Mach-O Load Command Info (mach_command_info_t) structs from a given
 *  Mach-O struct.
 * 
 *  mach:       The Mach-O file containing a list of Load Commands
 * 
 *  returns:    A GSList of Command Info structs.
 * 
 */
GSList *mach_load_command_info_get_list (macho_t *mach)
{
    return mach->lcmds;
}


/**
 *  Function:   mach_load_command_info_print
 *  ----------------------------------------
 * 
 *  Prints a given mach_command_info_t struct in a formatted way. 
 * 
 *  cmd:        The Mach Command Info to print
 * 
 */
void mach_load_command_info_print (mach_command_info_t *cmd)
{
    mach_load_command_print (cmd, LC_INFO);
    g_print ("--- Meta:\n");
    g_print ("  Type:\t0x%x\n", cmd->type);
    g_print ("Offset:\t0x%llx\n", cmd->off);
}


/**
 *  Function:   mach_load_command_print
 *  -----------------------------------
 * 
 *  Prints a given struct, either mach_load_command_t or mach_command_info_t
 *  in a formatted way, guided by a flag.
 * 
 *  cmd:        The Mach Load Command (mach_load_command_t / mach_command_info_t)
 *  flag:       LC_RAW / LC_INFO
 * 
 */
void mach_load_command_print (void *cmd, int flag)
{
    // flag = 0     -   cmd is mach_load_command_t
    // flag = 1     -   cmd is mach_command_info_t

    mach_load_command_t *lc = mach_load_command_create ();
    if (flag == LC_RAW) {
        lc = (mach_load_command_t *) cmd;
    } else if (flag == LC_INFO) {
        mach_command_info_t *inf = (mach_command_info_t *) cmd;
        lc = (mach_load_command_t *) inf->lc;
    } else {
        g_print ("[*] Error: Unknown Load Command print flag: 0x%x\n", flag);
        return;
    }

    g_print ("     Command:\t%s\n", mach_load_command_get_string (lc));
    g_print ("Command Size:\t%d\n", lc->cmdsize);
}


/**
 *  Function:   mach_load_command_get_string
 *  ----------------------------------------
 * 
 *  Returns a string representation of the Load Command Type, for example
 *  LC_SOURCE_VERSION.
 * 
 *  lc:         The Load Command to translate to a string
 * 
 *  returns:    The string representation of the Load Command.
 * 
 */
char *mach_load_command_get_string (mach_load_command_t *lc)
{
    if (!lc->cmd) {
        g_print ("[*] Error: lc->cmd not valid\n");
        exit (0);
    }
    char *cmd_str = "";
    switch (lc->cmd) {
        case LC_SEGMENT:
            cmd_str = "LC_SEGMENT";
            break;
        case LC_SYMTAB:
            cmd_str = "LC_SYMTAB";
            break;
        case LC_SYMSEG:
            cmd_str = "LC_SYMSEG";
            break;
        case LC_THREAD:
            cmd_str = "LC_THREAD";
            break;
        case LC_UNIXTHREAD:
            cmd_str = "LC_UNIXTHREAD";
            break;
        case LC_LOADFVMLIB:
            cmd_str = "LC_LOADFVMLIB";
            break;
        case LC_IDFVMLIB:
            cmd_str = "LC_IDFVMLIB";
            break;
        case LC_IDENT:
            cmd_str = "LC_IDENT";
            break;
        case LC_FVMFILE:
            cmd_str = "LC_FVMFILE";
            break;
        case LC_PREPAGE:
            cmd_str = "LC_PREPAGE";
            break;
        case LC_DYSYMTAB:
            cmd_str = "LC_DYSYMTAB";
            break;
        case LC_LOAD_DYLIB:
            cmd_str = "LC_LOAD_DYLIB";
            break;
        case LC_ID_DYLIB:
            cmd_str = "LC_ID_DYLIB";
            break;
        case LC_LOAD_DYLINKER:
            cmd_str = "LC_LOAD_DYLINKER";
            break;
        case LC_ID_DYLINKER:
            cmd_str = "LC_ID_DYLINKER";
            break;
        case LC_PREBOUND_DYLIB:
            cmd_str = "LC_PREBOUND_DYLIB";
            break;
        case LC_ROUTINES:
            cmd_str = "LC_ROUTINES";
            break;
        case LC_SUB_FRAMEWORK:
            cmd_str = "LC_SUB_FRAMEWORK";
            break;
        case LC_SUB_UMBRELLA:
            cmd_str = "LC_SUB_UMBRELLA";
            break;
        case LC_SUB_CLIENT:
            cmd_str = "LC_SUB_CLIENT";
            break;
        case LC_SUB_LIBRARY:
            cmd_str = "LC_SUB_LIBRARY";
            break;
        case LC_TWOLEVEL_HINTS:
            cmd_str = "LC_TWOLEVEL_HINTS";
            break;
        case LC_PREBIND_CKSUM:
            cmd_str = "LC_PREBIND_CKSUM";
            break;
        case LC_LOAD_WEAK_DYLIB:
            cmd_str = "LC_LOAD_WEAK_DYLIB";
            break;
        case LC_SEGMENT_64:
            cmd_str = "LC_SEGMENT_64";
            break;
        case LC_ROUTINES_64:
            cmd_str = "LC_ROUTINES_64";
            break;
        case LC_UUID:
            cmd_str = "LC_UUID";
            break;
        case LC_RPATH:
            cmd_str = "LC_RPATH";
            break;
        case LC_CODE_SIGNATURE:
            cmd_str = "LC_CODE_SIGNATURE";
            break;
        case LC_SEGMENT_SPLIT_INFO:
            cmd_str = "LC_SEGMENT_SPLIT_INFO";
            break;
        case LC_REEXPORT_DYLIB:
            cmd_str = "LC_REEXPORT_DYLIB";
            break;
        case LC_LAZY_LOAD_DYLIB:
            cmd_str = "LC_LAZY_LOAD_DYLIB";
            break;
        case LC_ENCRYPTION_INFO:
            cmd_str = "LC_ENCRYPTION_INFO";
            break;
        case LC_DYLD_INFO:
            cmd_str = "LC_DYLD_INFO";
            break;
        case LC_DYLD_INFO_ONLY:
            cmd_str = "LC_DYLD_INFO_ONLY";
            break;
        case LC_LOAD_UPWARD_DYLIB:
            cmd_str = "LC_LOAD_UPWARD_DYLIB";
            break;
        case LC_VERSION_MIN_MACOSX:
            cmd_str = "LC_VERSION_MIN_MACOSX";
            break;
        case LC_VERSION_MIN_IPHONEOS:
            cmd_str = "LC_VERSION_MIN_IPHONEOS";
            break;
        case LC_FUNCTION_STARTS:
            cmd_str = "LC_FUNCTION_STARTS";
            break;
        case LC_DYLD_ENVIRONMENT:
            cmd_str = "LC_DYLD_ENVIRONMENT";
            break;
        case LC_MAIN:
            cmd_str = "LC_MAIN";
            break;
        case LC_DATA_IN_CODE:
            cmd_str = "LC_DATA_IN_CODE";
            break;
        case LC_SOURCE_VERSION:
            cmd_str = "LC_SOURCE_VERSION";
            break;
        case LC_DYLIB_CODE_SIGN_DRS:
            cmd_str = "LC_DYLIB_CODE_SIGN_DRS";
            break;
        case LC_ENCRYPTION_INFO_64:
            cmd_str = "LC_ENCRYPTION_INFO_64";
            break;
        case LC_LINKER_OPTION:
            cmd_str = "LC_LINKER_OPTION";
            break;
        case LC_LINKER_OPTIMIZATION_HINT:
            cmd_str = "LC_LINKER_OPTIMIZATION_HINT";
            break;
        case LC_VERSION_MIN_TVOS:
            cmd_str = "LC_VERSION_MIN_TVOS";
            break;
        case LC_VERSION_MIN_WATCHOS:
            cmd_str = "LC_VERSION_MIN_WATCHOS";
            break;
        case LC_NOTE:
            cmd_str = "LC_NOTE";
            break;
        case LC_BUILD_VERSION:
            cmd_str = "LC_BUILD_VERSION";
            break;
        case LC_DYLD_EXPORTS_TRIE:
            cmd_str = "LC_DYLD_EXPORTS_TRIE";
            break;
        case LC_DYLD_CHAINED_FIXUPS:
            cmd_str = "LC_DYLD_CHAINED_FIXUPS";
            break;
        default:
            cmd_str = "LC_UNKNOWN";
            break;
    }
    return cmd_str;
}


//////////////////////////////////////////////////////////////////////////
//                     Specific Load Commands                           //
//////////////////////////////////////////////////////////////////////////

