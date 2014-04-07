/** 
 * \file   java.c
 * \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 * \version 1.0
 * \brief  Contains the java functions associated with thresher
 *
 * \details
 * 
 * Contains the supporting functions for building java tables and parsing 
 * java buffers
 *
 * All commenting is designed to be compatible with Doxygen.
 */

#include "java.h"

void java_build_table(int *table, char *curFile) {

    // If the table value is greater than 0, print the error and the number
    // of times the error occurred.
    if (table[1]) {
        printf("%d missing identifier\n", table[1]);
    }
    if (table[2]) {
        printf("%d missing symbol\n", table[2]);
    }
    if (table[3]) {
        printf("%d non-static access\n", table[3]);
    }
    if (table[4]) {
        printf("%d other\n", table[4]);
    }

    // If the child exited normally, give the exit status. Otherwise inform
    // the user.
    child_exit_status(curFile, table[6]);
}

int java_parse(char **buffer, char *curFile) {

    // If the buffer does not begin with "name:number":, return 0
    if (!is_name_no(buffer, curFile)) {
        return 0;
    }

    // Check if the buffer contains "<identifier> expected", if so return 1.
    if (strstr(*buffer, "<identifier> expected")) {
        return 1;
    }

    // Check if the buffer contains "cannot find symbol", if so return 2.
    if (strstr(*buffer, "cannot find symbol")) {
        return 2;
    }

    // Check if the buffer contains "static context", if so return 3.
    if (strstr(*buffer, "static context")) {
        return 3;
    }

    return 4;
}