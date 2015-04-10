/* Author(s): Justin Liddicoat, Daniel Durazo, Tsosie Schneider
 *
 * This file is the "driver" program.
 * First: call daemon_init() to become a daemon.
 * Any code following the init will run as daemon.
 * To close daemon call: kill <pid of daemon>
 * Information is sent to the system log "CS480 Daemon".
 */

#include "main.h"


int main(void){

	// Initialize this code to run as a daemon
	daemon_init();

	// Variable to record how many seconds daemon has been running
	int time = 0;

	// Quick message to reference to daemon start & the PID (for calling kill)
	syslog(LOG_NOTICE, "is starting up... \n");

    // Infinitely loop to keep daemon running
	while(TRUE){

        // Take breaks to avoid swamping out syslog
        sleep(30);

        // Keep track of approximate run time
		time += 30;

        // The major "task" for now, simple info
		syslog(LOG_INFO, "has been running for %d seconds \n", time);

	}
}
