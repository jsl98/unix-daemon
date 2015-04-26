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
    Container content;
    int sock_descriptor, sock_client;
    socklen_t add_size = sizeof(struct sockaddr_in);
    pthread_t thread;
    pthread_mutex_t loch;

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

    // Set shared memory for threads
    content.memory = (int *)malloc(1000*sizeof(int));
    // Initialize count vaiable
    int i = 0;
    // set shared array to values to zero
    while (i<1000){
        content.memory[i++] = 0;
    }
    // set the mutex lock location
    content.lock = loch;
    // initialize the mutex
    pthread_mutex_init(&content.lock, NULL);

    // Marks sock_descriptor as listening and accepting incoming connections
    listen(sock_descriptor, 0);
//    printf("%p %p \n", content.memory, loch);
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
            // set the socket desciptor inside the container stucture to be passed to thread
            content.sock_descriptor = sock_client;
            // Attempt to spawn a new thread,
            if (pthread_create(&thread, NULL, conn_handler, (void *)&content) == -1){

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
	pthread_mutex_destroy(&content.lock);
	// Getting here should not be possible
	return EXIT_FAILURE;
}

// This is the function that handles the new connections
void *conn_handler(void *sock_descriptor) {
    // Variable declarations for thread functions
    Container *temp_box,box;

    // Cast the argument into a usable container structure
    temp_box = (Container *)sock_descriptor;
    box = *temp_box;
    int num_bytes_read, status, buffer, steps;
//    char send_me[1];

    // set socket descriptor to local variable
    int sock = box.sock_descriptor;

    // Continually get input until client sends any 'q'
    while(TRUE){

//        buffer = 0;
        // Attempt to read data coming from the client into the buffer.
        // Capture the number of bytes read to iterate through.
        num_bytes_read = read(sock, &buffer, sizeof(buffer));

        // Closes the  socket when a read error is returned
        if (num_bytes_read ==-1){
            syslog(LOG_ERR, "Failed to read from socket, closing connection.");
            close(sock);
            pthread_exit((void *)EXIT_FAILURE);
        }
        // closes the connection when the client indicated too end session
        if (buffer < 1){
            // Close the socket and exit thread
            close(sock);
            pthread_exit((void *)EXIT_SUCCESS);
        }

        // call to 3 a plus one algorithm to get step #
        steps = threeAplusOne(buffer, 0, box.memory);

        // mutex lock for read and write to array
        // though reading is not critical it was left inside the
        // the lock to prevent multiple writes to the array
        pthread_mutex_lock(&box.lock);
        // buffer offset by one to account for 0 index
        if (buffer-1 < 1000 && box.memory[buffer-1]==0){
            // sets step count for shared array to memorize
            // previously compute numbers
            box.memory[buffer-1]=steps;
        }
        //unclock the mutex
        pthread_mutex_unlock(&box.lock);
        // send the number of steps back to the client
        status = write(sock, &steps, sizeof(steps));
        if (status < 0){
            // Something went wrong, log error
            syslog(LOG_ERR, "Failed to write to socket.");
            }
    }
}

int threeAplusOne(int start, int total, int* memory){
    // base case to indicate error, server code should filter
    // these cases
    if (start<1){
        return -1;
    }
    // base case to utilize memorization functionality
    if (start-1<1000 && memory[start-1]){
        return total + memory[start-1];
    }

    // base case when the function reaches one returns the total steps the function took.
    if (start == 1){
        return total;
    }
    // recursive call when a number is even value divided by 2 and step count incremente
    if (start%2==0){
        return threeAplusOne(start/2, total+1, memory);
    }
    // recursive call when a number is odd 3 times value plus 1 step count incremente
    else{
        return threeAplusOne((start*3)+1,total+1, memory);
    }
}
