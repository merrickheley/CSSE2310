/** 
 * \file   misc.c
 * \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 * \version 1.0
 * \brief  Contains miscellaneous functions used throughout the project
 * 
 * All commenting is designed to be compatible with Doxygen.
 */

#include "misc.h"

int connect_to (struct in_addr *ipAddress, int port) {
    struct sockaddr_in socketAddr;
    int fd;
    
    /* Create TCP socket */
    fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0) {
        fprintf(stderr, "Bad Server\n");
        exit(ERR_SERVER);
    }

    /* Set up an address structure that contains the address
     * (IP address and port number) that we're trying to connect to.
     */
    socketAddr.sin_family = AF_INET;	/* IP v4 */
    socketAddr.sin_port = htons(port);	/* port number in network byte order */
    socketAddr.sin_addr.s_addr = ipAddress->s_addr;	/* IP address */

    /* Attempt to connect to remote address */
    if (connect(fd, (struct sockaddr*) &socketAddr, sizeof(socketAddr)) < 0) {
        fprintf(stderr, "Bad Server\n");
        exit(ERR_SERVER);
    }
    
    return fd;
}

int get_line(int input, char **buffer) { 

    int count;
    *buffer = (char *) realloc(*buffer, sizeof(char));

    char c[2];

    count = 0;

    // Run while there are still characters to get
    while (read(input, c, 1) > 0) {

        // If the string is done, set the buffer
        if (c[0] == '\n' || c[0] == '\0') {
            return count + 1;
        } else {
            // Reallocate the buffer and increase its size by 1
            *buffer = (char*) realloc(*buffer, 
                    (size_t) sizeof(char) *(count+2) );

            // Assign the character read to the buffer
            c[1] = '\0';
            //strcat(*buffer, c);
            memcpy(*buffer+count, c, 2);
        }
        count++;
    }

    return 0;
}

int str_is_num( char *buffer, int len) {
    for (int i = 0; i < len - 1; i++) {
        if (buffer[i] < 48 || buffer[i] > 57) {
            return 0;
        }
    }

    return 1;
}

int name_to_IP_addr(char* host, struct in_addr *ipAddress) {
    struct addrinfo *addressInfo;

    /* Converting hostname into address information (IP address) */
    if(getaddrinfo(host, NULL, NULL, &addressInfo)) {
        ipAddress = NULL;
        return 0;
    }

    *ipAddress = ((struct sockaddr_in*)(addressInfo->ai_addr))->sin_addr;

    freeaddrinfo(addressInfo);

    return 1;
}

void send_message(int fd, char *message) {

    //printf("%s", message);

    if(write(fd, message, strlen(message)) < 1) {
        fprintf(stderr, "System Error\n");
        exit(ERR_SYS);
    }
}

void send_integer(int fd, int num) {

    char *message = (char *) malloc(sizeof(char) * (num_digits(num) + 1));

    sprintf(message, "%d", num);

    send_message(fd, message);

    free(message);
}

int num_digits(int num) {

    int digits = 0;

    if (num <= 0) {
        digits = 1;
    }

    while (num) {
        num /= 10;
        digits++;
    }

    return digits;
}