/* Author(s): Justin Liddicoat, Daniel Durazo, Tsosie Schneider
 *
 * This file is the "server" program.
 * First: call daemon_init() to become a daemon.
 * Any code following the init will run as daemon.
 * To close daemon call: kill <pid of daemon>
 * Information is sent to the system log "CS480 Daemon".
 */

#include "server.h"

int main(int argc, char *argv[]){

    // Variable declaration needed to setup echo server
    struct sockaddr_in serv_address, client_address;
    int sock_descriptor, sock_client;
    socklen_t add_size = sizeof(struct sockaddr_in);
    pthread_t thread;

	// Initialize this code to run as a daemon
	daemon_init();

	// Quick message to reference to daemon start & the PID (for calling kill)
	syslog(LOG_NOTICE, "is starting up...");

    // Setup listening on localhost and port 8080
    serv_address.sin_family = AF_INET;
    serv_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_address.sin_port = htons(8080);

    // Set the socket descriptor
    if ((sock_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1){

        // If error, send message to syslog and exit
        syslog(LOG_ERR, "Failed to create socket. Exiting...");
        exit(EXIT_FAILURE);
    }

    // Bind the sock_descriptor to the serv_address
    if (bind(sock_descriptor, (struct sockaddr *)&serv_address, add_size) == -1){

        // If error, send message to syslog and exit
        syslog(LOG_ERR, "Failed to bind. Exiting...");
        exit(EXIT_FAILURE);
    }

    // Marks sock_descriptor as listening and accepting incoming connections
    listen(sock_descriptor, 0);

    // Infinite loop that spawns new threads for incoming connections.
    // Connection/thread errors are sent to system log and attempted again.
	while(TRUE){

        // Accept incoming connection and get file descriptor to use when thread is created
        sock_client = accept(sock_descriptor, (struct sockaddr *)&client_address, &add_size);

        // Check to make sure accept returned a valid file descriptor
        if (sock_client == -1){

            // Something went wrong, log the error, get ready for more connections
            syslog(LOG_ERR, "Connection error.");

        } else {

            // Attempt to spawn a new thread,
            if (pthread_create(&thread, NULL, conn_handler, (void *)&sock_client) == -1){

                // Thread creation failure, log the error, get ready for other connections
                syslog(LOG_ERR, "Thread creation error.");

            } else {

                // Log the successful connection & thread creation
                syslog(LOG_INFO, "Client successfully connected.");

                // Detach thread is detached from the parent
                pthread_detach(thread);
            }
        }
	}
	// Getting here should not be possible
	return EXIT_FAILURE;
}

// This is the function that handles the new connections
void *conn_handler(void *sock_descriptor) {

    // Variable declarations for thread functions
    int num_bytes_read, i, status;
    char buffer [100];
    char send_me[1];

    // Cast to get file descriptor of connection
    int sock = *(int *)sock_descriptor;

    // Continually get input until client sends any 'q'
    while(TRUE){

        // Attempt to read data coming from the client into the buffer.
        // Capture the number of bytes read to iterate through.
        num_bytes_read = read(sock, buffer, 100);

        // Initialize loop counter
        i = 0;

        // This loop goes through input stream char by char
        while (i < num_bytes_read){

            // Copy the char to a temporary array
            *send_me = buffer[i];

            // Write single character back to client
            status = write(sock, send_me, sizeof(char));

            // Check the result of the write to socket
            if (status < 0){

                // Something went wrong, log error
                syslog(LOG_ERR, "Failed to write to socket.");
            }

            // Quit when a 'q' character is found.
            // It has already been sent back to the client so it can exit/disconnect too.
            if (buffer[i] == 'q'){

                // Close the socket and exit thread
                close(sock);
                pthread_exit(EXIT_SUCCESS);
            }

            // Otherwise keep processing input
            i++;
        }
    }
}
