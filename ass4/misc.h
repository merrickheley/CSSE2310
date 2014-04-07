/** 
 * \file   misc.h
 * \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 * \version 1.0
 * \brief  header file for misc.c
 *
 * \details
 * 
 * All commenting is designed to be compatible with Doxygen.
 */

#include <arpa/inet.h>
#include <ctype.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define ERR_USAGE 1
#define ERR_NUM 2
#define ERR_FILE 3
#define ERR_PORT 4
#define ERR_SERVER 5
#define ERR_LISTEN 6
#define ERR_CLIENT 7
#define ERR_SYS 8
#define ERR_EOF 9
#define ERR_DC 10
#define ERR_FULL 11
#define ERR_PROTOCOL 12

#define TRUE 1
#define FALSE 0

int connect_to(struct in_addr* ipAddress, int port);
int get_line(int input, char **buffer);
int name_to_IP_addr(char* host, struct in_addr *ipAddress);
int str_is_num(char *buffer, int len);
void send_message(int fd, char *message);
void send_integer(int fd, int num);
int num_digits(int num);