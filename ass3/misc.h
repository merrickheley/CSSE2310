/** 
 * \file   misc.h
 * \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 * \version 1.0
 * \brief  header file for misc.c
 *
 * \details
 * 
 * All commenting is designed to be compatible with Doxygen.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define READ 0
#define WRITE 1

#define ANSIC 0
#define CNN 1
#define JAVA 2
#define LATEX 3

#define ERR_USAGE 1
#define ERR_UNKNOWN 2
#define ERR_EXEC 3
#define ERR_SYS 4
#define ERR_NONZERO 5

/**\details
 * Generates the childs exit status
 * 
 * If the child exited normally, give the exit status. Otherwise inform the 
 * user. 
 *
 * If the child exited with a non-zero status, quit with status 5.
 *
 * \param table (int table of size 7)
 * \param curFile (string for the current file)
 */
void child_exit_status(char *curFile, int status);

/**\details
 * Informs the parent and quits the child
 * 
 * Send a status to the parent along errPipe, then exit with this same status.
 *
 * \param errPipe (file pointer to send err data to the parent)
 * \param status (the status to send to the parent and exit with)
 */
void child_quit(FILE *errPipe, int status);

/**\details
 * Checks if the buffer starts with "name:num:"
 * 
 * Allocate strings for both the file and the num, and read them from 
 * the buffer. If the num is a number and the name matches curFile, return
 * 'true'. Otherwise return 0.
 *
 * \param buffer (string containing the last read line from the child)
 * \param curFile (string for the current file) 
 *
 * \return 0 if buffer does not begin with "name:num:"
 * \return non-zero otherwise
 */
int is_name_no(char **buffer, char *curFile);

/**\details
 * Get the next line from the buffer
 * 
 * Create the temp buffer, and start a loop that runs while there is still
 * characters to read from the buffer (if this loop fails, return 0). For each
 * character read, reallocate and increase the size of the buffer by 1 and add
 * the character to the end of the string.
 *
 * If newline or endstring is reached, create the buffer, free the tempbuffer
 * and return 1.
 *
 * \param buffer (pointer to a char, value is modified by the function)
 * \param input (file pointer to the input to read into buffer) 
 *
 * \return 1 if the line has been read
 * \return 0 if there are no more lines to be read
 */
int get_line(char **buffer, FILE *input);

/**\details
 * Throw a message and quit the program
 * 
 * Throw a message to stderr corresponding to the status given
 * 1. Usage: thresher [--show] type command filename ...
 * 2. Unknown build type
 * 3. Exec failed
 * 4. System error
 *
 * Print the required amount of lines of dashes given by printLines, and
 * exit with status.
 *
 * \param status (int denoting status to quit with)
 * \param printLines (an int indicating the number of lines of dashes to be
 * printed) 
 */
void quit(int status, int printLines);