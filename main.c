/* Author(s): Justin Liddicoat, Daniel Durazo, Tsosie Schneider
 *
 * This file is the "driver" program.
 * First: call daemon_init() to become a daemon.
 * Any code following the init will run as daemon.
 * To close daemon call: kill <pid of daemon>
 * Information is sent to the system log "CS480 Daemon".
 */

#include "main.h"

//Protype the thread function
void *conn_handler(void *sock_descriptor);

int main(int argc, char *argv[]){

    // Variable declaration needed to setup echo server
    struct sockaddr_in serv_address, client_address;
    int sock_descriptor, sock_client;
    socklen_t add_size = sizeof(struct sockaddr_in);
    pthread_t thread;

	// Initialize this code to run as a daemon
	daemon_init();

	// Quick message to reference to daemon start & the PID (for calling kill)
	syslog(LOG_NOTICE, "is starting up... \n");

    // Sets values inside server_address second line sets the value for localhost
    // The third line sets the port to 8080
    serv_address.sin_family = AF_INET;
    serv_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_address.sin_port = htons(8080);

    // This conditional sets the socket descriptor on
    // error the problem is written to syslog and exit daemon
    if ((sock_descriptor = socket(AF_INET, SOCK_STREAM, 0))==-1){
        syslog(LOG_ERR, "Failed to create socket.");
        exit(EXIT_FAILURE);
    }

    // This conditional binds sock_descriptor to serv_address
    // on Error write to syslog about bind error exit daemon
    if (bind(sock_descriptor, (struct sockaddr *)&serv_address, add_size)==-1){
        syslog(LOG_ERR, "Failed to bind.");
        exit(EXIT_FAILURE);
    }

    // This function marks sock_descriptor as listening
    // this it can now accept incoming connections
    listen(sock_descriptor,0);

    // Infinite loop that spawns new threads for
    // incoming connections. Errors that occur inside
    // this loop are logged but the daemon does not exit
    // on failure as it seems bit silly to end a daemon
    // due to connection/thread errors when the client would
    // just try to reconnect
	while(TRUE){
        // function accepts and incoming connection and returns a file
        // descriptor to be used when a thread is created
        sock_client = accept(sock_descriptor, (struct sockaddr *)&client_address, &add_size);

        // Check to make sure accept returned a valid fd
        if (sock_client == -1){
            syslog(LOG_ERR, "Connection not accepted.");
        }
        else{
            // Conditional that attempts to spawn a new thread
            // on error syslog is informed
            if (pthread_create(&thread, NULL, conn_handler, (void *)&sock_client) == -1){
                syslog(LOG_ERR, "Thread creation error.");
            }
            else{
            // upon successful creation the thread is detached from the parent
            pthread_detach(thread);
            }
        }

	}
	// Should only get her if daemon is killed by id
	return 0;
}

// This is the function that handles the new connections
void *conn_handler(void *sock_descriptor){
    // variable declarations for thread funcitons
    int read_in, i, err;
    char buffer[80];
    char letter[2];
    // cast to get file descriptor or new connection
    int sock = *(int *)sock_descriptor;
    // loop that only ends when q is put into input stream
    while(TRUE){
        // clears the buffer on each loop
        memset(buffer, '\0', sizeof(buffer));

        //reads the data coming from the client
        read_in = read(sock, buffer, sizeof(buffer));
        i=0;
        // This loop goes through input stream char by char
        while (i<read_in){
            // clears the letter buffer on each loop
            memset(letter, '\0', sizeof(letter));

            // When q is found in the input stream the connection to
            // the client is closed and the thread is terminated
            if (buffer[i] == 'q'){
                close(sock);
                pthread_exit(0);
            }
            // character in buffer is inserted into letter array
            letter[0]=buffer[i];

            // write single character to client
            err = write(sock, letter, sizeof(letter));

            // checks for write error if found writes to syslog
            if (err<0){
                syslog(LOG_ERR, "Failed to write to socket.");
            }
            // increment loop counter
            i++;
        }
    }
}
