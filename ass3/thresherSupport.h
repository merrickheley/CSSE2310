/** 
 * \file   thresherSupport.h
 * \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 * \version 1.0
 * \brief  header file for thresherSupport.c
 *
 * \details
 *
 * All commenting is designed to be compatible with Doxygen.
 */

#include "ansiC.h"
#include "java.h"
#include "latex.h"
#include "misc.h"

/** \struct ThresherStruct
 *  \brief Creates a structure that values for thresher that have to be
 *          passed around often
 */
typedef struct {
    int childOutput[2];     /**< File descript for child output  */
    int childInput[2];      /**< File descript for child input  */
    int childError[2];      /**< File descript for child error  */
    pid_t pid;              /**< Pid of the process (0 for child) */
    int type;               /**< Type of the program */
    char *curFile;          /**< String for the current file */
    char *cmd;              /**< String for the command to compile with */
    int show;               /**< Boolean for show enabled */
} ThresherStruct;

pid_t childPid; //!< Global var. Stores child's pid if exists, otherwise 0.

/**\details
 * Handle threshers arguments. 
 *
 * Set the show value if "--show" has been given, check if the minimum number
 * of arguments have been given. If the minimum arguments have not been given,
 * quit, otherwise set the type value. If an invalid type is given, quit.
 *
 * If any errors are encountered they are sent down the error pipe to the 
 * parent and the child will quit.
 *
 * \param argc (int of the number of arguments)
 * \param argv (string array containing the arguments)
 * \param show (int that shows whether show is enabled, value is modified 
 * by the function)
 * 
 * \return type (int describing what type thresher is running)
 */
int arg_handler(int argc, char** argv, int *show);

/**\details
 * Creates the child process. 
 * 
 * Sets up an error pipe, an input pipe and an 
 * output pipe, with the input pipe replacing stdin and the output pipe
 * replacing stdout. Close the unused ends of the pipe. If this was 
 * successful, exec the required process. If exec fails, quit.
 *
 * If any errors are encountered they are sent down the error pipe to the 
 * parent and the child will quit.
 *
 * \param ts (ThresherStruct with initialised values)
 */
void create_child(ThresherStruct *ts);

/**\details
 * Creates the parent process. 
 * 
 * Set up the error, input and output pipes so
 * that the parent can read from the child, then parse the output given by
 * the child. Once the output has been parsed, parse the error pipe to see
 * if the program encounted any errors. Finally, build the table and close
 * the pipes.
 *
 * If any errors are encountered or the child passed errors down the error
 * pipe, quit with the relevant return value.
 *
 * \param childPid global variable used (set to 0 when child is reaped)
 * \param ts (ThresherStruct with initialised values)
 */
void create_parent(ThresherStruct *ts);

/**\details
 * Parses the output given by the child. 
 * 
 * Create a loop that will read the childs output into a buffer while the 
 * child is still sending data. If show is true output the buffer, then 
 * call the parse function relevant to thresher's type of and increase 
 * the table value that it returns. Finally, free the buffer.
 *
 * \param ts (ThresherStruct with initialised values)
 * \param table (int table of size 7, value is modified by the function)
 * \param writePipe (file pointer that passes data to the child)
 * \param readPipe (file pointer that passes data from the child)
 */
void parse_child(ThresherStruct *ts, int table[], 
        FILE *writePipe, FILE *readPipe);

/**\details
 * Parses the error pipe from the child. 
 * 
 * Create a loop that will read the childs error pipe into a buffer while
 * the child is still sending data. If a relevant error message is recieved, 
 * quit the parent with the relevant status.
 *
 * \param errPipe (file pointer that passes error data from the child)
 */
void parse_child_error(FILE *errPipe);

/**\details
 * Builds the table that shows what errors were encountered. 
 * 
 * Call the relevant build function relevant to thresher's type. 
 *
 * \param ts (ThresherStruct with initialised values)
 * \param table (int table of size 7)
 */
void build_table(ThresherStruct *ts, int table[]);

/**\details
 * Handles threshers response to SIGINT
 * 
 * Kill thresher's children using SIGTERM and then exit with status 0
 *
 * \param childPid global variable used
 * \param s (The signal number that cauased the function to be called)
 */
void sigint_recieved(int s);