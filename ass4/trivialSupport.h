/** 
 * \file   server_support.h
 * \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 * \version 1.0
 * \brief  header file for trivial_support.c
 *
 * \details
 * 
 * All commenting is designed to be compatible with Doxygen.
 */

#include "misc.h"

/** \struct TrivialData
 *  \brief Holds data for trivial that have to be passed around often
 */
typedef struct {
    int serverCon;  /**< File descriptor for the server connection */
    int clientCon;  /**< File descriptor for the user input */
    char *sBuffer;  /**< Buffer for data read from server */
    char *cBuffer;  /**< Buffer for data read from client */
    int errNum;     /**< The error number (if encountered) */
    int numOptions; /**< The number of options given by current question */

    sem_t blocker;  /**< Posted when client or serv has finished */
} TrivialData;

/**\details
 * Check the arguments are valid, with respect to correct number and data 
 * range for each variable
 *
 * \param argc (int of the number of arguments)
 * \param argv (string array of arguments)
 * \param port (the pointer to read the value of port into)
 * \param ipAddress (the pointer to a struct containing relevant address info
 *
 * \return error (int containing the error num of the first encountered error)
 */
int args_valid(int argc, char *argv[], int *port, struct in_addr *ipAddress);

/**\details
 * Get the clients input
 *
 * Input must be a number greater than 0 and less than the number of options.
 * If this is true, send it to the server, otherwise print invalid guess.
 * 
 * Exit when there is no more data to be read
 *
 * \param arg (a void pointer that can be cast as a pointer to TrivialData)
 */
void *get_client_input(void *arg);

/**\details
 * Get the messages from the server
 *
 * Take the messages from the defined protocol and parse them as outlined in
 * the spec sheet. If there are any irregularities, set the protocol error.
 *
 * If the input ends in an unusual position, set the DC error
 *
 * \param arg (a void pointer that can be cast as a pointer to TrivialData)
 */
void *get_server_messages(void *arg);

/**\details
 * Part the input (other than greeting)
 *
 * Take the messages from the defined protocol and parse them as outlined in
 * the spec sheet. If there are any irregularities, set the protocol error.
 *
 * \param td (a pointer that can be cast as a pointer to TrivialData)
 * \param marker (a pointer to an int containing the marker position)
 */
void parse_input(TrivialData *td, int *marker);

/**\details
 * Display the players scores line, and winners line if required.
 *
 * If there are any irregularities, set the protocol error.
 *
 * \param td (a pointer that can be cast as a pointer to TrivialData)
 * \param marker (a pointer to an int containing the marker position)
 * \param oStart (a pointer to an int containing the option start point)
 * \param winner (a pointer to a string containing the winner text)
 * \param quit (a pointer to an int the denotes whether winner should be used)
 */
void display_scores(TrivialData *td, int *marker, int *oStart, char **winner, 
        int *quit);

/**\details
 * Display the questions
 *
 * Continue until a line contains a single full stop.
 *
 * \param td (a pointer that can be cast as a pointer to TrivialData)
 * \param counter (a pointer to an int that counts num lines in the question)
 * \param marker (a pointer to an int containing the marker position)
 * \param len (a pointer to an int the denotes the length of the buffer)
 */
void display_questions(TrivialData *td, int *counter, int *marker, int *len);

/**\details
 * Display the options
 *
 * Continue until all options have been displayed (when
 *
 * \param td (a pointer that can be cast as a pointer to TrivialData)
 * \param oStart (a pointer to an int containing the option start point)
 * \param counter (a pointer to an int that counts num lines in the question)
 * \param marker (a pointer to an int containing the marker position)
 */
void display_num_options(TrivialData *td, int *oStart, int *counter, 
        int *marker);

/**\details
 * Print the relevant error label
 *
 * \param curErrNum (an int containing the errNum for the first error)
 */
void print_error(int curErrNum);