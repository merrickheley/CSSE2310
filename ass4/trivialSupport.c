/** 
 * \file   trivial_support.c
 * \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 * \version 1.0
 * \brief  Contains supporting functions for trivial
 *
 * \details
 * 
 * All commenting is designed to be compatible with Doxygen.
 */

#include "trivialSupport.h"

int args_valid(int argc, char *argv[], int *port, struct in_addr *ipAddress) {
    char *host;

    // Check if 3 or 4 arguments are given
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "Usage: trivial name port [host]\n");
        return ERR_USAGE;
    }

    // Check if port is in the correct range
    if (sscanf(argv[2], "%d", port) != 1 || *port < 1 || *port > 65535) {
        fprintf(stderr, "Invalid Port\n");
        return ERR_PORT;
    }

    // Grab the hostname. If it is not given, assume localhost.
    if (argc == 4) {
        host = argv[3];
    } else {
        host = "localhost";
    }

    // Convert the hostname to an IP
    if(!name_to_IP_addr(host, ipAddress)) {
        fprintf(stderr, "Bad Server\n");
        return ERR_SERVER;
    }

    return 0;
}

void *get_client_input(void *arg) {
    TrivialData *td = (TrivialData *) arg;
    int guess;
    int c;

    // Loop while there is still data to be read
    while ((c = get_line(td->clientCon, &td->cBuffer)) > 0) {

        if (!str_is_num(td->cBuffer, c)) {
            printf("Invalid guess\n");
            continue;
        }

        sscanf(td->cBuffer, "%d", &guess);

        // If guess is valid and within range, send it to the server
        if (guess > 0 && guess <= td->numOptions) {
            send_message(td->serverCon, td->cBuffer);
            send_message(td->serverCon, "\n");
        } else {
            printf("Invalid guess\n");
        }
    }

    // Exit with EOF
    td->errNum = ERR_EOF;
    sem_post(&td->blocker);
    return NULL;
}

void *get_server_messages(void *arg) {
    TrivialData *td = (TrivialData *) arg;
    int marker = 0;
    
    // Grab the greeting line. Disconnect if it begins with $, otherwise print
    if (get_line(td->serverCon, &td->sBuffer)) {
        if (td->sBuffer[0] == '$') {
            td->errNum = ERR_FULL;
            sem_post(&td->blocker);
            return NULL;
        }

        printf("%s\n", td->sBuffer);
    }

    // Parse all other input and display it to the user
    parse_input(td, &marker);

    // If there was an error, quit
    if (marker == 1) {
        td->errNum = -1;
    } else {
        td->errNum = ERR_DC;
    }
    sem_post(&td->blocker);
    return NULL;
}

void parse_input(TrivialData *td, int *marker) {
    int c;
    char *winner;
    int counter = 1;
    int oStart = 0;
    int quit = 0;

    // Grab data line-by-line
    while ((c = get_line(td->serverCon, &td->sBuffer)) > 0) {
        switch (*marker) {  // Marker denote the sort of data being looked at
            case 0: // Scores data
                display_scores(td, marker, &oStart, &winner, &quit);
                break;
            case 1: // Question data
                display_questions(td, &counter, marker, &c);
                break;
            case 2: // Num options
                display_num_options(td, &oStart, &counter, marker);
                break;
            case 3: // Option data
                printf("%d: %s\n", counter-oStart, td->sBuffer);
                if (counter-oStart == td->numOptions) {
                    *marker = 4;
                    printf("++++\n");
                }
                break;
            case 4: // Final protocol information
                // If winners message, set the winners string for scores to
                // print later. Otherwise print the results string or error.
                if (td->sBuffer[0] == 'W') {
                    winner = (char *) malloc(sizeof(char) * 
                            (strlen(td->sBuffer)));
                    strcpy(winner, td->sBuffer+1);
                    quit = 1;
                } else if (td->sBuffer[0] == 'C') {
                    printf("Results: %s\n", td->sBuffer+1);
                    *marker = 0;
                    counter = 0;
                } else {
                    td->errNum = ERR_PROTOCOL;
                    sem_post(&td->blocker);
                }
                break;
        }
        counter++;
    }
}

void display_scores(TrivialData *td, int *marker, int *oStart, char **winner, 
        int *quit) {

    // Check if scores line
    if (td->sBuffer[0] != 'S') {
        td->errNum = ERR_PROTOCOL;
        sem_post(&td->blocker);
        return;
    }

    td->numOptions = 0;
    *oStart = 0;
    *marker = 1;
    printf("Scores: %s\n", td->sBuffer+1);

    // Display winners
    if (*quit) {
        printf("Winner(s): %s\n", *winner);
        free(*winner);
        td->errNum = -1;
        sem_post(&td->blocker);
    }
}

void display_questions(TrivialData *td, int *counter, int *marker, int *len) {
    if (*counter == 2) {
        printf("\n");
    } 

    // If the question is not complete, print the question
    if (*counter > 2 && !strcmp(td->sBuffer, ".")) {
        *marker = 2;
    } else {
        printf("%s\n", (*len > 1 ? td->sBuffer : ""));
    }

}

void display_num_options(TrivialData *td, int *oStart, int *counter, 
        int *marker) {
    
    // Grab the number of options, and set oStart
    sscanf(td->sBuffer, "%d", &td->numOptions);
    printf("=====\n");
    *oStart = *counter;
    *marker = 3;

    // If options is 0, end the display region straight away
    if (td->numOptions == 0) {
        *marker = 4;
        printf("++++\n");
    }
}

void print_error(int curErrNum) {
    switch (curErrNum) {
        case ERR_SYS:
            fprintf(stderr, "System Error\n");
            break;
        case ERR_EOF:
            fprintf(stderr, "Client EOF\n");
            break;
        case ERR_DC:
            fprintf(stderr, "Server Disconnected\n");
            break;
        case ERR_FULL:
            fprintf(stderr, "Server Full\n");
            break;
        case ERR_PROTOCOL:
            fprintf(stderr, "Protocol Error\n");
            break;
        default:
            curErrNum = 0;
    }
}