/** 
 * \file   scores.c
 * \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 * \version 1.0
 * \brief  Outputs the scores table produced by serv
 *
 * \details
 * 
 * scores is a command line c program that displays data sent by serv.
 * This data will typically have the form:
 *
 * %s played:%d won:%d disc:%d score:%d
 *
 * However this is not guaranteed
 *
 * All commenting is designed to be compatible with Doxygen.
 */

#include "misc.h"

int main(int argc, char* argv[]) {
    int fd;
    int port;
    struct in_addr ipAddress;
    char *host;
    char *buffer = (char*) malloc(sizeof(char));

    // Check the arguments
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "Usage: scores port [host]\n");
        return ERR_USAGE;
    }
    
    if (sscanf(argv[1], "%d", &port) != 1 || port < 1 || port > 65535) {
        fprintf(stderr, "Invalid Port\n");
        return ERR_PORT;
    }

    // If host name is not specified, assume localhost
    if (argc == 3) {
        host = argv[2];
    } else {
        host = "localhost";
    }
    
    // Resolve the hostname
    if (!name_to_IP_addr(host, &ipAddress)) {
        fprintf(stderr, "Bad Server\n");
        return ERR_SERVER;
    }
    
    // Connect to the server and identify as scores
    fd = connect_to(&ipAddress, port);
    send_message(fd, "scores\n");

    // Output any data from the server
    while (get_line(fd, &buffer)) {
        printf("%s\n", buffer);
    }

    // Clean up and quit
    free(buffer);
    close(fd);

    return 0;
}