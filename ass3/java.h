/** 
 * \file   java.h
 * \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 * \version 1.0
 * \brief  header file for java.c
 *
 * \details
 *
 * All commenting is designed to be compatible with Doxygen.
 */

#include "misc.h"

/**\details
 * Builds the C table that shows what errors were encountered. 
 * 
 * table[1]: missing identifier
 * table[2]: missing symbol
 * table[3]: non-static acecss
 * table[5]: other
 *
 * If the child exited normally, give the exit status. Otherwise inform the 
 * user. If the child exited with a non-zero status, quit with status 5.
 *
 * \param table (int table of size 7)
 * \param curFile (string for the current file)
 */
void java_build_table(int *table, char *curFile);

/**\details
 * Parses the buffer under a set of rules, and returns a value based on a
 * set of rules.
 *
 * \param buffer (string for the current file)
 * \param curFile (string for the current file)
 * 
 * \return 0 if the buffer does not contains "name:number:"
 * \return 1 if the buffer contains "<identifier> expected"
 * \return 2 if the buffer contains "cannot find symbol"
 * \return 3 if the bufer contains "static context"
 * \return 4 otherwise
 */
int java_parse(char **buffer, char *curFile);