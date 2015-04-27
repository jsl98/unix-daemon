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

    int send_buffer, err;

    char output[100];

    int recv_buffer;

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
    printf("Enter Integers to send to server: (type an integer < 1 to quit)\n");

    // Loop until quit condition is met
    while(TRUE){

        // Get user integer input
        err = scanf("%i", &send_buffer);
        // Conditional to test if characters were entered
        if (err == 0){
        // indicated input error and clears scanf buffer
            printf("Invalid input.\n");
            while (getchar() != '\n');
        }
        // Input less than 1 indicates a desire to close the connection
        if (send_buffer < 1){
            // send server indicator to close connection
            status = send(sock_client, &send_buffer, sizeof(send_buffer), 0);
            // prompts user to try again if the server was not informe to close connection
            if (status == -1)
                printf("Failed to close connection please try again.\n");
            else
                // closes the file desciptor for the socket
                close(sock_client);
                // exit the client successfully
                return (EXIT_SUCCESS);
        }
        else{
            // send the integer to the server
            status = send(sock_client, &send_buffer, sizeof(send_buffer), 0);

            //
            if( status == -1) {
                // Something went wrong while sending
                printf("Send failure!\n");
            }else{

            // Await the reply of the echo server
            recv(sock_client, &recv_buffer, sizeof(recv_buffer), 0);
            // print the integer response to user
            printf("%i\n",recv_buffer);
            }
        }

    } // Loop to get user input and await server echo

    // Should not get here normally
    return(EXIT_FAILURE);
}
