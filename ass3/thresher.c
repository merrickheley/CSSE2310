/** 
 * \file   thresher.c
 * \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 * \version 1.0
 * \brief  Summarises compiler output for ansiC, C99, java and LaTeX
 * 
 * \details
 *
 * Usage: thresher [--show] type command filename ...
 *
 * thresher is a program that summarises compiler outputs, with support for
 * ansiC, c99, java and latex (specified in type). The path to the program to
 * use is specified in command, and can summarise specified after command 
 * (seperated by spaces).
 *
 * All commenting is designed to be compatible with Doxygen.
 */

#include "thresherSupport.h"

int main(int argc, char** argv) {

    //Set childPid (global variable)
    childPid = 0;

    // Contains key information for thresher
    ThresherStruct ts;

    // Sets up thresher to handle SIGINT signals.
    struct sigaction sa;
    sa.sa_handler = sigint_recieved;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, 0);

    //Sets the type, show, and cmd values
    ts.type = arg_handler(argc, argv, &ts.show);
    ts.cmd = argv[2 + ts.show];

    // Loop across all files given in arguments
    for (int i = 3 + ts.show; i < argc; ++i) { 

        // Get the current file
        ts.curFile = argv[i];

        // Create the pipes, throwing an error if the system call fails
        if (pipe(ts.childError) || pipe(ts.childInput) 
                || pipe(ts.childOutput)) {
            quit(ERR_SYS, ts.show + 2);
        }

        // Fork and grab the pid (modify childPid global)
        // -1 failed, 0 child, otherwise parent.
        ts.pid = fork();
        switch (childPid = ts.pid) {
            // There was an error with fork(), quit and tell the user
            case -1:
                quit(ERR_SYS, ts.show+2);
                break;
            // Create the child
            case 0:
                create_child(&ts);
                break;
            // Create the parent
            default:
                create_parent(&ts);
                break;
        }
    }
    return 0;
}