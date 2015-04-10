/* Author(s): Justin Liddicoat, Daniel Durazo, Tsosie Schneider
 *
 * This file contains code used to initialize a daemon
 * and terminating that daemon.
 */

#include "daemon.h"

void daemon_init(void){

    // Used to check fork() status (parent/child/failure)
    pid_t pid;

    // Used to check setsid() status (success/failure)
    pid_t sid;

    // File descriptor used when opening files/input/output
    int file;

    // Attempt to fork to create child
    pid = fork();

    // Something went wrong with fork!
    if(pid < 0){
        printf("Fork failure! \n");
        exit(EXIT_FAILURE);
    }

    // Parent is no longer needed, so exit.
    if(pid > 0){
        exit(EXIT_SUCCESS);
    }

    // Unmask the file mode to avoid issues with user's umask setting
    umask(0);

    // Attempt to create new session, become process group leader of session,
    // and have no controlling terminal
    sid = setsid();

    // Something went wrong getting a new session ID
    if(sid < 0){
        printf("setsid() failure! \n");
        exit(EXIT_FAILURE);
    }

    // Change working directory to root
    chdir("/");

    // Close stdin/stdout/stderr
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Redirect stdin/stdout/stderr to null
    file = open("/dev/null", O_RDWR); // stdin
    dup(file); // stdout
    dup(file); // stderr

    // Open the syslog connection with helpful options
    // Use a prefix that identifies the daemon
    // Request a print out of the PID
    // Set the default "facility" to daemon
    openlog("CS480 Daemon ", LOG_PID, LOG_DAEMON);

    // Allow daemon to be killed by calling "kill <pid of daemon>"
    signal(SIGTERM, daemon_exit);

}

void daemon_exit(int sig_number){

    // Send message to system log about closing the daemon
	syslog(LOG_NOTICE, "is now exiting... \n");

	// Close the syslog
	closelog();

	// Terminating successfully
	exit(EXIT_SUCCESS);
}
