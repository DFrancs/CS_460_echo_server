#include "echo_server.h"

void startEchoServer()
{
    int server_socket;                     // Socket file descriptor for the server
    int new_socket;                        // Socket file descriptor for a new incoming connection
    int client_socket[MAX_CLIENTS];        // Array to hold client socket file descriptors
    int max_clients = MAX_CLIENTS;         // Maximum number of clients allowed
    int max_sd;                            // Maximum socket descriptor value
    int sd;                                // Temporary socket descriptor
    int activity;                          // Number of ready descriptors returned by select()
    int valread;                           // Number of bytes read by recv()
    int opt = 1;                           // Option for setsockopt()
    int index = 0;                         // Indexing for for loops
    struct sockaddr_in address;            // Structure to hold server address information
    char buffer[BUFFER_SIZE];              // Buffer to hold data received from clients
    socklen_t addrlen;                     // Size of the address structure

    // Initialize all client_socket[] to 0
    for ( index = 0; index < max_clients; index++ )
    {
        client_socket[i] = 0;
    }

    // Create the server socket
    if ( ( server_socket = socket( AF_INET, SOCK_STREAM, 0 ) ) == 0 ) // Error occurs if function returns == 0
    {
        perror( "Socket failed" );
        exit( EXIT_FAILURE );
    }

    // Set socket options to allow multiple connections
    if ( setsockopt( server_socket, SOL_SOCKET, SO_REUSEADDR, ( char * )&opt, sizeof( opt ) ) < 0 ) // Error occurs if function returns < 0
    {
        perror( "Setsockopt failed" );
        close( server_socket );
        exit( EXIT_FAILURE );
    }

    // Define the server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Bind the socket to the specified port
    if ( bind( server_socket, ( struct sockaddr * )&address, sizeof( address ) ) < 0 ) // Error occurs if function returns < 0
    {
        perror( "Bind failed" );
        close( server_socket );
        exit( EXIT_FAILURE );
    }

    // Start listening for incoming connections
    if ( listen( server_socket, 3 ) < 0 ) // Error occurs if function returns < 0
    {
        perror( "Listen failed" );
        close( server_socket );
        exit( EXIT_FAILURE );
    }

    printf( "Server listening on port %d\n", PORT );

    fd_set readfds;

    while (1)
    {
        // Clear the socket set and add the server socket
        FD_ZERO( &readfds );
        FD_SET( server_socket, &readfds );
        max_sd = server_socket;

        // Add child sockets to the set
        for ( index = 0; index < max_clients; index++ )
        {
            sd = client_socket[i];

            // Add valid socket descriptors to the set
            if ( sd > 0 ) FD_SET( sd, &readfds );
            if ( sd > max_sd ) max_sd = sd;
        }

        // Wait for activity on one of the sockets
        activity = select( max_sd + 1, &readfds, NULL, NULL, NULL );

        if ( ( activity < 0 ) && ( errno != EINTR ) )
        {
            perror( "Select error" );
        }

        // If something happened on the master socket, it's an incoming connection
        if ( FD_ISSET( server_socket, &readfds ) )
        {
            addrlen = sizeof( address );
            if ( ( new_socket = accept( server_socket, ( struct sockaddr * )&address,
                    ( socklen_t * )&addrlen ) ) < 0 ) // Error occurs if function returns < 0
            {
                perror( "Accept failed" );
                exit( EXIT_FAILURE );
            }

            // Inform user of socket number
            printf( "New connection, socket fd is %d, ip is : %s, port : %d\n",
                   new_socket, inet_ntoa( address.sin_addr ), ntohs( address.sin_port ) );

            // Add new socket to array of sockets
            for ( index = 0; index < max_clients; index++ )
            {
                if ( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    printf( "Adding to list of sockets as %d\n", i );
                    break;
                }
            }
        }

        // Handle input/output from clients
        for ( index = 0; index < max_clients; index++ )
        {
            sd = client_socket[i];

            if ( FD_ISSET( sd, &readfds ) )
            {
                // Check if it was for closing
                if ( ( valread = recv( sd, buffer, BUFFER_SIZE, 0 ) ) == 0 )
                {
                    // Somebody disconnected, get details and close the socket
                    getpeername( sd, ( struct sockaddr * )&address, ( socklen_t * )&addrlen );
                    printf( "Host disconnected, ip %s, port %d\n",
                           inet_ntoa( address.sin_addr ), ntohs( address.sin_port ) );

                    close( sd );
                    client_socket[index] = 0;
                }
                else
                {
                    // Echo the message back to the client
                    buffer[valread] = '\0';
                    send( sd, buffer, valread, 0 );
                }
            }
        }
    }
}

int main()
{
    startEchoServer();
    return 0;
}