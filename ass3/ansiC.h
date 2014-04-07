/** 
 * \file   ansiC.h
 * \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 * \version 1.0
 * \brief  header file for ansiC.c
 *
 * \details
 *
 * All commenting is designed to be compatible with Doxygen.
 */

#include "misc.h"

/**\details
 * Builds the C table that shows what errors were encountered. 
 * 
 * table[1]: Implicit declaration
 * table[2]: undeclared
 * if ansiC:
 *     table[3]: c99
 *     table[4]: c++ comment
 * table[5]: other
 *
 * If the child exited normally, give the exit status. Otherwise inform the 
 * user. If the child exited with a non-zero status, quit with status 5.
 *
 * \param table (int table of size 7)
 * \param curFile (string for the current file)
 * \param type (int describing what type thresher is running)
 */
void c_build_table(int *table, char *curFile, int type);

/**\details
 * Parses the buffer under a set of rules, and returns a value based on a
 * set of rules.
 *
 * \param buffer (string containing the last read line from the child)
 * \param curFile (string for the current file)
 * \param type (int describing what type thresher is running)
 * 
 * \return 0 if the buffer does not contains "name:number:", or contains
 * "note:", "error (Each undeclared", or "error: for each function"
 * \return 1 if the buffer contains "implicit declaration"
 * \return 2 if the buffer contains "undeclared"
 * \return 3 if the bufer contains "C99" and type is 0
 * \return 4 if the buffer contains "expected expression before '/' token" 
 * and type is 0
 * \return 5 otherwise
 */
int c_parse(char **buffer, char *curFile, int type);