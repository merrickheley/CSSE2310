/** 
 * \file   latex.c
 * \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 * \version 1.0
 * \brief  Contains the latex functions associated with thresher
 * 
 * \details
 * 
 * Contains the supporting functions for building java tables and parsing 
 * java buffers
 *
 * All commenting is designed to be compatible with Doxygen.
 */

#include "latex.h"

void latex_build_table(int *table, char *curFile) {

    // If the table value is greater than 0, print the error and the number
    // of times the error occurred.
    if (table[0]) {
        printf("%d math mode error\n", table[0]);
    }
    if (table[1]) {
        printf("%d bad macro\n", table[1]);
    }
    if (table[2]) {
        printf("%d warning\n", table[2]);
    }
    if (table[3]) {
        printf("%d error\n", table[3]);
    }
    if (table[4]) {
        printf("%d bad box\n", table[4]);
    }

    // If the child exited normally, give the exit status. Otherwise inform
    // the user.
    child_exit_status(curFile, table[6]);

}

int latex_parse(char **buffer, char *curFile, FILE *writePipe) {

    // Check if the buffer contains "<identifier> expected", if so return 1
    // and send "\n" to the child
    if (strstr(*buffer, "! Missing $ inserted.")) {
        fprintf(writePipe, "\n");
        fflush(writePipe);
        return 0;
    }

    // Check if the buffer contains "! Undefined control sequence.", 
    // if so return 1 and send "\n" to the child
    if (strstr(*buffer, "! Undefined control sequence.")) {
        fprintf(writePipe, "\n");
        fflush(writePipe);
        return 1;
    }

    // Check if the buffer contains "LaTeX Warning", if so return 1
    // and send "\n" to the child
    if (strstr(*buffer, "LaTeX Warning")) {
        fprintf(writePipe, "\n");
        fflush(writePipe);
        return 2;
    }

    // Check if the buffer contains "LaTeX Error", if so return 1
    // and send "X\n" to the child
    if (strstr(*buffer, "LaTeX Error")) {
        fprintf(writePipe, "X\n");
        fflush(writePipe);
        return 3;
    }
    
    // Check if the buffer contains "Overfull \\hbox", if so return 4
    if (strstr(*buffer, "Overfull \\hbox")) {
        return 4;
    }

    return 5;
}
