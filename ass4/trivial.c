/** 
 * \file   trivial.c
 * \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 * \version 1.0
 * \brief  client program for serv, does basic guess and protocol checking
 *
 * \details
 * 
 * trivial is a command line c program that acts a trivia client; trivial is 
 * sent data, it displays this data to the user and sends back the users 
 * guess.
 *
 * trivial will do some basic checking on the users input, ensure that it is
 * numeric only and greater than 0.
 *
 * It will also interpret the raw data sent to it into a more readable form. 
 * If the data cannot be converted to this form (indicitive of a bad data), a
 * protocol error will occur and trivial will stop.
 *
 * All commenting is designed to be compatible with Doxygen.
 */

#include "trivialSupport.h"

int main(int argc, char* argv[]) {
    TrivialData td;
    int port;
    struct in_addr ipAddress;
    pthread_t threadOne;
    pthread_t threadTwo;

    // Check if args are valid
    int err = args_valid(argc, argv, &port, &ipAddress);

    if (err) {
        return err;
    }

    // Set up the structure. Include a semaphore for detecting either end of
    // input or server disconnected
    td.serverCon = connect_to(&ipAddress, port);
    td.clientCon = STDIN_FILENO;
    td.numOptions = 0;
    td.errNum = 0;
    td.cBuffer = (char*) malloc(sizeof(char));
    td.sBuffer = (char*) malloc(sizeof(char));
    sem_init(&td.blocker, 0, 0);

    // Identify trivials user with the server
    send_message(td.serverCon, argv[1]);
    send_message(td.serverCon, "\n");

    pthread_create(&threadOne, NULL, get_server_messages, (void*) &td);
    pthread_create(&threadTwo, NULL, get_client_input, (void*) &td);

    // Wait until either of the threads return and retrieve the current error
    sem_wait(&td.blocker);
    int curErrNum = td.errNum;

    // Clean up and print error
    pthread_cancel(threadOne);
    pthread_cancel(threadTwo);
    pthread_join(threadOne, NULL);
    pthread_join(threadTwo, NULL);

    free(td.cBuffer);
    free(td.sBuffer);

    print_error(curErrNum);

    return curErrNum;
}