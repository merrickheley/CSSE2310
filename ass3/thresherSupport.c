/** 
 * \file   thresherSupport.c
 * \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 * \version 1.0
 * \brief  Contains support functions for thresher.c
 * 
 * \details
 *
 * Contains supporting functions for thresher that create the child,
 * manage the parent and manages what the program does when it recieves
 * SIGINT.
 *
 * All commenting is designed to be compatible with Doxygen.
 */

#include "thresherSupport.h"

int arg_handler(int argc, char** argv, int *show) {

    int type;

    // See if the "--show" argument has been set
    if (argc > 1 && !strcmp(argv[1], "--show")) {
        *show = 1;
    } else {
        *show = 0;
    }

    // Check that the minimum number of arguments have been given.
    if (argc < 4 + *show) {
        quit(ERR_USAGE, 0);
    }

    // Set the type for thresher: 0 ansiC, 1 c99, 2 java, 3 latex
    if (!strcmp(argv[1+*show], "ansiC")) {
        type = ANSIC;
    } else if (!strcmp(argv[1+*show], "c99")) {
        type = CNN;
    } else if (!strcmp(argv[1+*show], "java")) {
        type = JAVA;
    } else if (!strcmp(argv[1+*show], "latex")) {
        type = LATEX;
    } else {
        quit(ERR_UNKNOWN, 0);
    }

    return type;
}

void create_child(ThresherStruct *ts) {

    FILE *errPipe;

    // Set up the error pipe
    errPipe = fdopen(ts->childError[WRITE], "w");

    // Replace stdin with the input sent by the parent.
    // If it fails, tell the parent and quit.
    if (dup2(ts->childInput[READ], STDIN_FILENO) == -1) {
        child_quit(errPipe, ERR_SYS);
    }

    // If latex, replace stdout with the output pipe. Otherwise, replace 
    // stderr. If any of these fail, tell the parent then quit. 
    if (ts->type == LATEX && 
            dup2(ts->childOutput[WRITE], STDOUT_FILENO) == -1) {
        child_quit(errPipe, ERR_SYS);
    } 
    
    if (ts->type != LATEX && 
            dup2(ts->childOutput[WRITE], STDERR_FILENO) == -1) {
        child_quit(errPipe, ERR_SYS);
    }

    // Close the other ends of the pipe. If close fails, tell parent and quit.
    if (close(ts->childError[READ]) || close(ts->childInput[WRITE]) 
            || close(ts->childOutput[READ])) {
        child_quit(errPipe, ERR_SYS);
    }
    
    // Exec the program depending on the type
    // 0 ansiC, 1 c99, 2 java, 3 latex
    switch (ts->type) {
        case ANSIC:
            execlp(ts->cmd, ts->cmd, "-ansi", "-pedantic", "-Wall", 
                    ts->curFile, (char *) NULL);
            break;
        case CNN:
            execlp(ts->cmd, ts->cmd, "-std=gnu99", "-pedantic", "-Wall", 
                    ts->curFile, (char *) NULL);
            break;
        case JAVA:
            execlp(ts->cmd, ts->cmd, "-d", ".", ts->curFile, (char *) NULL);
            break;
        case LATEX:
            execlp(ts->cmd, ts->cmd, ts->curFile, (char *) NULL);
            break;
    }

    // If this point has been reached, exec has failed. 
    child_quit(errPipe, ERR_EXEC);
}

void create_parent(ThresherStruct *ts) {

    FILE *errPipe, *writePipe, *readPipe;
    int table[7];

    // Set up the pipes so the parent can interact with the child
    errPipe = fdopen(ts->childError[READ], "r");
    writePipe = fdopen(ts->childInput[WRITE], "w");
    readPipe = fdopen(ts->childOutput[READ], "r");
    
    // Close the other ends of the pipes
    if (close(ts->childError[WRITE]) || close(ts->childInput[READ]) 
            || close(ts->childOutput[WRITE])) {
        quit(ERR_SYS, ts->show+2);
    }

    printf("----\n");

    // Parse the values outputted by the child, and construct a table of
    // errors (table) depending on the type;
    parse_child(ts, table, writePipe, readPipe);

    // If show is enabled, end the buffer printing
    if (ts->show) {
        printf("----\n");
    }
    
    // Check if the child sent any errors through the error pipe
    parse_child_error(errPipe);

    // Wait until the child has completed and grab its error value
    waitpid(ts->pid, &table[6], 0);

    // Clear childPid (global variable) for the signal handler
    childPid = 0;

    // Build the table coresponding to the type (and the values parsed)
    build_table(ts, table);

    printf("----\n");

    // Close the open files
    fclose(readPipe);
    fclose(writePipe);
    fclose(errPipe);
}

void parse_child(ThresherStruct *ts, int table[], FILE *writePipe, 
        FILE *readPipe) {
    
    char *buffer;

    // Clear the table
    for (int i = 0; i < 6; ++i) {
        table[i] = 0;
    }

    // grab the line to be read from the readpipe and place it in the buffer
    while (get_line(&buffer, readPipe)) {
        // If show is enabled, print the buffer
        if (ts->show) {
            printf("%s\n", buffer);
        }

        // Depending on the type, grab the parse value (corresponding to 
        // an error entered on the table). Increase the value of the entry
        // in the table.
        switch (ts->type) {
            case ANSIC:
                table[c_parse(&buffer, ts->curFile, ts->type)]++;
                break;
            case CNN:
                table[c_parse(&buffer, ts->curFile, ts->type)]++;
                break;
            case JAVA:
                table[java_parse(&buffer, ts->curFile)]++;
                break;
            case LATEX:
                table[latex_parse(&buffer, ts->curFile, writePipe)]++;
                break;
        }
        
        // Free the buffer when done parsing
        free(buffer);
    }
}

void parse_child_error(FILE *errPipe) {
    
    char *buffer;

    // Check if the child sent anything down the error pipe. If it is an
    // an error (exec/system failed), quit using this status.
    while (get_line(&buffer, errPipe)) {
        if (strstr(buffer, "3")) {
            quit(ERR_EXEC, 1);
        } else if (strstr(buffer, "4")) {
            quit(ERR_SYS, 1);
        }

        free(buffer);
    }   
}

void build_table(ThresherStruct *ts, int table[]) {

    // Build the table corresponding to the type
    switch (ts->type) {
        case ANSIC:
            c_build_table(table, ts->curFile, ts->type);
            break;
        case CNN:
            c_build_table(table, ts->curFile, ts->type);
            break;
        case JAVA:
            java_build_table(table, ts->curFile);
            break;
        case LATEX:
            latex_build_table(table, ts->curFile);
            break;
    }  
}

void sigint_recieved(int s) {

    // If child exists, kill it.
    if (childPid) {
        kill(childPid, SIGTERM);
        waitpid(childPid, NULL, 0);
    }
    quit(0, 0);
}