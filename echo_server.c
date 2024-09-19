#include "echo_server.h"

void startEchoServer()
{
    int server_socket, new_socket, client_socket[MAX_CLIENTS], max_clients = MAX_CLIENTS;
    int max_sd, sd, activity, valread;
    int opt = 1;
    int index;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];
    socklen_t addrlen;


    // Initialize all client_socket[] to 0
    for (index = 0; index < max_clients; index++)
    {
        client_socket[index] = 0;
    }

    // Create the server socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to allow multiple connections
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
        perror("Setsockopt failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Define the server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the specified port
    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_socket, 3) < 0)
    {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    fd_set readfds;

    // Just keep running unless an error occurs
    while (1)
    {
        // Clear the socket set and add the server socket
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);
        max_sd = server_socket;

        // Add child sockets to the set
        for (index = 0; index < max_clients; index++)
        {
            sd = client_socket[index];

            // Add valid socket descriptors to the set
            if (sd > 0) FD_SET(sd, &readfds);
            if (sd > max_sd) max_sd = sd;
        }

        // Wait for activity on one of the sockets
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            perror("Select error");
        }

        // If something happened on the master socket, it's an incoming connection
        if (FD_ISSET(server_socket, &readfds))
        {
            int addrlen = sizeof(address);
            if ((new_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
            {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }

            // Inform user of socket number
            printf("New connection, socket fd is %d, ip is : %s, port : %d\n",
                   new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // Add new socket to array of sockets
            for (index = 0; index < max_clients; index++)
            {
                if (client_socket[index] == 0)
                {
                    client_socket[index] = new_socket;
                    printf("Adding to list of sockets as %d\n", index);
                    break;
                }
            }
        }


        // Handle input/output from clients
        for (index = 0; index < max_clients; index++)
        {
            sd = client_socket[index];

            if (FD_ISSET(sd, &readfds))
            {
                // Check if it was for closing
                if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0)
                {
                    // Somebody disconnected, get details and close the socket
                    getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                    printf("Host disconnected, ip %s, port %d\n",
                           inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    close(sd);
                    client_socket[index] = 0;
                } else {
                    // Echo the message back to the client
                    buffer[valread] = '\0';
                    send(sd, buffer, valread, 0);
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