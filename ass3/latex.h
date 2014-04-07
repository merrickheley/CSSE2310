/** 
 * \file   latex.h
 * \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 * \version 1.0
 * \brief  header file for latex.c
 *
 * \details
 *
 * All commenting is designed to be compatible with Doxygen.
 */

#include "misc.h"

/**\details
 * Builds the C table that shows what errors were encountered. 
 * 
 * table[0]: math mode error
 * table[1]: bad macro
 * table[2]: warning
 * table[3]: error
 * table[4]: bad box
 *
 * If the child exited normally, give the exit status. Otherwise inform the 
 * user. If the child exited with a non-zero status, quit with status 5.
 *
 * \param table (int table of size 7)
 * \param curFile (string for the current file)
 */
void latex_build_table(int *table, char *curFile);

/**\details
 * Parses the buffer under a set of rules, and returns a value based on a
 * set of rules.
 *
 * \param buffer (string containing the last read line from the child)
 * \param curFile (string for the current file)
 * \param writePipe (file pointer to send data to the child)
 * 
 * \return 0 if the buffer does not contains "! Missing $ inserted."
 * \return 1 if the buffer contains "! Undefined control sequence."
 * \return 2 if the buffer contains "LaTeX Warning"
 * \return 3 if the buffer contains "LaTeX Error"
 * \return 4 if the buffer contain "Overfull \\hbox"
 * \return 5 otherwise
 */
int latex_parse(char **buffer, char *curFile, FILE *writePipe);