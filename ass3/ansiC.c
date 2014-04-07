/** 
 * \file   ansiC.c
 * \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 * \version 1.0
 * \brief  Contains the C functions associated with thresher
 * 
 * \details
 *
 * Default is ansiC (type = 0). Also contains handling for c99 (type = 1).
 * Contains the supporting functions for building C tables and parsing 
 * C buffers
 *  
 * All commenting is designed to be compatible with Doxygen.
 */

#include "ansiC.h"

void c_build_table(int *table, char *curFile, int type) {

    // If the table value is greater than 0, print the error and the number
    // of times the error occurred.
    if (table[1]) {
        printf("%d implicit declaration\n", table[1]);
    }
    if (table[2]) {
        printf("%d undeclared\n", table[2]);
    }

    // Only print these if ansiC
    if (type == ANSIC && table[3]) {
        printf("%d c99\n", table[3]);
    }   
    if (type == ANSIC && table[4]) {
        printf("%d c++ comment?\n", table[4]);
    }

    if (table[5]) {
        printf("%d other\n", table[5]);
    }
    
    // If the child exited normally, give the exit status. Otherwise inform
    // the user.
    child_exit_status(curFile, table[6]);
}

int c_parse(char **buffer, char *curFile, int type) {

    // If the buffer does not begin with "name:number":, contains "note:",
    // "error (Each undeclared", or "error: for each function", return 0.
    if (!is_name_no(buffer, curFile) || strstr(*buffer, "note:")
            || strstr(*buffer, "error: (Each undeclared")
            || strstr(*buffer, "error: for each function")) {
        return 0;
    }

    // Check if the buffer contains "implicit declaration", if so return 1.
    if (strstr(*buffer, "implicit declaration")) {
        return 1;
    }

    // Check if the buffer contains "undeclared", if so return 2.
    if (strstr(*buffer, "undeclared")) {
        return 2;
    }

    // Check if the buffer contains "C99", if so return 3.
    if (type == ANSIC && strstr(*buffer, "C99")) {
        return 3;
    }

    // Check if the buffer contains "expected expression before '/' token", 
    // if so return 4.
    if (type == ANSIC 
            && strstr(*buffer, "expected expression before '/' token")) {
        return 4;
    }

    // Return 5 for otherwise
    return 5;
}