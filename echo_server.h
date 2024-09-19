#ifndef ECHO_SERVER_H
#define ECHO_SERVER_H

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <errno.h>

// Definitions
#define PORT 8888
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

// Functions
void startEchoServer();

#endif
