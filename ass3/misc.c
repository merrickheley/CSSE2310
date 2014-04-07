/** 
 * \file   misc.c
 * \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 * \version 1.0
 * \brief  Contains miscellaneous functions used throughout thresher
 * 
 * All commenting is designed to be compatible with Doxygen.
 */

#include "misc.h"

void child_exit_status(char *curFile, int status) {
    
    // If the child exited normally, give the exit status. Otherwise inform
    // the user.
    if (WIFEXITED(status)) {
        printf("%s exited with status %d\n", curFile, WEXITSTATUS(status));
    } else {
        printf("%s did not exit normally\n", curFile);
    }

    // If the child exited with a non-zero status, quit.
    if (WEXITSTATUS(status) != 0) {
        quit(ERR_NONZERO, 1);
    }
}

void child_quit(FILE *errPipe, int status) {

    // Send the status to the parent
    fprintf(errPipe, "%d\n", status);
    fflush(errPipe);
    exit(status);
}

int is_name_no(char **buffer, char *curFile) {
    
    char *file, *num;
    int isNum = 1;
    int isFile = 0;

    // Allocate memory for both the file and the num string, both of which
    // have the potentional to take up the entire buffer.
    file = (char *) malloc(sizeof(char) * strlen(*buffer));
    num = (char *) malloc(sizeof(char) * strlen(*buffer));

    // Use sscanf to extract te file and the num 
    // %[^:] indicates a string that does not contain ":"
    sscanf(*buffer, "%[^:]:%[^:]:", file, num);

    // Iterate throw the num string and check that all characters in
    // the string were numbers
    for (int i = 0; i < strlen(num); ++i) {
        if (num[i] < 48 || num[i] > 57) {
            isNum = 0;
            break;
        }
    }

    // Check if the file name grabbed from the buffer is the same as the
    // filename given in the argument
    isFile = !strcmp(curFile, file);

    free(file);
    free(num);
    
    // If the file name is the same and the num string is actually a number
    // return true
    return (isNum && isFile);

}

int get_line(char **buffer, FILE *input) { 
   
    int c, count;
    char *tempBuffer = (char *) malloc(sizeof(char)*1);

    count = 0;

    // Run while there are still characters to get
    while ((c = fgetc(input)) > 0) {
        
        // Reallocate the buffer and increase its size by 1
        tempBuffer = (char *) realloc(tempBuffer, 
                sizeof(char) * (strlen(tempBuffer) + 1));

        // Assign the character read to the buffer
        tempBuffer[count] = c;

        // If the string is done, set the buffer
        if (c == '\n' || c == '\0') {
            tempBuffer[count] = '\0';

            *buffer = tempBuffer;

            return 1;
        }

        count++;
    }

    free(tempBuffer);

    return 0;
}

void quit(int status, int printLines) {

    // Print the message corresponding to the exit status given
    switch (status) {
        case ERR_USAGE:
            fprintf(stderr, "Usage: thresher [--show] type command "\
                    "filename ...\n");
            break;
        case ERR_UNKNOWN:
            fprintf(stderr, "Unknown build type\n");
            break;
        case ERR_EXEC:
            fprintf(stderr, "Exec failed\n");
            break;
        case ERR_SYS:
            fprintf(stderr, "System error\n");
            break;
    }

    // Print the remaining lines of dashes
    for (int i = 0; i < printLines; ++i) {
        printf("----\n");
    }

    exit(status);
}