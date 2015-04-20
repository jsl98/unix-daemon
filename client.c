/* Author(s): Justin Liddicoat, Daniel Durazo, Tsosie Schneider
 *
 * This file contains everything needed for the
 * client program to connect to the server.
 * User entered text is sent to the server,
 * which the server echo's back. This client
 * receives the echo and prints it back out.
 */

#include "client.h"

int main(int argc, char* argv[]){

    // Variable declaration needed to setup echo server
    struct sockaddr_in serv_address;
    int sock_client, status, i;

    // Place to store user input
    char send_buffer[100];
    char output[100];
    char recv_buffer[1];

    // Setup listening on localhost and port 8080
    serv_address.sin_family = AF_INET;
    serv_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_address.sin_port = htons(8080);

    // Display client text to user
    printf("Connecting to Echo Server @ 127.0.0.1\n");

    // Attempt to allocate a socket descriptor
    if ( (sock_client = socket(AF_INET, SOCK_STREAM, 0)) == -1) {

        // Something went wrong, show user and exit
        printf("Error creating socket descriptor!\n");
        exit(EXIT_FAILURE);
    };

    // Attempt to connect socket to the server
    if ( (connect(sock_client, (struct sockaddr *)&serv_address, sizeof(serv_address))) == -1) {

        // Connection problem, show error and exit
        printf("Error connecting socket to server!\n");
        exit(EXIT_FAILURE);
    }

    // Inform client user vital information
    printf("Connection success!\n");
    printf("Enter text to send to server: (type 'q' to quit)\n");

    // Loop until quit condition is met
    while(TRUE){

        // Get user input
        fgets(send_buffer, sizeof(send_buffer), stdin);

        // Loop through the input string
        i = 0;
        while(send_buffer[i]) { // While not the null terminator

            // Replace the new-line char with a null terminator
            if(send_buffer[i] == '\n') {send_buffer[i] = '\0';}

            // Until end of string
            i++;
        }

        // Now 'i' contains the number of chars to transmit to server
        // Attempt to send the buffer contents to server
        status = send(sock_client, send_buffer, i, 0);
        if( status == -1) {

            // Something went wrong while sending
            printf("Send failure!\n");
        }

        // Await the reply of the echo server, storing char by char into buffer
        // until receiving a 'q' or null-terminator
        i = 0;

        // Expecting one char at a time: server is sending one char at a time
        while(recv(sock_client, recv_buffer, sizeof(char), 0) == sizeof(char)) {

            // Copy the received char into the output buffer
            output[i] = *recv_buffer;

            // Check if this is the end of the string
            if(output[i] == '\0') {

                // End of string, so break out of receive loop
                break;
            }

            // Check if this char is 'q' to signify request to quit
            if (output[i] == 'q') {

                // Quit requested, so terminate the string
                output[++i] = '\0';

                // Print out the string up until the quit request
                puts(output);
                printf("\n");

                // Exit client
                exit(EXIT_SUCCESS);
            }

            // Else continue to grab input
            i++;
        }

        // Done grabbing input from server: print the received buffer contents
        puts(output);

    } // Loop to get user input and await server echo

    // Should not get here normally
    return(EXIT_FAILURE);
}
